/*
 * MIT License
 *
 * Copyright (c) 2019 Giulio Girardi <giulio.girardi@protechgroup.it>
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

#pragma once //QGrpcChannel

#include <QUrl>

#include <grpcpp/security/credentials.h>

#include <memory>

#include "qabstractgrpcchannel.h"

namespace QtProtobuf {

class QAbstractGrpcCredentials;
struct QGrpcChannelPrivate;

/*!
 * \ingroup QtGrpc
 * \brief The QGrpcChannel class is a gRPC-cpp native api implementation of QAbstractGrpcChannel interface
 * \details QGrpcChannel accepts the same grpc::ChannelCredentials type that is
 *          required by native-api grpc::CreateChannel.
 *          See https://grpc.github.io/grpc/cpp/classgrpc__impl_1_1_channel_credentials.html.
 */
class Q_GRPC_EXPORT QGrpcChannel final : public QAbstractGrpcChannel
{
public:
    /*!
     * \brief QGrpcChannel constructs QGrpcChannel
     * \param name uri used to establish channel connection
     * \param credentials grpc credientials object
     */
    QGrpcChannel(const QUrl &name, std::shared_ptr<grpc::ChannelCredentials> credentials);
    ~QGrpcChannel();

    QGrpcStatus call(const QString &method, const QString &service, const QByteArray &args, QByteArray &ret) override;
    void call(const QString &method, const QString &service, const QByteArray &args, QtProtobuf::QGrpcAsyncReply *reply) override;
    void subscribe(QGrpcStream *stream, const QString &service, QAbstractGrpcClient *client) override;
    std::shared_ptr<QAbstractProtobufSerializer> serializer() const override;

private:
    Q_DISABLE_COPY_MOVE(QGrpcChannel)

    std::unique_ptr<QGrpcChannelPrivate> dPtr;
};

}
