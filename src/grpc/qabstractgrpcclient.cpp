/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
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

#include "qabstractgrpcclient.h"

#include "qgrpcasyncreply.h"
#include "qgrpcstream.h"
#include "qprotobufserializerregistry_p.h"

#include <QTimer>
#include <QThread>

namespace QtProtobuf {

//! \private
class QAbstractGrpcClientPrivate final {
public:
    QAbstractGrpcClientPrivate(const QString &service) : service(service) {
        serializer = QProtobufSerializerRegistry::instance().getSerializer("protobuf");
    }

    std::shared_ptr<QAbstractGrpcChannel> channel;
    const QString service;
    std::shared_ptr<QAbstractProtobufSerializer> serializer;
    std::vector<QGrpcStreamShared> activeStreams;
};
}

using namespace QtProtobuf;

QAbstractGrpcClient::QAbstractGrpcClient(const QString &service, QObject *parent) : QObject(parent)
  , dPtr(std::make_unique<QAbstractGrpcClientPrivate>(service))
{
}

QAbstractGrpcClient::~QAbstractGrpcClient()
{}

void QAbstractGrpcClient::attachChannel(const std::shared_ptr<QAbstractGrpcChannel> &channel)
{
    if (channel->thread() != QThread::currentThread()) {
        qProtoCritical() << "QAbstractGrpcClient::attachChannel is called from different thread.\n"
                           "QtGrpc doesn't guarantie thread safety on channel level.\n"
                           "You have to be confident that channel routines are working in the same thread as QAbstractGrpcClient";
        throw std::runtime_error("Call from another thread");
    }
    dPtr->channel = channel;
    dPtr->serializer = channel->serializer();
}

QGrpcStatus QAbstractGrpcClient::call(const QString &method, const QByteArray &arg, QByteArray &ret)
{
    QGrpcStatus callStatus{QGrpcStatus::Unknown};
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [&]()->QGrpcStatus {
                                                qProtoDebug() << "Method: " << dPtr->service << method << " called from different thread";
                                                return call(method, arg, ret);
                                            }, Qt::BlockingQueuedConnection, &callStatus);
        return callStatus;
    }

    if (dPtr->channel) {
        callStatus = dPtr->channel->call(method, dPtr->service, arg, ret);
    } else {
        callStatus = QGrpcStatus{QGrpcStatus::Unknown, QLatin1String("No channel(s) attached.")};
    }

    if (callStatus != QGrpcStatus::Ok) {
        error(callStatus);
    }

    return callStatus;
}

QGrpcAsyncReplyShared QAbstractGrpcClient::call(const QString &method, const QByteArray &arg)
{
    QGrpcAsyncReplyShared reply;
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [&]()->QGrpcAsyncReplyShared {
                                      qProtoDebug() << "Method: " << dPtr->service << method << " called from different thread";
                                      return call(method, arg);
                                  }, Qt::BlockingQueuedConnection, &reply);
    } else if (dPtr->channel) {
        reply.reset(new QGrpcAsyncReply(dPtr->channel, this), [](QGrpcAsyncReply *reply) { reply->deleteLater(); });

        auto errorConnection = std::make_shared<QMetaObject::Connection>();
        auto finishedConnection = std::make_shared<QMetaObject::Connection>();
        *errorConnection = connect(reply.get(), &QGrpcAsyncReply::error, this, [this, reply, errorConnection, finishedConnection](const QGrpcStatus &status) mutable {
            error(status);
            QObject::disconnect(*finishedConnection);
            QObject::disconnect(*errorConnection);
            reply.reset();
        });

        *finishedConnection = connect(reply.get(), &QGrpcAsyncReply::finished, [reply, errorConnection, finishedConnection]() mutable {
            QObject::disconnect(*finishedConnection);
            QObject::disconnect(*errorConnection);
            reply.reset();
        });

        dPtr->channel->call(method, dPtr->service, arg, reply.get());
    } else {
        error({QGrpcStatus::Unknown, QLatin1String("No channel(s) attached.")});
    }

    return reply;
}

QGrpcStreamShared QAbstractGrpcClient::stream(const QString &method, const QByteArray &arg, const QtProtobuf::StreamHandler &handler)
{
    QGrpcStreamShared grpcStream;

    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [&]()->QGrpcStreamShared {
                                      qProtoDebug() << "Stream: " << dPtr->service << method << " called from different thread";
                                      return stream(method, arg, handler);
                                  }, Qt::BlockingQueuedConnection, &grpcStream);
    } else if (dPtr->channel) {
        grpcStream.reset(new QGrpcStream(dPtr->channel, method, arg, handler, this), [](QGrpcStream *stream) { stream->deleteLater(); });

        auto it = std::find_if(std::begin(dPtr->activeStreams), std::end(dPtr->activeStreams), [grpcStream](const QGrpcStreamShared &activeStream) {
           return *activeStream == *grpcStream;
        });

        if (it != std::end(dPtr->activeStreams)) {
            (*it)->addHandler(handler);
            return *it; //If stream already exists return it for handling
        }

        auto errorConnection = std::make_shared<QMetaObject::Connection>();
        *errorConnection = connect(grpcStream.get(), &QGrpcStream::error, this, [this, grpcStream](const QGrpcStatus &status) {
            qProtoWarning() << grpcStream->method() << "call" << dPtr->service << "stream error: " << status.message();
            error(status);
            std::weak_ptr<QGrpcStream> weakStream = grpcStream;
            //TODO: Make timeout configurable from channel settings
            QTimer::singleShot(1000, this, [this, weakStream, method = grpcStream->method()] {
                auto stream = weakStream.lock();
                if (stream) {
                    dPtr->channel->stream(stream.get(), dPtr->service, this);
                } else {
                    qProtoDebug() << "Stream for " << dPtr->service << "method" << method << " will not be restored by timeout.";
                }
            });
        });

        auto finishedConnection = std::make_shared<QMetaObject::Connection>();
        *finishedConnection = connect(grpcStream.get(), &QGrpcStream::finished, this, [this, grpcStream, errorConnection, finishedConnection]() mutable {
            qProtoWarning() << grpcStream->method() << "call" << dPtr->service << "stream finished";
            auto it = std::find_if(std::begin(dPtr->activeStreams), std::end(dPtr->activeStreams), [grpcStream](QGrpcStreamShared activeStream) {
               return *activeStream == *grpcStream;
            });

            if (it != std::end(dPtr->activeStreams)) {
                dPtr->activeStreams.erase(it);
            }
            QObject::disconnect(*errorConnection);
            QObject::disconnect(*finishedConnection);
            grpcStream.reset();
        });

        dPtr->channel->stream(grpcStream.get(), dPtr->service, this);
        dPtr->activeStreams.push_back(grpcStream);
    } else {
        error({QGrpcStatus::Unknown, QLatin1String("No channel(s) attached.")});
    }
    return grpcStream;
}

QAbstractProtobufSerializer *QAbstractGrpcClient::serializer() const
{
    return dPtr->serializer.get();
}
