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
#include "qgrpcsubscription.h"
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
    std::vector<QGrpcSubscription *> activeSubscriptions;
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
    dPtr->channel = channel;
    dPtr->serializer = channel->serializer();
}

QGrpcStatus QAbstractGrpcClient::call(const QString &method, const QByteArray &arg, QByteArray &ret)
{
    QGrpcStatus callStatus{QGrpcStatus::Unknown};
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

QGrpcAsyncReply *QAbstractGrpcClient::call(const QString &method, const QByteArray &arg)
{
    QGrpcAsyncReply *reply = nullptr;
    if (dPtr->channel) {
        reply = new QGrpcAsyncReply(dPtr->channel, this);

        connect(reply, &QGrpcAsyncReply::error, this, [this, reply](const QGrpcStatus &status) {
            error(status);
            reply->deleteLater();
        });

        connect(reply, &QGrpcAsyncReply::finished, this, [reply] {
            reply->deleteLater();
        });

        dPtr->channel->call(method, dPtr->service, arg, reply);
    } else {
        error({QGrpcStatus::Unknown, QLatin1String("No channel(s) attached.")});
    }

    return reply;
}

QGrpcSubscription *QAbstractGrpcClient::subscribe(const QString &method, const QByteArray &arg, const QtProtobuf::SubscriptionHandler &handler)
{
    QGrpcSubscription *subscription = nullptr;
    if (dPtr->channel) {
        subscription = new QGrpcSubscription(dPtr->channel, method, arg, handler, this);

        auto it = std::find_if(std::begin(dPtr->activeSubscriptions), std::end(dPtr->activeSubscriptions), [subscription](QGrpcSubscription *activeSubscription) {
           return *activeSubscription == *subscription;
        });

        if (it != std::end(dPtr->activeSubscriptions)) {
            (*it)->addHandler(handler);
            return *it; //If subscription already exists return it for handling
        }

        connect(subscription, &QGrpcSubscription::error, this, [this, subscription](const QGrpcStatus &status) {
            qProtoWarning() << subscription->method() << "call" << dPtr->service << "subscription error: " << status.message();
            error(status);
            dPtr->channel->subscribe(subscription, dPtr->service, this);
        });

        connect(subscription, &QGrpcSubscription::finished, this, [this, subscription] {
            qProtoWarning() << subscription->method() << "call" << dPtr->service << "subscription finished";
            auto it = std::find_if(std::begin(dPtr->activeSubscriptions), std::end(dPtr->activeSubscriptions), [subscription](QGrpcSubscription *activeSubscription) {
               return *activeSubscription == *subscription;
            });

            if (it != std::end(dPtr->activeSubscriptions)) {
                dPtr->activeSubscriptions.erase(it);
            }
            subscription->deleteLater();
        });

        dPtr->channel->subscribe(subscription, dPtr->service, this);
        dPtr->activeSubscriptions.push_back(subscription);
    } else {
        error({QGrpcStatus::Unknown, QLatin1String("No channel(s) attached.")});
    }
    return subscription;
}

QAbstractProtobufSerializer *QAbstractGrpcClient::serializer() const
{
    return dPtr->serializer.get();
}
