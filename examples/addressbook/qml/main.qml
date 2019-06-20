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

import QtQuick 2.9
import QtQuick.Controls 2.4

import qtprotobuf.examples 1.0
import examples.addressbook 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("QtProtobuf Address Book Example")
    Rectangle {
        id: background
        anchors.fill: parent
        color: "#B0BEC5"
    }

    StackView {
        id: mainStack
        anchors.fill: parent
    }

    Component {
        id: contactList
        ContactList {
            anchors.fill: background
            model: abEngine.contacts
            Component.onCompleted: {
                console.log("abEngine.contacts.list.length " + abEngine.contacts.count)
            }
            onRequestAddContact: {
                mainStack.push(addContactView, {"stack": mainStack})
            }
            onShowContact: {
                mainStack.push(contactDetailsView, {"stack": mainStack, "contact": contact})
            }
        }
    }
    Component {
        id: addContactView
        AddContactView {
            anchors.fill: background
        }
    }

    Component {
        id: contactDetailsView
        ContactDetails  {
            anchors.fill: background
        }
    }

    Connections {
        target: abEngine.callStatus
        onStatusChanged: {
            console.log("Call status: " + abEngine.callStatus.status);
        }
    }

    CallPopup {
        id: activeCallPopup
        width: 300
        height: 170
        anchors.centerIn: parent
        callStatus: abEngine.callStatus
    }

    Component.onCompleted: {
        mainStack.push(contactList, {"stack": mainStack})
    }
}



