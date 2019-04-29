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

#include "simplechatengine.h"

#include <http2channel.h>
#include <insecurecredentials.h>
#include <sslcredentials.h>

#include <QDebug>
#include <QFile>
#include <QSslConfiguration>
#include <QCryptographicHash>
#include <QDateTime>

using namespace qtprotobuf::examples;

class AuthCredentials : public qtprotobuf::CallCredentials
{
public:
    AuthCredentials(const QString &userName, const QString &password) :
        CallCredentials(CredentialMap{{QLatin1String("user-name"), QVariant::fromValue(userName)},
                                      {QLatin1String("user-password"), QVariant::fromValue(password)}}) {}
};

SimpleChatEngine::SimpleChatEngine(QObject *parent) : QObject(parent), m_client(new SimpleChatClient)
{

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

    std::shared_ptr<qtprotobuf::AbstractChannel> channel(new qtprotobuf::Http2Channel("localhost", 65002, qtprotobuf::SslCredentials(conf) |
                                                                                      AuthCredentials(name, QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex())));

    m_client->attachChannel(channel);
    m_client->subscribeMessageListUpdates(None());
    QObject::connect(m_client, &SimpleChatClient::messageListUpdated, this, [this](const qtprotobuf::examples::ChatMessages &messages){
        loggedIn();
        m_messages.reset(messages.messages());
    });
}

void SimpleChatEngine::sendMessage(const QString &content)
{
    m_client->sendMessage(ChatMessage(QDateTime::currentMSecsSinceEpoch(), content.toUtf8(), ChatMessage::ContentType::Text));
}
