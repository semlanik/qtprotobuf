/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
 *
 * This file is part of qtprotobuf project https://git.semlanik.org/semlanik/qtprotobuf
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

namespace qtprotobuf {

class CallCredentials;
class ChannelCredentials;

class AbstractCredentials
{
public:
    template<typename Call, typename Channel,
             typename std::enable_if_t<std::is_base_of<qtprotobuf::CallCredentials, Call>::value
                                       && std::is_base_of<qtprotobuf::ChannelCredentials, Channel>::value, int> = 0>
    AbstractCredentials(const Call &call, const Channel &channel);

    template<typename Call,
             typename std::enable_if_t<std::is_base_of<qtprotobuf::CallCredentials, Call>::value, int> = 0>
    AbstractCredentials(const Call &call);

    template<typename Channel,
             typename std::enable_if_t<std::is_base_of<qtprotobuf::ChannelCredentials, Channel>::value, int> = 0>
    AbstractCredentials(const Channel &channel);

    using CredentialMap = QHash<QLatin1String, QVariant>;

    CredentialMap callCredentials() {
        if (callCredentials_p) {
            return callCredentials_p();
        }
        return CredentialMap();
    }

    CredentialMap channelCredentials() {
        if (channelCredentials_p) {
            return channelCredentials_p();
        }
        return CredentialMap();
    }
protected:
    AbstractCredentials() = default;

    using CredentialsFunction = std::function<CredentialMap(void)>;
    CredentialsFunction callCredentials_p;
    CredentialsFunction channelCredentials_p;
};

class CallCredentials : public AbstractCredentials
{
protected:
    CallCredentials() {
        channelCredentials_p = [](){ return CredentialMap(); };
    }

private:
    using AbstractCredentials::channelCredentials_p;
};

class ChannelCredentials : public AbstractCredentials
{
protected:
    ChannelCredentials() {
        callCredentials_p = [](){ return CredentialMap(); };
    }
private:
    using AbstractCredentials::callCredentials_p;
};

template<typename Call, typename Channel,
         typename std::enable_if_t<std::is_base_of<qtprotobuf::CallCredentials, Call>::value
                                   && std::is_base_of<qtprotobuf::ChannelCredentials, Channel>::value, int> = 0>
AbstractCredentials::AbstractCredentials(const Call &call, const Channel &channel)
{
    callCredentials_p = call.callCredentials_p;
    channelCredentials_p = channel.channelCredentials_p;
}

template<typename Call,
         typename std::enable_if_t<std::is_base_of<qtprotobuf::CallCredentials, Call>::value, int> = 0>
AbstractCredentials::AbstractCredentials(const Call &call)
{
    callCredentials_p = call.callCredentials_p;
}

template<typename Channel,
         typename std::enable_if_t<std::is_base_of<qtprotobuf::ChannelCredentials, Channel>::value, int> = 0>
AbstractCredentials::AbstractCredentials(const Channel &channel)
{
    channelCredentials_p = channel.channelCredentials_p;
}
}

template<typename Call, typename Channel,
         typename std::enable_if_t<std::is_base_of<qtprotobuf::CallCredentials, Call>::value
                                   && std::is_base_of<qtprotobuf::ChannelCredentials, Channel>::value, int> = 0>
qtprotobuf::AbstractCredentials operator|(const Call &call, const Channel &channel)
{
    return qtprotobuf::AbstractCredentials(call, channel);
}

template<typename Call, typename Channel,
         typename std::enable_if_t<std::is_base_of<qtprotobuf::CallCredentials, Call>::value
                                   && std::is_base_of<qtprotobuf::ChannelCredentials, Channel>::value, int> = 0>
qtprotobuf::AbstractCredentials operator|(const Channel &channel, const Call &call)
{
    return qtprotobuf::AbstractCredentials(call, channel);
}
