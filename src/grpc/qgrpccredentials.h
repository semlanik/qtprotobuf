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

#pragma once //QGrpcCredentials

#include "qtgrpcglobal.h"
#include "qabstractgrpccredentials.h"

#include <memory>

namespace QtProtobuf {

/*!
 * \ingroup QtGrpc
 * \brief The QGrpcCallCredentials is base class for gRPC call credentials.
 *        You may inherrit of this calls to create you own credentials implementation,
 *        that will be used for each method call.
 *        Classes inherited of QGrpcCallCredentials should reimplement
 *        QtProtobuf::QGrpcCredentialMap operator() method and return actual credentials
 *        map for the call.
 */
class Q_GRPC_EXPORT QGrpcCallCredentials {};

/*!
 * \ingroup QtGrpc
 * \brief The QGrpcChannelCredentials is base class for gRPC channel credentials.
 *        Channel credentials are used by channel once while communication channel
 *        establishing. For example Ssl credentials or some session tockens.
 *        Classes inherited of QGrpcChannelCredentials should implement
 *        QGrpcCredentialMap channelCredentials() const method, that returns session
 *        parameters for specific channel.
 */
class Q_GRPC_EXPORT QGrpcChannelCredentials {};

/*!
 * \ingroup QtGrpc
 * \brief The QGrpcCredentials class is combination of call and channel credentials
 *        that is used by gRPC channels to communicate to services with given
 *        authentications parameters.
 */
template<typename Call, typename Channel,
         typename std::enable_if_t<std::is_base_of<QtProtobuf::QGrpcCallCredentials, Call>::value &&
                                   std::is_base_of<QtProtobuf::QGrpcChannelCredentials, Channel>::value, int> = 0>
class QGrpcCredentials final : public QAbstractGrpcCredentials
{
public:
    QGrpcCredentials(const Call &call, const Channel &channel) :
        mCall(call)
      , mChannel(channel)
    {
    }

    QGrpcCredentials(const Call &call) :
        mCall(call)
    {
    }

    QGrpcCredentials(const Channel &channel) :
        mChannel(channel)
    {
    }

    QGrpcCredentialMap callCredentials() override {
        return mCall();
    }

    QGrpcCredentialMap channelCredentials() override {
        return mChannel.channelCredentials();
    }

private:
    //! \private
    QGrpcCredentials() = default;

    Call mCall;
    Channel mChannel;
};

/*!
 * \brief SslConfigCredential
 */
extern Q_GRPC_EXPORT const char *SslConfigCredential;

/*! \} */
}

//! \private
template<typename Call, typename Channel,
         typename std::enable_if_t<std::is_base_of<QtProtobuf::QGrpcCallCredentials, Call>::value &&
                                   std::is_base_of<QtProtobuf::QGrpcChannelCredentials, Channel>::value, int> = 0>
std::unique_ptr<QtProtobuf::QAbstractGrpcCredentials> operator |(const Call &call, const Channel &channel)
{
    return std::unique_ptr<QtProtobuf::QAbstractGrpcCredentials>(new QtProtobuf::QGrpcCredentials<Call, Channel>(call, channel));
}

//! \private
template<typename Call, typename Channel,
         typename std::enable_if_t<std::is_base_of<QtProtobuf::QGrpcCallCredentials, Call>::value &&
                                   std::is_base_of<QtProtobuf::QGrpcChannelCredentials, Channel>::value, int> = 0>
std::unique_ptr<QtProtobuf::QAbstractGrpcCredentials> operator |(const Channel &channel, const Call &call)
{
    return std::unique_ptr<QtProtobuf::QAbstractGrpcCredentials>(new QtProtobuf::QGrpcCredentials<Call, Channel>(call, channel));
}
