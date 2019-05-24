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

#pragma once

#include <functional>
#include <QPointer>
#include <QMutex>
#include <memory>

#include "abstractchannel.h"

#include "qtgrpc_global.h"

namespace qtprotobuf {

class QTGRPCSHARED_EXPORT AsyncReply final : public QObject
{
    Q_OBJECT
public:
    void abort() {
        m_channel->abort(this);
    }

    template <typename T>
    T read() {
        QMutexLocker locker(&m_asyncLock);
        T value;
        value.deserialize(m_data);
        return value;
    }

    void setData(const QByteArray &data) { m_data = data; }

signals:
    void finished();
    void error(AbstractChannel::StatusCodes);

protected:
    AsyncReply(const std::shared_ptr<AbstractChannel> &channel, QObject* parent = nullptr) : QObject(parent)
    , m_channel(channel){}
    ~AsyncReply();

private:
    AsyncReply();
    Q_DISABLE_COPY(AsyncReply)
    AsyncReply(AsyncReply &&) = delete;
    AsyncReply &operator =(AsyncReply &&) = delete;

    friend class AbstractClient;

    std::shared_ptr<AbstractChannel> m_channel;
    QByteArray m_data;

    QMutex m_asyncLock;
};

}
