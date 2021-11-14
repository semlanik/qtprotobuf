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

#pragma once //QAbstractGrpcChannel

#include <QString>
#include <QByteArray>
#include <functional>
#include <memory>

#include "qgrpcstatus.h"
#include "qgrpccredentials.h"
#include "qtgrpcglobal.h"

class QThread;

namespace QtProtobuf {

class QAbstractGrpcClient;
class QAbstractProtobufSerializer;
struct QAbstractGrpcChannelPrivate;
/*!
 * \ingroup QtGrpc
 * \brief The QAbstractGrpcChannel class is interface that represents common gRPC channel functionality.
 *        You may implement this interface to create own channels for gRPC transport.
 */
class Q_GRPC_EXPORT QAbstractGrpcChannel
{
public:
    /*!
     * \brief Calls \p method synchronously with given serialized message \p args and write result of call to \p ret.
     *        \note This method is synchronous, that means it doesn't return until call is completed or aborted by timeout if it's
     *        implemented in inherited channel.
     *        \note This method should not be called directly.
     * \param[in] method remote method is called
     * \param[in] service service identified in URL path format
     * \param[in] args serialized argument message
     * \param[out] ret output bytearray to collect returned message
     * \return returns gRPC QAbstractGrpcChannel::Status code for operation
     */
    virtual QGrpcStatus call(const QString &method, const QString &service, const QByteArray &args, QByteArray &ret) = 0;

    /*!
     * \brief Calls \p method asynchronously with given serialized messge \p args. Result of method call is written to QGrpcCallReply.
     *        \note This method is asynchronous, that means it returns control imediately after it is called.
     *        \note This method should not be called directly.
     * \param[in] method remote method is called
     * \param[in] service service identified in URL path format
     * \param[in] args serialized argument message
     * \param[out] ret QGrpcCallReply that will be returned to end-point user to read data once call complete.
     *            QGrpcCallReply lifecycle is managed by QAbstractGrpcClient only.
     *            \see QGrpcCallReply for details
     */
    virtual void call(const QString &method, const QString &service, const QByteArray &args, QtProtobuf::QGrpcCallReply *ret) = 0;

    /*!
     * \brief Streams to server-side stream to receive updates for given \p method.
     *        \note This method should not be called directly.
     * \param[in] method remote method is called
     * \param[in] service service identified in URL path format
     * \param[in] args serialized argument message
     * \param[in] handler callback that will be called when message recevied from the server-stream
     */
    virtual void stream(QGrpcStream *stream, const QString &service, QAbstractGrpcClient *client) = 0;

    virtual std::shared_ptr<QAbstractProtobufSerializer> serializer() const = 0;

    const QThread *thread() const;

protected:
    //! \private
    QAbstractGrpcChannel();
    //! \private
    virtual ~QAbstractGrpcChannel();
private:
    Q_DISABLE_COPY(QAbstractGrpcChannel)
    std::unique_ptr<QAbstractGrpcChannelPrivate> dPtr;
};
}
