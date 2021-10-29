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

#include <QEventLoop>
#include <QThread>

#include <grpcpp/channel.h>
#include <grpcpp/impl/codegen/byte_buffer.h>
#include <grpcpp/impl/codegen/client_context.h>
#include <grpcpp/impl/codegen/sync_stream.h>
#include <grpcpp/security/credentials.h>

#include "qabstractgrpccredentials.h"
#include "qgrpccallreply.h"
#include "qgrpcstream.h"
#include "qabstractgrpcclient.h"
#include "qgrpccredentials.h"
#include "qprotobufserializerregistry_p.h"
#include "qtprotobuflogging.h"

namespace QtProtobuf {

//! \private
class QGrpcChannelStream : public QObject {
    //! \private
    Q_OBJECT;

public:
    QGrpcChannelStream(grpc::Channel *channel, const QString &method, const QByteArray &data, QObject *parent = nullptr);
    ~QGrpcChannelStream();

    void cancel();
    void start();

signals:
    void dataReady(const QByteArray &data);
    void finished();

public:
    QGrpcStatus status;

private:
    QThread *thread;
    grpc::ClientContext context;
    grpc::ClientReader<grpc::ByteBuffer> *reader = nullptr;
};

//! \private
class QGrpcChannelCall : public QObject {
    //! \private
    Q_OBJECT;

public:
    QGrpcChannelCall(grpc::Channel *channel, const QString &method, const QByteArray &data, QObject *parent = nullptr);
    ~QGrpcChannelCall();

    void cancel();
    void start();

signals:
    void finished();

public:
    QGrpcStatus status;
    QByteArray response;

private:
    QThread *thread;
    grpc::ClientContext context;
};

//! \private
struct QGrpcChannelPrivate {
    //! \private
    std::shared_ptr<grpc::Channel> m_channel;

    QGrpcChannelPrivate(const QUrl &url, std::shared_ptr<grpc::ChannelCredentials> credentials);
    ~QGrpcChannelPrivate();

    void call(const QString &method, const QString &service, const QByteArray &args, QGrpcCallReply *reply);
    QGrpcStatus call(const QString &method, const QString &service, const QByteArray &args, QByteArray &ret);
    void stream(QGrpcStream *stream, const QString &service, QAbstractGrpcClient *client);
};

};
