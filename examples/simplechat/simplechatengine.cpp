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

#include "simplechatengine.h"

#include "simplechat_grpc.qpb.h"

#include <QGrpcHttp2Channel>
#include <QGrpcUserPasswordCredentials>
#include <QGrpcSslCredentials>

#include <QDebug>
#include <QFile>
#include <QSslConfiguration>
#include <QCryptographicHash>
#include <QDateTime>
#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>
#include <QImage>
#include <QByteArray>
#include <QBuffer>

using namespace qtprotobuf::examples;

SimpleChatEngine::SimpleChatEngine(QObject *parent) : QObject(parent), m_client(new SimpleChatClient)
  , m_clipBoard(QGuiApplication::clipboard())
{
    if (m_clipBoard) {
        connect(m_clipBoard, &QClipboard::dataChanged, this, &SimpleChatEngine::clipBoardContentTypeChanged);
    }
}

SimpleChatEngine::~SimpleChatEngine()
{
    delete m_client;
}

void SimpleChatEngine::login(const QString &name, const QString &password)
{
    QSslConfiguration conf = QSslConfiguration::defaultConfiguration();
    QFile certFile("cert.pem");
    certFile.open(QIODevice::ReadOnly);
    QByteArray cert = certFile.readAll();
    conf.setCaCertificates({QSslCertificate(cert)});
    conf.setProtocol(QSsl::TlsV1_2);
    conf.setAllowedNextProtocols({QSslConfiguration::ALPNProtocolHTTP2});

    QUrl url("https://localhost:65002");
    std::shared_ptr<QtProtobuf::QAbstractGrpcChannel> channel(new QtProtobuf::QGrpcHttp2Channel(url, QtProtobuf::QGrpcSslCredentials(conf) |
                                                                                      QtProtobuf::QGrpcUserPasswordCredentials<>(name, QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex())));

    m_client->attachChannel(channel);
    QtProtobuf::QGrpcStreamShared stream = m_client->streamMessageList(None());
    QObject::connect(stream.get(), &QtProtobuf::QGrpcStream::error, this, [stream] {
        qCritical() << "Stream error, cancel";
        stream->abort();
    });
    QObject::connect(stream.get(), &QtProtobuf::QGrpcStream::messageReceived, this, [this, name, stream]() {
        if (m_userName != name) {
            m_userName = name;
            userNameChanged();
            loggedIn();
        }
        m_messages.reset(stream->read<qtprotobuf::examples::ChatMessages>().messages());
    });
}

void SimpleChatEngine::sendMessage(const QString &content)
{
    m_client->sendMessage(ChatMessage(static_cast<quint64>(QDateTime::currentMSecsSinceEpoch()), content.toUtf8(), ChatMessage::ContentType::Text));
}

qtprotobuf::examples::ChatMessage::ContentType SimpleChatEngine::clipBoardContentType() const
{
    if (m_clipBoard != nullptr) {
        const QMimeData *mime = m_clipBoard->mimeData();
        if (mime != nullptr) {
            if (mime->hasImage() || mime->hasUrls()) {
                return qtprotobuf::examples::ChatMessage::ContentType::Image;
            } else if (mime->hasText()) {
                return qtprotobuf::examples::ChatMessage::ContentType::Text;
            }
        }
    }
    return qtprotobuf::examples::ChatMessage::Unknown;
}

void SimpleChatEngine::sendImageFromClipboard()
{
    if (m_clipBoard == nullptr) {
        return;
    }

    QByteArray imgData;
    const QMimeData *mime = m_clipBoard->mimeData();
    if (mime != nullptr) {
        if (mime->hasImage()) {
            QImage img = mime->imageData().value<QImage>();
            QBuffer buffer(&imgData);
            buffer.open(QIODevice::WriteOnly);
            img.save(&buffer, "PNG");
            buffer.close();
        } else if (mime->hasUrls()) {
            QUrl imgUrl = mime->urls().first();
            if (!imgUrl.isLocalFile()) {
                qWarning() << "Only supports transfer of local images";
                return;
            }
            QImage img(imgUrl.toLocalFile());
            if (img.isNull()) {
                qWarning() << "Invalid image format";
                return;
            }

            QBuffer buffer(&imgData);
            buffer.open(QIODevice::WriteOnly);
            img.save(&buffer, "PNG");
            buffer.close();
        }
    }

    if (imgData.isEmpty()) {
        return;
    }

    m_client->sendMessage(ChatMessage(static_cast<quint64>(QDateTime::currentMSecsSinceEpoch()), imgData, qtprotobuf::examples::ChatMessage::ContentType::Image));
}

QString SimpleChatEngine::getImageThumbnail(const QByteArray &data) const
{
    QImage img = QImage::fromData(data, "PNG");
    img = img.scaled(200, 200, Qt::KeepAspectRatio);
    QByteArray scaledData;
    QBuffer buffer(&scaledData);
    img.save(&buffer, "PNG");
    return getImage(scaledData);
}
