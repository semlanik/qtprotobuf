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

#include <functional>
#include <QPointer>
#include <QMutex>
#include <memory>

#include "qabstractgrpcchannel.h"
#include "qabstractgrpcclient.h"

#include "qtgrpcglobal.h"

namespace QtProtobuf {

/*!
 * \ingroup QtGrpc
 * \brief The QGrpcAsyncReply class contains data for asynchronous call of gRPC client API. It's owned by client class, that
 *        created it. QGrpcAsyncReply coul be used by QAbstractGrpcChannel implementations to control call work flow and
 *        abort calls if possible in case if QGrpcAsyncReply::abort method called by library user.
 */
class Q_GRPC_EXPORT QGrpcAsyncReply final : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Reads message from raw byte array stored in QGrpcAsyncReply
     * \return Copy of deserialized message or non-initialized message in case of exceptional situation
     */
    template <typename T>
    T read() {
        QMutexLocker locker(&m_asyncLock);
        T value;
        try {
            value.deserialize(static_cast<QAbstractGrpcClient*>(parent())->serializer(), m_data);
        } catch (std::invalid_argument &) {
            static const QLatin1String invalidArgumentErrorMessage("Response deserialization failed invalid field found");
            error({QGrpcStatus::InvalidArgument, invalidArgumentErrorMessage});
        } catch (std::out_of_range &) {
            static const QLatin1String outOfRangeErrorMessage("Invalid size of received buffer");
            error({QGrpcStatus::OutOfRange, outOfRangeErrorMessage});
        } catch (...) {
            error({QGrpcStatus::Internal, QLatin1String("Unknown exception caught during deserialization")});
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

    /*!
     * \brief Aborts this reply and try to abort call in channel
     */
    void abort() {
        m_channel->abort(this);
    }

signals:
    /*!
     * \brief The signal is emitted when reply is ready for read. Usualy called by channel when all chunks of data
     *        recevied
     */
    void finished();

    /*!
     * \brief The signal is emitted when error happend in channel or during serialization
     * \param code gRPC channel QGrpcStatus::StatusCode
     * \param errorMessage Description of error occured
     */
    void error(const QGrpcStatus &status);

protected:
    //! \private
    QGrpcAsyncReply(const std::shared_ptr<QAbstractGrpcChannel> &channel, QAbstractGrpcClient *parent = nullptr) : QObject(parent)
    , m_channel(channel) {}
    //! \private
    ~QGrpcAsyncReply();

private:
    QGrpcAsyncReply();
    Q_DISABLE_COPY_MOVE(QGrpcAsyncReply)

    friend class QAbstractGrpcClient;

    std::shared_ptr<QAbstractGrpcChannel> m_channel;
    QByteArray m_data;

    QMutex m_asyncLock;
};
}
