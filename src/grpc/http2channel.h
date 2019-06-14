/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Viktor Kopp <vifactor@gmail.com>
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

#include "abstractchannel.h"

#include <QUrl>

namespace qtprotobuf {

class AbstractCredentials;

/*!
 * \ingroup QtGrpc
 * \brief The Http2Channel class
 */
class Q_GRPC_EXPORT Http2Channel final : public AbstractChannel
{
public:
    [[deprecated ("This contructor is obsolete and is going to be removed soon. Use Http2Channel(const QUrl&, const AbstractCredentials&)")]]
    Http2Channel(const QString &addr, quint16 port, const AbstractCredentials &credentials);
    Http2Channel(const QUrl &url, const AbstractCredentials &credentials);
    ~Http2Channel();

    StatusCode call(const QString &method, const QString &service, const QByteArray &args, QByteArray &ret) override;
    void call(const QString &method, const QString &service, const QByteArray &args, qtprotobuf::AsyncReply *reply) override;
    void subscribe(const QString &method, const QString &service, const QByteArray &args, AbstractClient *client, const std::function<void (const QByteArray &)> &handler) override;

protected:
    void abort(AsyncReply *reply) override;

private:
    Q_DISABLE_COPY(Http2Channel)

    // PIMPL
    struct Http2ChannelPrivate *d;
};
}
