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

#include "qgrpcstream.h"

#include "qtgrpclogging_p.h"

#include <QThread>

QT_BEGIN_NAMESPACE

QGrpcStream::QGrpcStream(const QString &method, const QByteArray &arg, const QGrpcStreamHandler &handler, QAbstractGrpcClient *parent) : QGrpcAsyncOperationBase(parent)
  , m_method(method)
  , m_arg(arg)
{
    if (handler) {
        m_handlers.push_back(handler);
    }
}

void QGrpcStream::addHandler(const QGrpcStreamHandler &handler)
{
    if (handler) {
        m_handlers.push_back(handler);
    }
}

void QGrpcStream::abort()
{
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, &QGrpcStream::finished, Qt::BlockingQueuedConnection);
    } else {
        finished();
    }
}

QT_END_NAMESPACE
