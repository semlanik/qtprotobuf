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

#pragma once //AbstractCredentials

#include <QString>
#include <QHash>
#include <QVariant>

#include <functional>
#include <memory>

#include "qtgrpcglobal.h"

namespace QtProtobuf {
/*!
*  \addtogroup QtGrpc
*  \{
*/

class CallCredentials;
class ChannelCredentials;

using CredentialMap = QHash<QLatin1String, QVariant>;

//! \private
class QGrpcAbstractCredentials {
public:
    virtual CredentialMap callCredentials() = 0;
    virtual CredentialMap channelCredentials() = 0;
};

/*!
 * \brief The CallCredentials class
 */
class Q_GRPC_EXPORT CallCredentials
{
public:
    CallCredentials() = default;
    virtual ~CallCredentials();
};

/*!
 * \brief The ChannelCredentials class
 */
class Q_GRPC_EXPORT ChannelCredentials
{
public:
    ChannelCredentials() = default;
    virtual ~ChannelCredentials();
};


/*!
 * \brief The QGrpcCredentials class
 */
template<typename Call, typename Channel,
         typename std::enable_if_t<std::is_base_of<QtProtobuf::CallCredentials, Call>::value
                                   && std::is_base_of<QtProtobuf::ChannelCredentials, Channel>::value, int> = 0>
class Q_GRPC_EXPORT QGrpcCredentials : public QGrpcAbstractCredentials
{
public:
    static QtProtobuf::QGrpcAbstractCredentials *withCredentials(const Call &call, const Channel &channel)
    {
        return new QGrpcCredentials<Call, Channel>(call, channel);
    }
    static QtProtobuf::QGrpcAbstractCredentials *withCallCredentials(const Call &call) {
        return new QGrpcCredentials<Call, Channel>(call);
    }
    static QtProtobuf::QGrpcAbstractCredentials *withChannelCredentials(const Channel &channel) {
        return new QGrpcCredentials<Call, Channel>(channel);
    }

    CredentialMap callCredentials() {
        return mCall();
    }

    CredentialMap channelCredentials() {
        return mChannel();
    }

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

protected:
    QGrpcCredentials() = default;

private:
    Call mCall;
    Channel mChannel;
};

/*! \} */
}

//! \private
template<typename Call, typename Channel,
         typename std::enable_if_t<std::is_base_of<QtProtobuf::CallCredentials, Call>::value
                                   && std::is_base_of<QtProtobuf::ChannelCredentials, Channel>::value, int> = 0>
std::unique_ptr<QtProtobuf::QGrpcAbstractCredentials> operator |(const Call &call, const Channel &channel)
{
    return std::unique_ptr<QtProtobuf::QGrpcAbstractCredentials>(QtProtobuf::QGrpcCredentials<Call, Channel>::withCredentials(call, channel));
}

//! \private
template<typename Call, typename Channel,
         typename std::enable_if_t<std::is_base_of<QtProtobuf::CallCredentials, Call>::value
                                   && std::is_base_of<QtProtobuf::ChannelCredentials, Channel>::value, int> = 0>
std::unique_ptr<QtProtobuf::QGrpcAbstractCredentials> operator |(const Channel &channel, const Call &call)
{
    return std::unique_ptr<QtProtobuf::QGrpcAbstractCredentials>(new QtProtobuf::QGrpcCredentials<Call, Channel>(call, channel));
}
