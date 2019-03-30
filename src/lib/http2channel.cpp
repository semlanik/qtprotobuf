/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
 *
 * This file is part of qtprotobuf project https://git.semlanik.org/semlanik/qtprotobuf
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "http2channel.h"

//FIXME: workaround for build issue
#undef QT_LINKED_OPENSSL

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QTimer>
#include <QtEndian>

#include <unordered_map>

#include "qtprotobuflogging.h"

using namespace qtprotobuf;

namespace qtprotobuf {

struct Http2ChannelPrivate {
    QUrl url;
    QNetworkAccessManager nm;
};

}

namespace  {
const static std::unordered_map<QNetworkReply::NetworkError, AbstractChannel::StatusCodes> StatusCodeMap = { { QNetworkReply::ConnectionRefusedError, AbstractChannel::Unavailable },
                                                                { QNetworkReply::RemoteHostClosedError, AbstractChannel::Unavailable },
                                                                { QNetworkReply::HostNotFoundError, AbstractChannel::Unavailable },
                                                                { QNetworkReply::TimeoutError, AbstractChannel::DeadlineExceeded },
                                                                { QNetworkReply::OperationCanceledError, AbstractChannel::Unavailable },
                                                                { QNetworkReply::SslHandshakeFailedError, AbstractChannel::PermissionDenied },
                                                                { QNetworkReply::TemporaryNetworkFailureError, AbstractChannel::Unknown },
                                                                { QNetworkReply::NetworkSessionFailedError, AbstractChannel::Unavailable },
                                                                { QNetworkReply::BackgroundRequestNotAllowedError, AbstractChannel::Unknown },
                                                                { QNetworkReply::TooManyRedirectsError, AbstractChannel::Unavailable },
                                                                { QNetworkReply::InsecureRedirectError, AbstractChannel::PermissionDenied },
                                                                { QNetworkReply::UnknownNetworkError, AbstractChannel::Unknown },
                                                                { QNetworkReply::ProxyConnectionRefusedError, AbstractChannel::Unavailable },
                                                                { QNetworkReply::ProxyConnectionClosedError, AbstractChannel::Unavailable },
                                                                { QNetworkReply::ProxyNotFoundError, AbstractChannel::Unavailable },
                                                                { QNetworkReply::ProxyTimeoutError, AbstractChannel::DeadlineExceeded },
                                                                { QNetworkReply::ProxyAuthenticationRequiredError, AbstractChannel::Unauthenticated },
                                                                { QNetworkReply::UnknownProxyError, AbstractChannel::Unknown },
                                                                { QNetworkReply::ContentAccessDenied, AbstractChannel::PermissionDenied },
                                                                { QNetworkReply::ContentOperationNotPermittedError, AbstractChannel::PermissionDenied },
                                                                { QNetworkReply::ContentNotFoundError, AbstractChannel::NotFound },
                                                                { QNetworkReply::AuthenticationRequiredError, AbstractChannel::PermissionDenied },
                                                                { QNetworkReply::ContentReSendError, AbstractChannel::DataLoss },
                                                                { QNetworkReply::ContentConflictError, AbstractChannel::InvalidArgument },
                                                                { QNetworkReply::ContentGoneError, AbstractChannel::DataLoss },
                                                                { QNetworkReply::UnknownContentError, AbstractChannel::Unknown },
                                                                { QNetworkReply::ProtocolUnknownError, AbstractChannel::Unknown },
                                                                { QNetworkReply::ProtocolInvalidOperationError, AbstractChannel::Unimplemented },
                                                                { QNetworkReply::ProtocolFailure, AbstractChannel::Unknown },
                                                                { QNetworkReply::InternalServerError, AbstractChannel::Internal },
                                                                { QNetworkReply::OperationNotImplementedError, AbstractChannel::Unimplemented },
                                                                { QNetworkReply::ServiceUnavailableError, AbstractChannel::Unavailable },
                                                                { QNetworkReply::UnknownServerError, AbstractChannel::Unknown }};

const char *GrpcAcceptEncodingHeader = "grpc-accept-encoding";
const char *AcceptEncodingHeader = "accept-encoding";
const char *TEHeader = "te";
const char *GrpcStatusHeader = "grpc-status";
}

Http2Channel::Http2Channel(const QString &addr, quint16 port) : AbstractChannel()
  , d(new Http2ChannelPrivate)
{
    d->url.setScheme("http");
    d->url.setHost(addr, QUrl::StrictMode);
    d->url.setPort(port);
}

Http2Channel::~Http2Channel()
{
    delete d;
}

AbstractChannel::StatusCodes Http2Channel::call(const QString &method, const QString &service, const QByteArray &args, QByteArray &ret)
{
    QUrl callUrl = d->url;
    callUrl.setPath("/" + service + "/" + method);

    qProtoDebug() << "Service call url: " << callUrl;
    QNetworkRequest request(callUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/grpc");
    request.setRawHeader(GrpcAcceptEncodingHeader, "identity,deflate,gzip");
    request.setRawHeader(AcceptEncodingHeader, "identity,gzip");
    request.setRawHeader(TEHeader, "trailers");

    request.setAttribute(QNetworkRequest::Http2DirectAttribute, true);

    QByteArray msg(5, '\0');
    *(unsigned int*)(msg.data() + 1) = qToBigEndian(args.size());
    msg += args;
    qProtoDebug() << "SEND: " << msg.toHex();
    QNetworkReply *reply = d->nm.post(request, msg);
    QEventLoop loop;
    QTimer timer;
    loop.connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    loop.connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    //TODO: Add configurable timeout logic
    timer.setInterval(1000);
    timer.start();

    if (!reply->isFinished()) {
        loop.exec();
    }

    timer.stop();

    //Check if no network error occured
    if (reply->error() != QNetworkReply::NoError) {
        return StatusCodeMap.at(reply->error());
    }

    //Check if response timeout triggered
    if (!reply->isFinished()) {
        reply->abort();
        return AbstractChannel::DeadlineExceeded;
    }

    //Check if server answer with error
    StatusCodes grpcStatus = static_cast<StatusCodes>(reply->rawHeader(GrpcStatusHeader).toInt());
    if (grpcStatus != StatusCodes::Ok) {
        return grpcStatus;
    }

    ret = reply->readAll();
    qProtoDebug() << "RECV: " << ret.toHex();
    return StatusCodes::Ok;
}
