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

#pragma once //QGrpcInsecureCredentials

#include <QtGrpc/qgrpccredentials.h>
#include <QtGrpc/qtgrpcglobal.h>

QT_BEGIN_NAMESPACE
/*!
 * \ingroup QtGrpc
 * \brief The QGrpcInsecureCallCredentials class is dummy implementation of
 *        QGrpcCallCredentials. It doesn't provide any extra information to channel.
 *
 *        Use it when you don't need any call credentials.
 */
class Q_GRPC_EXPORT QGrpcInsecureCallCredentials final : public QGrpcCallCredentials
{
public:
    QGrpcCredentialMap operator()() const {
        return QGrpcCredentialMap{};
    }
};

/*!
 * \ingroup QtGrpc
 * \brief The QGrpcInsecureChannelCredentials class is dummy implementation of
 *        QGrpcChannelCredentials. It doesn't provide any extra information to channel.
 *
 *        Use it when you don't need any channel credentials.
 */
class Q_GRPC_EXPORT QGrpcInsecureChannelCredentials final : public QGrpcChannelCredentials
{
public:
    QGrpcCredentialMap channelCredentials() const {
        return QGrpcCredentialMap{};
    }
};

QT_END_NAMESPACE
