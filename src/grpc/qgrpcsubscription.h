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

#pragma once //QGrpcSubscription

#include <functional>
#include <QMutex>
#include <memory>

#include "qabstractgrpcchannel.h"
#include "qabstractgrpcclient.h"

#include "qtgrpcglobal.h"

namespace QtProtobuf {

class Q_GRPC_EXPORT QGrpcSubscription final : public QObject
{
    Q_OBJECT
public:
    virtual ~QGrpcSubscription() = default;

    /*!
     * \brief Cancels this subscription and try to abort call in channel
     */
    void cancel() {
        m_channel->cancel(this);
    }

    /*!
     * \private
     * \brief Returns method for this subscription
     */
    QString method() const {
        return m_method;
    }

    /*!
     * \private
     * \brief Returns serialized arguments for this subscription
     */
    QByteArray arg() const {
        return m_arg;
    }

    /*!
     * \private
     * \brief Invokes handler method assigned to this subscription
     */
    void handler(const QByteArray& data) const {
        m_handler(data);
    }
signals:
    /*!
     * \brief The signal is emitted when subscription is finished by user
     */
    void finished();

    /*!
     * \brief The signal is emitted when error happend in channel or during serialization
     *        \note QtGrpc automaically re-tries to restore subscription in case of any channel
     *        or serialization error.
     * \param code gRPC channel QGrpcStatus::StatusCode
     * \param errorMessage Description of error occured
     */
    void error(const QGrpcStatus &status);

protected:
    QGrpcSubscription(const std::shared_ptr<QAbstractGrpcChannel> &channel, const QString &method,
                      const QByteArray &arg, const std::function<void(const QByteArray&)> &handler);

private:
    friend class QAbstractGrpcClient;
    std::shared_ptr<QAbstractGrpcChannel> m_channel;
    QString m_method;
    QByteArray m_arg;
    std::function<void(const QByteArray&)> m_handler;
};

}
