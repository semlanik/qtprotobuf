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
#include "asyncreply.h"
#include "abstractclient.h"

#include <unordered_map>

#include "qtprotobuflogging.h"

using namespace qtprotobuf;

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

namespace qtprotobuf {

struct Http2ChannelPrivate {
    QUrl url;
    QNetworkAccessManager nm;
    QNetworkReply* post(const QString &method, const QString &service, const QByteArray &args, bool stream = false) {
        QUrl callUrl = url;
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

        QNetworkReply* networkReply = nm.post(request, msg);

        if (!stream) {
            //TODO: Add configurable timeout logic
            QTimer::singleShot(6000, networkReply, [networkReply]() {
                Http2ChannelPrivate::abortNetworkReply(networkReply);
            });
        }
        return networkReply;
    }

    static void abortNetworkReply(QNetworkReply* networkReply) {
        if (networkReply->isRunning()) {
            networkReply->abort();
        }
    }

    static QByteArray processReply(QNetworkReply* networkReply, AbstractChannel::StatusCodes& statusCode) {
        //Check if no network error occured
        if (networkReply->error() != QNetworkReply::NoError) {
            qProtoWarning() << "Network error occured" << networkReply->errorString();
            statusCode = StatusCodeMap.at(networkReply->error());
            return {};
        }

        //Check if server answer with error
        statusCode = static_cast<AbstractChannel::StatusCodes>(networkReply->rawHeader(GrpcStatusHeader).toInt());
        if (statusCode != AbstractChannel::StatusCodes::Ok) {
            qProtoWarning() << "Protobuf server error occured" << networkReply->errorString();
            return {};
        }

        //Message size doesn't matter for now
        return networkReply->readAll().mid(5);
    }
};

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
    QEventLoop loop;

    QNetworkReply *networkReply = d->post(method, service, args);
    QObject::connect(networkReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    //If reply was finished in same stack it doesn't matter to start event loop
    if (!networkReply->isFinished()) {
        loop.exec();
    } else {
        return AbstractChannel::DeadlineExceeded;
    }

    StatusCodes grpcStatus = StatusCodes::Unknown;
    ret = d->processReply(networkReply, grpcStatus);

    qProtoDebug() << __func__ << "RECV: " << ret.toHex();
    return grpcStatus;
}

void Http2Channel::call(const QString &method, const QString &service, const QByteArray &args, qtprotobuf::AsyncReply *reply)
{
    QNetworkReply *networkReply = d->post(method, service, args);

    auto connection = QObject::connect(networkReply, &QNetworkReply::finished, reply, [reply, networkReply]() {
        StatusCodes grpcStatus = StatusCodes::Unknown;
        QByteArray data = Http2ChannelPrivate::processReply(networkReply, grpcStatus);
        qProtoDebug() << "RECV: " << data;
        if (grpcStatus != StatusCodes::Ok) {
            reply->setData({});
            reply->error(grpcStatus);
            reply->finished();
            return;
        }
        reply->setData(data);
        reply->finished();
    });

    QObject::connect(reply, &AsyncReply::error, networkReply, [reply, networkReply, connection]() {
        QObject::disconnect(connection);
        Http2ChannelPrivate::abortNetworkReply(networkReply);
    });
}

void Http2Channel::subscribe(const QString &method, const QString &service, const QByteArray &args, AbstractClient *client, const std::function<void (const QByteArray &)> &handler)
{
    QNetworkReply *networkReply = d->post(method, service, args, true);

    auto connection = QObject::connect(networkReply, &QNetworkReply::readyRead, client, [networkReply, handler]() {
        handler(networkReply->readAll().mid(5));
    });

    QObject::connect(client, &AbstractClient::destroyed, networkReply, [client, networkReply, connection](){
        QObject::disconnect(connection);
        Http2ChannelPrivate::abortNetworkReply(networkReply);
    });
//TODO: implement error handling
//    QObject::connect(networkReply, &QNetworkReply::error, networkReply, [networkReply, connection](QNetworkReply::NetworkError) {
//        QObject::disconnect(connection);
//        Http2ChannelPrivate::abortNetworkReply(networkReply);
//    });
}

void Http2Channel::abort(AsyncReply *reply)
{
    assert(reply != nullptr);
    reply->setData({});
    reply->error(StatusCodes::Aborted);
    reply->finished();
}
