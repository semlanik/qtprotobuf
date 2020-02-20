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

#include "addressbookengine.h"

#include "qtprotobuf/examples/addressbook.qpb.h"
#include "qtprotobuf/examples/addressbook_grpc.qpb.h"

#include <QGrpcHttp2Channel>
#include <QGrpcUserPasswordCredentials>
#include <QGrpcSslCredentials>

#include <QDebug>
#include <QFile>
#include <QSslConfiguration>
#include <QCryptographicHash>

using namespace qtprotobuf::examples;

AddressBookEngine::AddressBookEngine() : QObject()
  , m_client(new AddressBookClient)
  , m_contacts(new ContactsListModel({}, this))
  , m_callStatus(CallStatus::Inactive)
{
    //Prepare ssl configuration
    QSslConfiguration conf = QSslConfiguration::defaultConfiguration();
    QFile certFile("cert.pem");
    certFile.open(QIODevice::ReadOnly);
    QByteArray cert = certFile.readAll();
    conf.setCaCertificates({QSslCertificate(cert)});
    conf.setProtocol(QSsl::TlsV1_2);
    conf.setAllowedNextProtocols({QSslConfiguration::ALPNProtocolHTTP2});

    std::shared_ptr<QtProtobuf::QAbstractGrpcChannel> channel(new QtProtobuf::QGrpcHttp2Channel(QUrl("https://localhost:65001"), QtProtobuf::QGrpcSslCredentials(conf) |
                                                                                      QtProtobuf::QGrpcUserPasswordCredentials<>("authorizedUser", QCryptographicHash::hash("test", QCryptographicHash::Md5).toHex())));
    m_client->attachChannel(channel);
    auto subscription = m_client->subscribeContactsUpdates(ListFrame());
    connect(subscription, &QtProtobuf::QGrpcSubscription::updated, this, [this, subscription]() {
        m_contacts->reset(subscription->read<Contacts>().list());
    });
    m_client->subscribeCallStatusUpdates(qtprotobuf::examples::None(), QPointer<CallStatus>(&m_callStatus));
}

void AddressBookEngine::addContact(qtprotobuf::examples::Contact *contact)
{
    Q_ASSERT(contact != nullptr);
    m_client->addContact(*contact);
}

void AddressBookEngine::makeCall(qtprotobuf::examples::PhoneNumber *phoneNumber)
{
    Q_ASSERT(phoneNumber != nullptr);
    m_client->makeCall(*phoneNumber);
}

void AddressBookEngine::endCall()
{
    m_client->endCall(qtprotobuf::examples::None());
}

AddressBookEngine::~AddressBookEngine()
{
    delete m_client;
    delete m_contacts;
}
