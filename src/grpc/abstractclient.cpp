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

#include "abstractclient.h"

#include <QTimer>

namespace qtprotobuf {
class AbstractClientPrivate final {
public:
    AbstractClientPrivate(const QString &service) : service(service)
    , lastError(AbstractChannel::StatusCodes::Ok) {}

    std::shared_ptr<AbstractChannel> channel;
    const QString service;
    AbstractChannel::StatusCodes lastError;
    QString lastErrorString;
};
}

using namespace qtprotobuf;

AbstractClient::AbstractClient(const QString &service, QObject *parent) : QObject(parent)
  , d(new AbstractClientPrivate(service))
{

}

AbstractClient::~AbstractClient()
{
    delete d;
}

void AbstractClient::attachChannel(const std::shared_ptr<AbstractChannel> &channel)
{
    d->channel = channel;
}

bool AbstractClient::call(const QString &method, const QByteArray &arg, QByteArray &ret)
{
    if (!d->channel) {
        d->lastError = AbstractChannel::Unknown;
        return false;
    }

    d->lastError = d->channel->call(method, d->service, arg, ret);
    return d->lastError == AbstractChannel::Ok;
}

AsyncReply *AbstractClient::call(const QString &method, const QByteArray &arg)
{
    AsyncReply *reply = new AsyncReply(d->channel, this);

    if (!d->channel) {
        d->lastError = AbstractChannel::Unknown;
        d->lastErrorString = "No channel attached";
        QTimer::singleShot(0, this, [reply]() {
            reply->error(AbstractChannel::Unknown);
            reply->deleteLater();
        });
        return reply;
    }

    connect(reply, &AsyncReply::error, this, [this, reply](AbstractChannel::StatusCodes statusCode){
        d->lastError = statusCode;
        reply->deleteLater();
    });

    connect(reply, &AsyncReply::finished, this, [this, reply](){
        reply->deleteLater();
    });

    d->lastError = AbstractChannel::Ok;//Assume that all is OK until something happened
    d->channel->call(method, d->service, arg, reply);
    return reply;
}

void AbstractClient::subscribe_p(const QString &method, const QByteArray &arg, const std::function<void(const QByteArray&)> &handler)
{
    d->lastError = AbstractChannel::Ok;
    if (!d->channel) {
        d->lastError = AbstractChannel::Unknown;
        d->lastErrorString = "No channel attached";
        return;
    }

    d->channel->subscribe(method, d->service, arg, this, handler);
}

AbstractChannel::StatusCodes AbstractClient::lastError() const
{
    return d->lastError;
}

QString AbstractClient::lastErrorString() const
{
    return d->lastErrorString;
}
