/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Viktor Kopp <vifactor@gmail.com>
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

#pragma once //QGrpcHttp2Channel

#include <QtGrpc/qabstractgrpcchannel.h>

#include <QUrl>
#include <memory>

QT_BEGIN_NAMESPACE

class QAbstractGrpcCredentials;
struct QGrpcHttp2ChannelPrivate;
/*!
 * \ingroup QtGrpc
 * \brief The QGrpcHttp2Channel class is HTTP/2 implementation of QAbstractGrpcChannel interface
 * \details For QGrpcHttp2Channel utilizes channel and call credentials.
 *          For channel credential QGrpcHttp2Channel supports QGrpcSslConfigCredential key. When https
 *          is used, this key has to be explicitly specified and provide QSslConfiguration and value.
 *          Provided QSslConfiguration will be used to establish HTTP/2 secured connection.
 *          All keys passed as QGrpcCallCredentials will be used as HTTP/2 headers with related values
 *          assigned.
 */
class Q_GRPC_EXPORT QGrpcHttp2Channel final : public QAbstractGrpcChannel
{
public:
    /*!
     * \brief QGrpcHttp2Channel constructs QGrpcHttp2Channel
     * \param url http/https url used to establish channel connection
     * \param credentials call/channel credentials pair
     */
    QGrpcHttp2Channel(const QUrl &url, std::unique_ptr<QAbstractGrpcCredentials> credentials);
    ~QGrpcHttp2Channel();

    QGrpcStatus call(const QString &method, const QString &service, const QByteArray &args, QByteArray &ret) override;
    void call(const QString &method, const QString &service, const QByteArray &args, QGrpcCallReply *reply) override;
    void stream(QGrpcStream *stream, const QString &service, QAbstractGrpcClient *client) override;
    std::shared_ptr<QAbstractProtobufSerializer> serializer() const override;
private:
    Q_DISABLE_COPY_MOVE(QGrpcHttp2Channel)

    std::unique_ptr<QGrpcHttp2ChannelPrivate> dPtr;
};

QT_END_NAMESPACE
