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

#include <memory>
#include <QObject>
#include <QByteArray>
#include <QDebug>

#include "abstractchannel.h"

namespace qtprotobuf {

class AbstractChannel;

class AbstractClient : public QObject //TODO: QObject is not really required yet
{
public:
    void attachChannel(std::shared_ptr<AbstractChannel> channel);

protected:
    AbstractClient(const QString &service, QObject *parent = nullptr);

    template<typename A, typename R>
    AbstractChannel::StatusCodes call(const QString &method, const A &arg, R &ret) {
        if (!m_channel) {
            return AbstractChannel::Unknown;
        }

        QByteArray retData;
        AbstractChannel::StatusCodes status = m_channel->call(method, m_service, arg.serialize(), retData);
        if (status != AbstractChannel::Ok) {
            return status;
        }
        ret.deserialize(retData.mid(5));
        return status;
    }

private:
    Q_DISABLE_COPY(AbstractClient)

    std::shared_ptr<AbstractChannel> m_channel;
    QString m_service;
};

}
