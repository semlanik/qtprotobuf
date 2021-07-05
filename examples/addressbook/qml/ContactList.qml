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
import QtQuick.Layouts 1.1
import qtprotobuf.examples 1.0

StackItem {
    id: root
    hasBack: false
    property alias model: contactList.model
    signal requestAddContact()
    signal showContact(var contact)
    ListView {
        id: contactList
        anchors.fill: parent
        delegate: Item {
            id: contactDelegate
            property Contact contact: model.modelData
            width: contactList.width
            height: 70
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                Row {
                    Layout.alignment: Qt.AlignVCenter
                    spacing: 5
                    PrimaryText {
                        id: firstName
                        text: contactDelegate.contact.firstName
                    }
                    PrimaryText {
                        id: middleName
                        text: contactDelegate.contact.middleName
                    }
                    PrimaryText {
                        id: lastName
                        text: contactDelegate.contact.lastName
                    }
                }
                Row {
                    Layout.alignment: Qt.AlignVCenter
                    SecondaryText {
                        id: defaultPhoneNumberText
                        property PhoneNumber defaultPhoneNumber: contactDelegate.contact.phones.length > 0 ?
                                                                     contactDelegate.contact.phones[0] : null
                        visible: defaultPhoneNumber != null
                        text: defaultPhoneNumber ?
                                  "+" + defaultPhoneNumber.countryCode + " " + defaultPhoneNumber.number : ""
                    }
                }
            }
            Rectangle {
                color:"#cfdfe7"
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
            }

            Rectangle {
                color:"#cfdfe7"
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.bottom
                height: 1
                visible: (contactList.count - 1) === model.index
            }
            MouseArea {
                anchors.fill: parent
                onClicked: showContact(contactDelegate.contact)
            }
        }
    }

    FloatingRoundButton {
        enabled: !contactList.moving
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 10
        icon: "qrc:/images/plus.png"
        onClicked: root.requestAddContact()
    }
}
