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
#include "qprotobufserializerregistry_p.h"

#include <QTimer>

namespace QtProtobuf {
class QAbstractGrpcClientPrivate final {
public:
    QAbstractGrpcClientPrivate(const QString &service) : service(service) {
        serializer = QProtobufSerializerRegistry::instance().getSerializer("protobuf");
    }

    std::shared_ptr<QAbstractGrpcChannel> channel;
    const QString service;
    std::shared_ptr<QAbstractProtobufSerializer> serializer;
};
}

using namespace QtProtobuf;

QAbstractGrpcClient::QAbstractGrpcClient(const QString &service, QObject *parent) : QObject(parent)
  , d_ptr(std::make_unique<QAbstractGrpcClientPrivate>(service))
{
}

QAbstractGrpcClient::~QAbstractGrpcClient()
{}

void QAbstractGrpcClient::attachChannel(const std::shared_ptr<QAbstractGrpcChannel> &channel)
{
    d_ptr->channel = channel;
    d_ptr->serializer = channel->serializer();
}

QGrpcStatus QAbstractGrpcClient::call(const QString &method, const QByteArray &arg, QByteArray &ret)
{
    QGrpcStatus callStatus{QGrpcStatus::Unknown};
    if (d_ptr->channel) {
        callStatus = d_ptr->channel->call(method, d_ptr->service, arg, ret);
    } else {
        callStatus = QGrpcStatus{QGrpcStatus::Unknown, QLatin1String("No channel(s) attached.")};
    }

    if (callStatus != QGrpcStatus::Ok) {
        error(callStatus);
    }

    return callStatus;
}

QGrpcAsyncReply *QAbstractGrpcClient::call(const QString &method, const QByteArray &arg)
{
    QGrpcAsyncReply *reply = nullptr;
    if (d_ptr->channel) {
        reply = new QGrpcAsyncReply(d_ptr->channel, this);

        connect(reply, &QGrpcAsyncReply::error, this, [this, reply](const QGrpcStatus &status) {
            error(status);
            reply->deleteLater();
        });

        connect(reply, &QGrpcAsyncReply::finished, this, [reply]() {
            reply->deleteLater();
        });

        d_ptr->channel->call(method, d_ptr->service, arg, reply);
    } else {
        error({QGrpcStatus::Unknown, QLatin1String("No channel(s) attached.")});
    }

    return reply;
}

void QAbstractGrpcClient::subscribe(const QString &method, const QByteArray &arg, const std::function<void(const QByteArray&)> &handler)
{
    if (d_ptr->channel) {
        d_ptr->channel->subscribe(method, d_ptr->service, arg, this, handler);
    } else {
        error({QGrpcStatus::Unknown, QLatin1String("No channel(s) attached.")});
    }
}

QAbstractProtobufSerializer *QAbstractGrpcClient::serializer() const
{
    return d_ptr->serializer.get();
}
