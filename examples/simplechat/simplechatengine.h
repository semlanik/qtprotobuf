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

#include <QObject>

#include "qtprotobuf/examples/simplechat.qpb.h"
#include "qtprotobuf/examples/simplechat_grpc.qpb.h"

#include "universallistmodel.h"

class QClipboard;

namespace qtprotobuf {
namespace examples {

using ChatMessageModel = UniversalListModel<qtprotobuf::examples::ChatMessage>;

class SimpleChatEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName NOTIFY userNameChanged)
    Q_PROPERTY(qtprotobuf::examples::ChatMessageModel *messages READ messages CONSTANT)
    Q_PROPERTY(qtprotobuf::examples::ChatMessage::ContentType clipBoardContentType READ clipBoardContentType NOTIFY clipBoardContentTypeChanged)

    ChatMessageModel m_messages;
    SimpleChatClient *m_client;
    QClipboard *m_clipBoard;
    QString m_userName;

public:
    explicit SimpleChatEngine(QObject *parent = nullptr);
    ~SimpleChatEngine();
    Q_INVOKABLE void login(const QString &name, const QString &password);
    Q_INVOKABLE void sendMessage(const QString &message);

    Q_INVOKABLE QString getText(const QByteArray &data) const { return QString::fromUtf8(data); }
    Q_INVOKABLE QString getImage(const QByteArray &data) const { return QString("data:image/png;base64,") + data.toBase64(); }
    Q_INVOKABLE QString getImageThumbnail(const QByteArray &data) const;
    Q_INVOKABLE void sendImageFromClipboard();

    ChatMessageModel *messages()
    {
        return &m_messages;
    }


    qtprotobuf::examples::ChatMessage::ContentType clipBoardContentType() const;

    QString userName() const
    {
        return m_userName;
    }

Q_SIGNALS:
    void loggedIn();
    void authFailed();
    void clipBoardContentTypeChanged();
    void userNameChanged();

};

}
}
Q_DECLARE_METATYPE(qtprotobuf::examples::ChatMessageModel*)
