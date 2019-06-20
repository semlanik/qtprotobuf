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

#include <QTimer>

namespace QtProtobuf {
class QAbstractGrpcClientPrivate final {
public:
    QAbstractGrpcClientPrivate(const QString &service) : service(service) {}

    std::shared_ptr<QAbstractGrpcChannel> channel;
    const QString service;
};
}

using namespace QtProtobuf;

QAbstractGrpcClient::QAbstractGrpcClient(const QString &service, QObject *parent) : QObject(parent)
  , d(new QAbstractGrpcClientPrivate(service))
{
}

QAbstractGrpcClient::~QAbstractGrpcClient()
{
    delete d;
}

void QAbstractGrpcClient::attachChannel(const std::shared_ptr<QAbstractGrpcChannel> &channel)
{
    d->channel = channel;
}

QAbstractGrpcChannel::StatusCode QAbstractGrpcClient::call(const QString &method, const QByteArray &arg, QByteArray &ret)
{
    QAbstractGrpcChannel::StatusCode callStatus = QAbstractGrpcChannel::Unknown;
    if (d->channel) {
        callStatus = d->channel->call(method, d->service, arg, ret);
    } else {
        error(callStatus, QLatin1String("No channel(s) attached."));
    }

    return callStatus;
}

QGrpcAsyncReply *QAbstractGrpcClient::call(const QString &method, const QByteArray &arg)
{
    QGrpcAsyncReply *reply = nullptr;
    if (d->channel) {
        reply = new QGrpcAsyncReply(d->channel, this);

        connect(reply, &QGrpcAsyncReply::error, this, [this, reply](QAbstractGrpcChannel::StatusCode statusCode, const QString &errorMessage) {
            error(statusCode, errorMessage);
            reply->deleteLater();
        });

        connect(reply, &QGrpcAsyncReply::finished, this, [reply]() {
            reply->deleteLater();
        });

        d->channel->call(method, d->service, arg, reply);
    } else {
        error(QAbstractGrpcChannel::Unknown, QLatin1String("No channel(s) attached."));
    }

    return reply;
}

void QAbstractGrpcClient::subscribe(const QString &method, const QByteArray &arg, const std::function<void(const QByteArray&)> &handler)
{
    if (d->channel) {
        d->channel->subscribe(method, d->service, arg, this, handler);
    } else {
        error(QAbstractGrpcChannel::Unknown, QLatin1String("No channel(s) attached."));
    }
}
