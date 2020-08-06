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

#include "qgrpcchannel.h"
#include "qgrpcchannel_p.h"

#include <QEventLoop>
#include <QThread>

#include <memory>
#include <thread>
#include <unordered_map>

#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/impl/codegen/byte_buffer.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/slice.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/sync_stream_impl.h>
#include <grpcpp/security/credentials.h>

#include "qabstractgrpccredentials.h"
#include "qgrpcasyncreply.h"
#include "qgrpcstatus.h"
#include "qgrpcsubscription.h"
#include "qabstractgrpcclient.h"
#include "qgrpccredentials.h"
#include "qprotobufserializerregistry_p.h"
#include "qtprotobuflogging.h"

using namespace QtProtobuf;

namespace QtProtobuf {

static inline grpc::Status parseByteBuffer(const grpc::ByteBuffer &buffer, QByteArray &data)
{
    std::vector<grpc::Slice> slices;
    auto status = buffer.Dump(&slices);

    if (!status.ok())
        return status;

    for (auto slice : slices) {
        data.append(QByteArray((const char *)slice.begin(), slice.size()));
    }

    return grpc::Status::OK;
}

static inline void parseQByteArray(const QByteArray &bytearray, grpc::ByteBuffer &buffer)
{
    grpc::Slice slice(bytearray.data(), bytearray.size());
    grpc::ByteBuffer tmp(&slice, 1);
    buffer.Swap(&tmp);
}

QGrpcChannelSubscription::QGrpcChannelSubscription(grpc::Channel *channel, const QString &method, const QByteArray &data, QObject *parent) : QObject(parent)
{
    grpc::ByteBuffer request;
    parseQByteArray(data, request);

    reader = grpc_impl::internal::ClientReaderFactory<grpc::ByteBuffer>::Create(channel,
        grpc::internal::RpcMethod(method.toStdString().c_str(), grpc::internal::RpcMethod::SERVER_STREAMING),
        &context, request);

    thread = QThread::create([this](){
        grpc::ByteBuffer response;
        QByteArray data;
        grpc::Status status;

        while (reader->Read(&response)) {
            status = parseByteBuffer(response, data);

            if (!status.ok()) {
                this->status = {
                    (QGrpcStatus::StatusCode) status.error_code(),
                    status.error_message().c_str()
                };

                return; // exit thread
            }

            emit this->dataReady(data);
        }

        status = reader->Finish();

        this->status = {
            (QGrpcStatus::StatusCode) status.error_code(),
            status.error_message().c_str()
        };

        return; // exit thread
    });

    connect(thread, &QThread::finished, this, &QGrpcChannelSubscription::finished);
}

void QGrpcChannelSubscription::start()
{
    thread->start();
}

QGrpcChannelSubscription::~QGrpcChannelSubscription()
{
    cancel();
    thread->wait();
    thread->deleteLater();

    if (reader != nullptr) {
        delete reader;
    }
}

void QGrpcChannelSubscription::cancel() {
    // TODO: check thread safety
    qProtoDebug() << "Subscription thread terminated";
    context.TryCancel();
}

QGrpcChannelCall::QGrpcChannelCall(grpc::Channel *channel, const QString &method, const QByteArray &data, QObject *parent) : QObject(parent) {
    grpc::ByteBuffer request;
    parseQByteArray(data, request);

    thread = QThread::create([this, request, channel, method](){
        grpc::ByteBuffer response;
        QByteArray data;
        grpc::Status status;

        status = grpc::internal::BlockingUnaryCall(channel,
            grpc::internal::RpcMethod(method.toStdString().c_str(), grpc::internal::RpcMethod::NORMAL_RPC),
            &context, request, &response
        );

        if (!status.ok()) {
            this->status = {
                static_cast<QGrpcStatus::StatusCode>(status.error_code()),
                status.error_message().c_str()
            };

            return; // exit thread
        }

        status = parseByteBuffer(response, this->response);

        this->status = {
            static_cast<QGrpcStatus::StatusCode>(status.error_code()),
            status.error_message().c_str()
        };
    });

    connect(thread, &QThread::finished, this, &QGrpcChannelCall::finished);
}

void QGrpcChannelCall::start()
{
    thread->start();
}

QGrpcChannelCall::~QGrpcChannelCall()
{
    cancel();
    thread->wait();
    thread->deleteLater();
}

void QGrpcChannelCall::cancel()
{
    // TODO: check thread safety
    qProtoDebug() << "Call thread terminated";
    context.TryCancel();
}

QGrpcChannelPrivate::QGrpcChannelPrivate(const QUrl &url, std::shared_ptr<grpc::ChannelCredentials> credentials)
{
    m_channel = grpc::CreateChannel(url.toString().toStdString(), credentials);
}

QGrpcChannelPrivate::~QGrpcChannelPrivate()
{
}

void QGrpcChannelPrivate::call(const QString &method, const QString &service, const QByteArray &args, QGrpcAsyncReply *reply)
{
    QString rpcName = QString("/%1/%2").arg(service).arg(method);

    std::shared_ptr<QGrpcChannelCall> call;
    std::shared_ptr<QMetaObject::Connection> connection(new QMetaObject::Connection);
    std::shared_ptr<QMetaObject::Connection> abortConnection(new QMetaObject::Connection);

    call.reset(
        new QGrpcChannelCall(m_channel.get(), rpcName, args, reply),
        [](QGrpcChannelCall * c) { c->deleteLater(); }
    );

    *connection = QObject::connect(call.get(), &QGrpcChannelCall::finished, reply, [call, reply, connection, abortConnection](){
        if (call->status.code() == QGrpcStatus::Ok) {
            reply->setData(call->response);
            reply->finished();
        } else {
            reply->setData({});
            reply->error(call->status);
        }

        QObject::disconnect(*connection);
        QObject::disconnect(*abortConnection);
    });

    *abortConnection = QObject::connect(reply, &QGrpcAsyncReply::error, call.get(), [call, connection, abortConnection](const QGrpcStatus &status){
        if (status.code() == QGrpcStatus::Aborted) {
            QObject::disconnect(*connection);
            QObject::disconnect(*abortConnection);
        }
    });

    call->start();
}

QGrpcStatus QGrpcChannelPrivate::call(const QString &method, const QString &service, const QByteArray &args, QByteArray &ret)
{
    QEventLoop loop;

    QString rpcName = QString("/%1/%2").arg(service).arg(method);
    QGrpcChannelCall call(m_channel.get(), rpcName, args);

    QObject::connect(&call, &QGrpcChannelCall::finished, &loop, &QEventLoop::quit);

    call.start();
    loop.exec();

    ret = call.response;
    return call.status;
}

void QGrpcChannelPrivate::subscribe(QGrpcSubscription *subscription, const QString &service, QAbstractGrpcClient *client)
{
    assert(subscription != nullptr);

    QString rpcName = QString("/%1/%2").arg(service).arg(subscription->method());

    std::shared_ptr<QGrpcChannelSubscription> sub;
    std::shared_ptr<QMetaObject::Connection> abortConnection(new QMetaObject::Connection);
    std::shared_ptr<QMetaObject::Connection> readConnection(new QMetaObject::Connection);
    std::shared_ptr<QMetaObject::Connection> clientConnection(new QMetaObject::Connection);
    std::shared_ptr<QMetaObject::Connection> connection(new QMetaObject::Connection);

    sub.reset(
        new QGrpcChannelSubscription(m_channel.get(), rpcName, subscription->arg(), subscription),
        [](QGrpcChannelSubscription * sub) { sub->deleteLater(); }
    );

    *readConnection = QObject::connect(sub.get(), &QGrpcChannelSubscription::dataReady, subscription, [subscription](const QByteArray &data) {
        subscription->handler(data);
    });

    *connection = QObject::connect(sub.get(), &QGrpcChannelSubscription::finished, subscription, [sub, subscription, readConnection, abortConnection, service, connection, clientConnection](){
        qProtoDebug() << "Subscription ended with server closing connection";

        QObject::disconnect(*connection);
        QObject::disconnect(*readConnection);
        QObject::disconnect(*abortConnection);
        QObject::disconnect(*clientConnection);

        if (sub->status.code() != QGrpcStatus::Ok)
        {
            subscription->error(sub->status);
        }
    });

    *abortConnection = QObject::connect(subscription, &QGrpcSubscription::finished, sub.get(), [connection, abortConnection, readConnection, sub, clientConnection] {
        qProtoDebug() << "Subscription client was finished";

        QObject::disconnect(*connection);
        QObject::disconnect(*readConnection);
        QObject::disconnect(*abortConnection);
        QObject::disconnect(*clientConnection);

        sub->cancel();
    });

    *clientConnection = QObject::connect(client, &QAbstractGrpcClient::destroyed, sub.get(), [readConnection, connection, abortConnection, sub, clientConnection](){
        qProtoDebug() << "Grpc client was destroyed";

        QObject::disconnect(*connection);
        QObject::disconnect(*readConnection);
        QObject::disconnect(*abortConnection);
        QObject::disconnect(*clientConnection);

        sub->cancel();
    });

    sub->start();
}

QGrpcChannel::QGrpcChannel(const QUrl &url, std::shared_ptr<grpc::ChannelCredentials> credentials) : QAbstractGrpcChannel()
  , dPtr(std::make_unique<QGrpcChannelPrivate>(url, credentials))
{
}

QGrpcChannel::~QGrpcChannel()
{
}

QGrpcStatus QGrpcChannel::call(const QString &method, const QString &service, const QByteArray &args, QByteArray &ret)
{
    return dPtr->call(method, service, args, ret);
}

void QGrpcChannel::call(const QString &method, const QString &service, const QByteArray &args, QGrpcAsyncReply *reply)
{
    dPtr->call(method, service, args, reply);
}

void QGrpcChannel::subscribe(QGrpcSubscription *subscription, const QString &service, QAbstractGrpcClient *client)
{
    dPtr->subscribe(subscription, service, client);
}

std::shared_ptr<QAbstractProtobufSerializer> QGrpcChannel::serializer() const
{
    //TODO: make selection based on credentials or channel settings
    return QProtobufSerializerRegistry::instance().getSerializer("protobuf");
}

}