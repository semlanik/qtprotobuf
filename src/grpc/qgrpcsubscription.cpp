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

#include "qgrpcsubscription.h"

#include <qtprotobuflogging.h>
#include <QThread>

using namespace QtProtobuf;

QGrpcSubscription::QGrpcSubscription(const std::shared_ptr<QAbstractGrpcChannel> &channel, const QString &method,
                                     const QByteArray &arg, const SubscriptionHandler &handler, QAbstractGrpcClient *parent) : QGrpcAsyncOperationBase(channel, parent)
  , m_method(method)
  , m_arg(arg)
{
    if (handler) {
        m_handlers.push_back(handler);
    }
}

void QGrpcSubscription::addHandler(const SubscriptionHandler &handler)
{
    if (handler) {
        m_handlers.push_back(handler);
    }
}

void QGrpcSubscription::cancel()
{
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [this](){m_channel->cancel(this);}, Qt::BlockingQueuedConnection);
    } else {
        m_channel->cancel(this);
    }
}
