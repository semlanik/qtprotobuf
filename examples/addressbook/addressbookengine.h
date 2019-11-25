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
#include "addressbook.pb.h"

#include "universallistmodel.h"

namespace qtprotobuf { namespace examples {
class AddressBookClient;
} }

using ContactsListModel = UniversalListModel<qtprotobuf::examples::Contact>;
using PhoneNumbersListModel = UniversalListModel<qtprotobuf::examples::PhoneNumber>;

class AddressBookEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ContactsListModel *contacts READ contacts NOTIFY contactsChanged)
    Q_PROPERTY(qtprotobuf::examples::CallStatus *callStatus READ callStatus NOTIFY callStatusChanged)
public:
    AddressBookEngine();
    virtual ~AddressBookEngine();

    ContactsListModel *contacts() const
    {
        return m_contacts;
    }

    qtprotobuf::examples::CallStatus *callStatus()
    {
        return &m_callStatus;
    }

    Q_INVOKABLE void addContact(qtprotobuf::examples::Contact *contact);
    Q_INVOKABLE void makeCall(qtprotobuf::examples::PhoneNumber *phoneNumber);
    Q_INVOKABLE void endCall();

signals:
    void contactsChanged();

    void callStatusChanged();

private:
    qtprotobuf::examples::AddressBookClient *m_client;
    ContactsListModel *m_contacts;
    qtprotobuf::examples::ContactRepeated m_container;
    qtprotobuf::examples::CallStatus m_callStatus;
};

Q_DECLARE_METATYPE(ContactsListModel*)
