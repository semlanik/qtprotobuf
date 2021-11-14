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

#pragma once

#include <QObject>
#include <QMutex>

#include <functional>
#include <memory>

#include "qabstractgrpcclient.h"

#include "qtgrpcglobal.h"

namespace QtProtobuf {

/*!
 * \ingroup QtGrpc
 * \private
 * \brief The QGrpcAsyncOperationBase class implements common logic to
 *        handle communication in Grpc channel.
 */
class Q_GRPC_EXPORT QGrpcAsyncOperationBase : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Reads message from raw byte array stored in QGrpcCallReply
     * \return Copy of deserialized message or non-initialized message in case of exceptional situation
     */
    template <typename T>
    T read() {
        QMutexLocker locker(&m_asyncLock);
        T value;
        auto client = static_cast<QAbstractGrpcClient*>(parent());
        if (client) {
            client->tryDeserialize(value, m_data);
        }
        return value;
    }

    /*!
     * \brief Interface for implementation of QAbstractGrpcChannel. Should be used to write raw data from channel to
     *        reply
     * \param data Raw data received from channel
     */
    void setData(const QByteArray &data)
    {
        QMutexLocker locker(&m_asyncLock);
        m_data = data;
    }

    virtual void abort() = 0;

signals:
    /*!
     * \brief The signal indicates the end of communication for this call. If signal emitted by
     *        stream this means that stream is succesfully closed either by client or server.
     */
    void finished();

    /*!
     * \brief The signal is emitted when error happend in channel or during serialization
     * \param[out] status received from gRPC channel
     */
    void error(const QtProtobuf::QGrpcStatus &status);

protected:
    //! \private
    QGrpcAsyncOperationBase(QAbstractGrpcClient *parent) : QObject(parent) {}
    //! \private
    virtual ~QGrpcAsyncOperationBase();

private:
    QGrpcAsyncOperationBase();
    Q_DISABLE_COPY_MOVE(QGrpcAsyncOperationBase)

    friend class QAbstractGrpcClient;

    QByteArray m_data;
    QMutex m_asyncLock;
};

}
