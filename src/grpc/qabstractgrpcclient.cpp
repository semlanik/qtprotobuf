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
    std::vector<QGrpcSubscriptionShared> activeSubscriptions;
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
        assert(channel->thread() == QThread::currentThread());
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

QGrpcSubscriptionShared QAbstractGrpcClient::subscribe(const QString &method, const QByteArray &arg, const QtProtobuf::SubscriptionHandler &handler)
{
    QGrpcSubscriptionShared subscription;

    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [&]()->QGrpcSubscriptionShared {
                                      qProtoDebug() << "Subscription: " << dPtr->service << method << " called from different thread";
                                      return subscribe(method, arg, handler);
                                  }, Qt::BlockingQueuedConnection, &subscription);
    } else if (dPtr->channel) {
        subscription.reset(new QGrpcSubscription(dPtr->channel, method, arg, handler, this), [](QGrpcSubscription *subscription) { subscription->deleteLater(); });

        auto it = std::find_if(std::begin(dPtr->activeSubscriptions), std::end(dPtr->activeSubscriptions), [subscription](const QGrpcSubscriptionShared &activeSubscription) {
           return *activeSubscription == *subscription;
        });

        if (it != std::end(dPtr->activeSubscriptions)) {
            (*it)->addHandler(handler);
            return *it; //If subscription already exists return it for handling
        }

        auto errorConnection = std::make_shared<QMetaObject::Connection>();
        *errorConnection = connect(subscription.get(), &QGrpcSubscription::error, this, [this, subscription](const QGrpcStatus &status) {
            qProtoWarning() << subscription->method() << "call" << dPtr->service << "subscription error: " << status.message();
            error(status);
            std::weak_ptr<QGrpcSubscription> weakSubscription = subscription;
            //TODO: Make timeout configurable from channel settings
            QTimer::singleShot(1000, this, [this, weakSubscription, method = subscription->method()] {
                auto subscription = weakSubscription.lock();
                if (subscription) {
                    dPtr->channel->subscribe(subscription.get(), dPtr->service, this);
                } else {
                    qProtoDebug() << "Subscription for " << dPtr->service << "method" << method << " will not be restored by timeout.";
                }
            });
        });

        auto finishedConnection = std::make_shared<QMetaObject::Connection>();
        *finishedConnection = connect(subscription.get(), &QGrpcSubscription::finished, this, [this, subscription, errorConnection, finishedConnection]() mutable {
            qProtoWarning() << subscription->method() << "call" << dPtr->service << "subscription finished";
            auto it = std::find_if(std::begin(dPtr->activeSubscriptions), std::end(dPtr->activeSubscriptions), [subscription](QGrpcSubscriptionShared activeSubscription) {
               return *activeSubscription == *subscription;
            });

            if (it != std::end(dPtr->activeSubscriptions)) {
                dPtr->activeSubscriptions.erase(it);
            }
            QObject::disconnect(*errorConnection);
            QObject::disconnect(*finishedConnection);
            subscription.reset();
        });

        dPtr->channel->subscribe(subscription.get(), dPtr->service, this);
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
