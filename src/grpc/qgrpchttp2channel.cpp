/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Viktor Kopp <vifactor@gmail.com>
 *
 * This file is part of QtProtobuf project https://git.semlanik.org/semlanik/qtprotobuf
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

#include "qgrpchttp2channel.h"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QTimer>
#include <QtEndian>
#include <QMetaObject>

#include <unordered_map>

#include "qgrpcasyncreply.h"
#include "qgrpcstream.h"
#include "qabstractgrpcclient.h"
#include "qgrpccredentials.h"
#include "qprotobufserializerregistry_p.h"
#include "qtprotobuflogging.h"

#include <qglobal.h>

using namespace QtProtobuf;

namespace  {

/*!
 * This QNetworkReply::NetworkError -> QGrpcStatus::StatusCode mapping should be kept in sync with original
 * <a href="https://github.com/grpc/grpc/blob/master/doc/statuscodes.md">gRPC status codes</a>
 */
const static std::unordered_map<QNetworkReply::NetworkError, QGrpcStatus::StatusCode> StatusCodeMap = {
                                                                { QNetworkReply::ConnectionRefusedError, QGrpcStatus::Unavailable },
                                                                { QNetworkReply::RemoteHostClosedError, QGrpcStatus::Unavailable },
                                                                { QNetworkReply::HostNotFoundError, QGrpcStatus::Unavailable },
                                                                { QNetworkReply::TimeoutError, QGrpcStatus::DeadlineExceeded },
                                                                { QNetworkReply::OperationCanceledError, QGrpcStatus::Unavailable },
                                                                { QNetworkReply::SslHandshakeFailedError, QGrpcStatus::PermissionDenied },
                                                                { QNetworkReply::TemporaryNetworkFailureError, QGrpcStatus::Unknown },
                                                                { QNetworkReply::NetworkSessionFailedError, QGrpcStatus::Unavailable },
                                                                { QNetworkReply::BackgroundRequestNotAllowedError, QGrpcStatus::Unknown },
                                                                { QNetworkReply::TooManyRedirectsError, QGrpcStatus::Unavailable },
                                                                { QNetworkReply::InsecureRedirectError, QGrpcStatus::PermissionDenied },
                                                                { QNetworkReply::UnknownNetworkError, QGrpcStatus::Unknown },
                                                                { QNetworkReply::ProxyConnectionRefusedError, QGrpcStatus::Unavailable },
                                                                { QNetworkReply::ProxyConnectionClosedError, QGrpcStatus::Unavailable },
                                                                { QNetworkReply::ProxyNotFoundError, QGrpcStatus::Unavailable },
                                                                { QNetworkReply::ProxyTimeoutError, QGrpcStatus::DeadlineExceeded },
                                                                { QNetworkReply::ProxyAuthenticationRequiredError, QGrpcStatus::Unauthenticated },
                                                                { QNetworkReply::UnknownProxyError, QGrpcStatus::Unknown },
                                                                { QNetworkReply::ContentAccessDenied, QGrpcStatus::PermissionDenied },
                                                                { QNetworkReply::ContentOperationNotPermittedError, QGrpcStatus::PermissionDenied },
                                                                { QNetworkReply::ContentNotFoundError, QGrpcStatus::NotFound },
                                                                { QNetworkReply::AuthenticationRequiredError, QGrpcStatus::PermissionDenied },
                                                                { QNetworkReply::ContentReSendError, QGrpcStatus::DataLoss },
                                                                { QNetworkReply::ContentConflictError, QGrpcStatus::InvalidArgument },
                                                                { QNetworkReply::ContentGoneError, QGrpcStatus::DataLoss },
                                                                { QNetworkReply::UnknownContentError, QGrpcStatus::Unknown },
                                                                { QNetworkReply::ProtocolUnknownError, QGrpcStatus::Unknown },
                                                                { QNetworkReply::ProtocolInvalidOperationError, QGrpcStatus::Unimplemented },
                                                                { QNetworkReply::ProtocolFailure, QGrpcStatus::Unknown },
                                                                { QNetworkReply::InternalServerError, QGrpcStatus::Internal },
                                                                { QNetworkReply::OperationNotImplementedError, QGrpcStatus::Unimplemented },
                                                                { QNetworkReply::ServiceUnavailableError, QGrpcStatus::Unavailable },
                                                                { QNetworkReply::UnknownServerError, QGrpcStatus::Unknown }};

const char *GrpcAcceptEncodingHeader = "grpc-accept-encoding";
const char *AcceptEncodingHeader = "accept-encoding";
const char *TEHeader = "te";
const char *GrpcStatusHeader = "grpc-status";
const char *GrpcStatusMessage = "grpc-message";
const int GrpcMessageSizeHeaderSize = 5;
}

namespace QtProtobuf {
//! \private
struct QGrpcHttp2ChannelPrivate {
    //! \private
    struct ExpectedData {
        int expectedSize;
        QByteArray container;
    };

    QUrl url;
    QNetworkAccessManager nm;
    std::unique_ptr<QAbstractGrpcCredentials> credentials;
    QSslConfiguration sslConfig;
    std::unordered_map<QNetworkReply *, ExpectedData> activeStreamReplies;
    QObject lambdaContext;

    QNetworkReply *post(const QString &method, const QString &service, const QByteArray &args, bool stream = false) {
        QUrl callUrl = url;
        callUrl.setPath("/" + service + "/" + method);

        qProtoDebug() << "Service call url: " << callUrl;
        QNetworkRequest request(callUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/grpc");
        request.setRawHeader(GrpcAcceptEncodingHeader, "identity,deflate,gzip");
        request.setRawHeader(AcceptEncodingHeader, "identity,gzip");
        request.setRawHeader(TEHeader, "trailers");
        request.setSslConfiguration(sslConfig);
        QGrpcCredentialMap callCredentials = credentials->callCredentials();
        for (auto i = callCredentials.begin(); i != callCredentials.end(); ++i) {
            request.setRawHeader(i.key().data(), i.value().toString().toUtf8());
        }

        request.setAttribute(QNetworkRequest::Http2DirectAttribute, true);

        QByteArray msg(GrpcMessageSizeHeaderSize, '\0');
        *reinterpret_cast<int *>(msg.data() + 1) = qToBigEndian(args.size());
        msg += args;
        qProtoDebug() << "SEND: " << msg.size();

        QNetworkReply *networkReply = nm.post(request, msg);

        QObject::connect(networkReply, &QNetworkReply::sslErrors, [networkReply](const QList<QSslError> &errors) {
           qProtoCritical() << errors;
           // TODO: filter out noncritical SSL handshake errors
           // FIXME: error due to ssl failure is not transferred to the client: last error will be Operation canceled
           QGrpcHttp2ChannelPrivate::abortNetworkReply(networkReply);
        });

        if (!stream) {
            //TODO: Add configurable timeout logic
            QTimer::singleShot(6000, networkReply, [networkReply]() {
                QGrpcHttp2ChannelPrivate::abortNetworkReply(networkReply);
            });
        }
        return networkReply;
    }

    static void abortNetworkReply(QNetworkReply *networkReply) {
        if (networkReply->isRunning()) {
            networkReply->abort();
        } else {
            networkReply->deleteLater();
        }
    }

    static QByteArray processReply(QNetworkReply *networkReply, QGrpcStatus::StatusCode &statusCode) {
        //Check if no network error occured
        if (networkReply->error() != QNetworkReply::NoError) {
            statusCode = StatusCodeMap.at(networkReply->error());
            return {};
        }

        //Check if server answer with error
        statusCode = static_cast<QGrpcStatus::StatusCode>(networkReply->rawHeader(GrpcStatusHeader).toInt());
        if (statusCode != QGrpcStatus::StatusCode::Ok) {
            return {};
        }

        //Message size doesn't matter for now
        return networkReply->readAll().mid(GrpcMessageSizeHeaderSize);
    }

    QGrpcHttp2ChannelPrivate(const QUrl &_url, std::unique_ptr<QAbstractGrpcCredentials> _credentials)
        : url(_url)
        , credentials(std::move(_credentials))
    {
        if (url.scheme() == "https") {
            if (!credentials->channelCredentials().contains(QLatin1String(SslConfigCredential))) {
                throw std::invalid_argument("Https connection requested but not ssl configuration provided.");
            }
            sslConfig = credentials->channelCredentials().value(QLatin1String(SslConfigCredential)).value<QSslConfiguration>();
        } else if (url.scheme().isEmpty()) {
            url.setScheme("http");
        }
    }

    static int getExpectedDataSize(const QByteArray &container) {
        return qFromBigEndian(*reinterpret_cast<const int *>(container.data() + 1)) + GrpcMessageSizeHeaderSize;
    }
};

}

QGrpcHttp2Channel::QGrpcHttp2Channel(const QUrl &url, std::unique_ptr<QAbstractGrpcCredentials> credentials) : QAbstractGrpcChannel()
  , dPtr(std::make_unique<QGrpcHttp2ChannelPrivate>(url, std::move(credentials)))
{
}

QGrpcHttp2Channel::~QGrpcHttp2Channel()
{
}

QGrpcStatus QGrpcHttp2Channel::call(const QString &method, const QString &service, const QByteArray &args, QByteArray &ret)
{
    QEventLoop loop;

    QNetworkReply *networkReply = dPtr->post(method, service, args);
    QObject::connect(networkReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    //If reply was finished in same stack it doesn't make sense to start event loop
    if (!networkReply->isFinished()) {
        loop.exec();
    }

    QGrpcStatus::StatusCode grpcStatus = QGrpcStatus::StatusCode::Unknown;
    ret = dPtr->processReply(networkReply, grpcStatus);

    networkReply->deleteLater();
    qProtoDebug() << __func__ << "RECV: " << ret.toHex() << "grpcStatus" << grpcStatus;
    return {grpcStatus, QString::fromUtf8(networkReply->rawHeader(GrpcStatusMessage))};
}

void QGrpcHttp2Channel::call(const QString &method, const QString &service, const QByteArray &args, QGrpcAsyncReply *reply)
{
    assert(reply != nullptr);
    QNetworkReply *networkReply = dPtr->post(method, service, args);

    std::shared_ptr<QMetaObject::Connection> connection(new QMetaObject::Connection);
    std::shared_ptr<QMetaObject::Connection> abortConnection(new QMetaObject::Connection);
    *connection = QObject::connect(networkReply, &QNetworkReply::finished, reply, [reply, networkReply, connection, abortConnection]() {
        QGrpcStatus::StatusCode grpcStatus = QGrpcStatus::StatusCode::Unknown;
        QByteArray data = QGrpcHttp2ChannelPrivate::processReply(networkReply, grpcStatus);
        if (*connection) {
            QObject::disconnect(*connection);
        }
        if (*abortConnection) {
            QObject::disconnect(*abortConnection);
        }

        qProtoDebug() << "RECV: " << data;
        if (QGrpcStatus::StatusCode::Ok == grpcStatus) {
            reply->setData(data);
            reply->finished();
        } else {
            reply->setData({});
            reply->error({grpcStatus, QString::fromUtf8(networkReply->rawHeader(GrpcStatusMessage))});
        }
        networkReply->deleteLater();
    });

    *abortConnection = QObject::connect(reply, &QGrpcAsyncReply::error, networkReply, [networkReply, connection, abortConnection] (const QGrpcStatus &status) {
        if (status.code() == QGrpcStatus::Aborted) {
            if (*connection) {
                QObject::disconnect(*connection);
            }
            if (*abortConnection) {
                QObject::disconnect(*abortConnection);
            }
            networkReply->deleteLater();
        }
    });
}

void QGrpcHttp2Channel::stream(QGrpcStream *grpcStream, const QString &service, QAbstractGrpcClient *client)
{
    assert(grpcStream != nullptr);
    QNetworkReply *networkReply = dPtr->post(grpcStream->method(), service, grpcStream->arg(), true);

    std::shared_ptr<QMetaObject::Connection> finishConnection(new QMetaObject::Connection);
    std::shared_ptr<QMetaObject::Connection> abortConnection(new QMetaObject::Connection);
    std::shared_ptr<QMetaObject::Connection> readConnection(new QMetaObject::Connection);
    *readConnection = QObject::connect(networkReply, &QNetworkReply::readyRead, grpcStream, [networkReply, grpcStream, this]() {
        auto replyIt = dPtr->activeStreamReplies.find(networkReply);

        QByteArray data = networkReply->readAll();
        qProtoDebug() << "RECV" << data.size();

        if (replyIt == dPtr->activeStreamReplies.end()) {
            qProtoDebug() << data.toHex();
            int expectedDataSize = QGrpcHttp2ChannelPrivate::getExpectedDataSize(data);
            qProtoDebug() << "First chunk received: " << data.size() << " expectedDataSize: " << expectedDataSize;

            if (expectedDataSize == 0) {
                grpcStream->handler(QByteArray());
                return;
            }

            QGrpcHttp2ChannelPrivate::ExpectedData dataContainer{expectedDataSize, QByteArray{}};
            replyIt = dPtr->activeStreamReplies.insert({networkReply, dataContainer}).first;
        }

        QGrpcHttp2ChannelPrivate::ExpectedData &dataContainer = replyIt->second;
        dataContainer.container.append(data);

        qProtoDebug() << "Proceed chunk: " << data.size() << " dataContainer: " << dataContainer.container.size() << " capacity: " << dataContainer.expectedSize;
        while (dataContainer.container.size() >= dataContainer.expectedSize && !networkReply->isFinished()) {
            qProtoDebug() << "Full data received: " << data.size() << " dataContainer: " << dataContainer.container.size() << " capacity: " << dataContainer.expectedSize;
            grpcStream->handler(dataContainer.container.mid(GrpcMessageSizeHeaderSize, dataContainer.expectedSize - GrpcMessageSizeHeaderSize));
            dataContainer.container.remove(0, dataContainer.expectedSize);
            if (dataContainer.container.size() > GrpcMessageSizeHeaderSize) {
                dataContainer.expectedSize = QGrpcHttp2ChannelPrivate::getExpectedDataSize(dataContainer.container);
            } else if (dataContainer.container.size() > 0) {
                qProtoWarning() << "Invalid container size received, size header is less than 5 bytes";
            }
        }

        if (dataContainer.container.size() < GrpcMessageSizeHeaderSize || networkReply->isFinished()) {
            dPtr->activeStreamReplies.erase(replyIt);
        }
    });

    QObject::connect(client, &QAbstractGrpcClient::destroyed, networkReply, [networkReply, finishConnection, abortConnection, readConnection, this]() {
        if (*readConnection) {
            QObject::disconnect(*readConnection);
        }
        if (*abortConnection) {
            QObject::disconnect(*abortConnection);
        }
        if (*finishConnection) {
            QObject::disconnect(*finishConnection);
        }
        dPtr->activeStreamReplies.erase(networkReply);
        QGrpcHttp2ChannelPrivate::abortNetworkReply(networkReply);
        networkReply->deleteLater();
    });

    *finishConnection = QObject::connect(networkReply, &QNetworkReply::finished, grpcStream, [grpcStream, service, networkReply, abortConnection, readConnection, finishConnection, client, this]() {
        QString errorString = networkReply->errorString();
        QNetworkReply::NetworkError networkError = networkReply->error();
        if (*readConnection) {
            QObject::disconnect(*readConnection);
        }
        if (*abortConnection) {
            QObject::disconnect(*abortConnection);
        }

        dPtr->activeStreamReplies.erase(networkReply);
        QGrpcHttp2ChannelPrivate::abortNetworkReply(networkReply);
        networkReply->deleteLater();
        qProtoWarning() << grpcStream->method() << "call" << service << "stream finished: " << errorString;
        switch (networkError) {
        case QNetworkReply::RemoteHostClosedError:
            qProtoDebug() << "Remote server closed connection. Reconnect silently";
            stream(grpcStream, service, client);
            break;
        case QNetworkReply::NoError:
            //Reply closed without error
            break;
        default:
            grpcStream->error(QGrpcStatus{StatusCodeMap.at(networkError), QString("%1 call %2 stream failed: %3").arg(service).arg(grpcStream->method()).arg(errorString)});
            break;
        }
    });

    *abortConnection = QObject::connect(grpcStream, &QGrpcStream::finished, networkReply, [networkReply, finishConnection, abortConnection, readConnection] {
        if (*finishConnection) {
            QObject::disconnect(*finishConnection);
        }
        if (*readConnection) {
            QObject::disconnect(*readConnection);
        }
        if (*abortConnection) {
            QObject::disconnect(*abortConnection);
        }
        QGrpcHttp2ChannelPrivate::abortNetworkReply(networkReply);
        networkReply->deleteLater();
    });
}

std::shared_ptr<QAbstractProtobufSerializer> QGrpcHttp2Channel::serializer() const
{
    //TODO: make selection based on credentials or channel settings
    return QProtobufSerializerRegistry::instance().getSerializer("protobuf");
}
