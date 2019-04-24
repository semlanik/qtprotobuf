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

#include "addressbookengine.h"
#include "addressbookclient.h"
#include <http2channel.h>
#include <insecurecredentials.h>
#include <sslcredentials.h>

#include <QDebug>
#include <QFile>
#include <QSslConfiguration>
#include <QCryptographicHash>

using namespace qtprotobuf::examples;

class AuthCredentials : public qtprotobuf::CallCredentials
{
public:
    AuthCredentials(const QString &userName, const QString &password) :
        CallCredentials(CredentialMap{{QLatin1String("user-name"), QVariant::fromValue(userName)},
                                      {QLatin1String("user-password"), QVariant::fromValue(password)}}) {}
};

AddressBookEngine::AddressBookEngine() : QObject()
  , m_client(new AddressBookClient)
  , m_contacts(new ContactsListModel({}, this))
{
    //Prepare ssl configuration
    QSslConfiguration conf = QSslConfiguration::defaultConfiguration();
    QFile certFile("cert.pem");
    certFile.open(QIODevice::ReadOnly);
    QByteArray cert = certFile.readAll();
    conf.setCaCertificates({QSslCertificate(cert)});
    conf.setProtocol(QSsl::TlsV1_2);
    conf.setAllowedNextProtocols({QSslConfiguration::ALPNProtocolHTTP2});

    std::shared_ptr<qtprotobuf::AbstractChannel> channel(new qtprotobuf::Http2Channel("localhost", 65001, qtprotobuf::SslCredentials(conf) |
                                                                                      AuthCredentials("authorizedUser", QCryptographicHash::hash("test", QCryptographicHash::Md5).toHex())));
    m_client->attachChannel(channel);
    m_client->subscribeContactsUpdates(ListFrame());
    connect(m_client, &AddressBookClient::contactsUpdated, this, [this](const Contacts &contacts) {
        m_contacts->reset(contacts.list());
    });
}

void AddressBookEngine::addContact(qtprotobuf::examples::Contact *contact)
{
    m_client->addContact(*contact);
}

AddressBookEngine::~AddressBookEngine()
{
    delete m_client;
}
