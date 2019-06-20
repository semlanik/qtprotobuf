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

#include <QString>
#include <QHash>
#include <QVariant>

#include <functional>

#include "qtgrpcglobal.h"

namespace QtProtobuf {
/*!
*  \addtogroup QtGrpc
*  \{
*/

class CallCredentials;
class ChannelCredentials;

/*!
 * \brief The AbstractCredentials class
 */
class Q_GRPC_EXPORT AbstractCredentials
{
public:
    template<typename Call, typename Channel,
             typename std::enable_if_t<std::is_base_of<QtProtobuf::CallCredentials, Call>::value
                                       && std::is_base_of<QtProtobuf::ChannelCredentials, Channel>::value, int> = 0>
    AbstractCredentials(const Call &call, const Channel &channel) {
        mCallCredentials = call.mCallCredentials;
        mChannelCredentials = channel.mChannelCredentials;
    }

    template<typename Call,
             typename std::enable_if_t<std::is_base_of<QtProtobuf::CallCredentials, Call>::value, int> = 0>
    AbstractCredentials(const Call &call) {
        mCallCredentials = call.mCallCredentials;
    }

    template<typename Channel,
             typename std::enable_if_t<std::is_base_of<QtProtobuf::ChannelCredentials, Channel>::value, int> = 0>
    AbstractCredentials(const Channel &channel) {
        mChannelCredentials = channel.mChannelCredentials;
    }

    using CredentialMap = QHash<QLatin1String, QVariant>;

    CredentialMap callCredentials() {
        return mCallCredentials;
    }

    CredentialMap channelCredentials() {
        return mChannelCredentials;
    }
protected:
    AbstractCredentials() = default;
    void setCallCredentials(const CredentialMap &credentialMap) { mCallCredentials = credentialMap; }
    void setChannelCredentials(const CredentialMap &credentialMap) { mChannelCredentials = credentialMap; }

private:
    CredentialMap mCallCredentials;
    CredentialMap mChannelCredentials;
};

/*!
 * \brief The CallCredentials class
 */
class Q_GRPC_EXPORT CallCredentials : public AbstractCredentials
{
protected:
    CallCredentials(const CredentialMap &credentialMap) {
        setCallCredentials(credentialMap);
        setChannelCredentials(CredentialMap{});
    }

private:
    CallCredentials() = default;
};

/*!
 * \brief The ChannelCredentials class
 */
class Q_GRPC_EXPORT ChannelCredentials : public AbstractCredentials
{
protected:
    ChannelCredentials(const CredentialMap &credentialMap) {
        setCallCredentials(CredentialMap{});
        setChannelCredentials(credentialMap);
    }
};
/*! \} */
}

//! \private
template<typename Call, typename Channel,
         typename std::enable_if_t<std::is_base_of<QtProtobuf::CallCredentials, Call>::value
                                   && std::is_base_of<QtProtobuf::ChannelCredentials, Channel>::value, int> = 0>
QtProtobuf::AbstractCredentials operator |(const Call &call, const Channel &channel)
{
    return QtProtobuf::AbstractCredentials(call, channel);
}

//! \private
template<typename Call, typename Channel,
         typename std::enable_if_t<std::is_base_of<QtProtobuf::CallCredentials, Call>::value
                                   && std::is_base_of<QtProtobuf::ChannelCredentials, Channel>::value, int> = 0>
QtProtobuf::AbstractCredentials operator |(const Channel &channel, const Call &call)
{
    return QtProtobuf::AbstractCredentials(call, channel);
}
