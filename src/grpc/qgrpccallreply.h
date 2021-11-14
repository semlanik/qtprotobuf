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

#pragma once //QGrpcCallReply

#include <functional>
#include <QMutex>
#include <memory>

#include "qabstractgrpcclient.h"
#include "qgrpcasyncoperationbase_p.h"

#include "qtgrpcglobal.h"

namespace QtProtobuf {

/*!
 * \ingroup QtGrpc
 * \brief The QGrpcCallReply class contains data for asynchronous call of gRPC client API. It's owned by client class, that
 *        created it. QGrpcCallReply could be used by QAbstractGrpcChannel implementations to control call work flow and
 *        abort calls if possible in case if QGrpcCallReply::abort method called by library user.
 */
class Q_GRPC_EXPORT QGrpcCallReply final : public QGrpcAsyncOperationBase
{
    Q_OBJECT
public:
    /*!
     * \brief Aborts this reply and try to abort call in channel
     */
    void abort() override;

    /*!
     * \brief Subscribe to QGrpcCallReply signals
     */
    template <typename Func1, typename Func2>
    inline void subscribe(QObject *receiver, Func1 finishCallback, Func2 errorCallback,
                                     Qt::ConnectionType type = Qt::AutoConnection)
    {
        QObject::connect(this, &QGrpcCallReply::finished, receiver, finishCallback, type);
        QObject::connect(this, &QGrpcCallReply::error, receiver, errorCallback, type);
    }

    /*!
     * \brief Overloaded QGrpcCallReply::subscribe method, to subscribe to finished signal
     *        only
     */
    template <typename Func1>
    inline void subscribe(QObject *receiver, Func1 finishCallback,
                                     Qt::ConnectionType type = Qt::AutoConnection)
    {
        QObject::connect(this, &QGrpcCallReply::finished, receiver, finishCallback, type);
    }

protected:
    //! \private
    QGrpcCallReply(QAbstractGrpcClient *parent) : QGrpcAsyncOperationBase(parent)
    {}
    //! \private
    ~QGrpcCallReply() = default;

private:
    //! \private
    QGrpcCallReply();
    Q_DISABLE_COPY_MOVE(QGrpcCallReply)

    friend class QAbstractGrpcClient;
};
}
