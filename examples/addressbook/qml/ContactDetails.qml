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

import QtQuick 2.9
import QtQuick.Controls 2.4

import qtprotobuf.examples 1.0

StackItem {
    id: root
    property Contact contact: null

    Flickable {
        anchors.fill: parent
        contentHeight: innerColumn.height + 70 + 10
        contentWidth: parent.width
        Column {
            id: innerColumn
            anchors.right: parent.right
            anchors.left: parent.left
            TextRow {
                id: firstNameField
                label: qsTr("First name")
                text: contact.firstName
            }
            TextRow {
                id: middleNameField
                label: qsTr("Middle name")
                text: contact.middleName
            }
            TextRow {
                id: lastNameField
                label: qsTr("Last name")
                text: contact.lastName
            }
            TextRow {
                id: _homePhoneField
                label: qsTr("Home phone")
                text: _homePhone ? "+" + _homePhone.countryCode + " " + _homePhone.number : ""
                property PhoneNumber _homePhone: contact.phonesData[0]
            }
            DropDownColumn {
                anchors.right: parent.right
                anchors.left: parent.left
                width: root.width
                header: qsTr("Job")
                value: _jobTitle.text

                TextRow {
                    id: _jobTitle
                    label: qsTr("Title")
                    text: contact.job.title
                }
                Item {
                    height: 70
                    anchors.right: parent.right
                    anchors.left: parent.left
                    PrimaryText {
                        anchors.bottom: parent.bottom
                        text: qsTr("Address")
                    }
                }
                TextRow {
                    id: _jstreetAddress1
                    label: qsTr("Street address 1")
                    text: contact.job.officeAddress.streetAddress1
                }
                TextRow {
                    id: _jstreetAddress2
                    label: qsTr("Street address 2")
                    text: contact.job.officeAddress.streetAddress2
                }
                TextRow {
                    id: _jzipCode
                    label: qsTr("Zip code")
                    text: contact.job.officeAddress.zipCode
                }
                TextRow {
                    id: _jstate
                    label: qsTr("State")
                    text: contact.job.officeAddress.state
                }
                TextRow {
                    id: _jCountry
                    label: qsTr("Country")
                    text: contact.job.officeAddress.country
                }
            }

            DropDownColumn {
                anchors.right: parent.right
                anchors.left: parent.left
                width: root.width
                header: qsTr("Home address")

                TextRow {
                    id: _streetAddress1
                    label: qsTr("Street address 1")
                    text: contact.address.streetAddress1
                }
                TextRow {
                    id: _streetAddress2
                    label: qsTr("Street address 2")
                    text: contact.address.streetAddress2
                }
                TextRow {
                    id: _zipCode
                    label: qsTr("Zip code")
                    text: contact.address.zipCode
                }
                TextRow {
                    id: _state
                    label: qsTr("State")
                    text: contact.address.state
                }
                TextRow {
                    id: _country
                    label: qsTr("Country")
                    text: contact.address.country
                }
            }
        }
    }

    FloatingRoundButton {
        id: call
        enabled: _homePhoneField.text.length > 0
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        icon: "qrc:/images/call.png"
        primaryColor: "#4CAF50"
        secondaryColor: "#58cb5c"
        onClicked: {
            abEngine.makeCall(contact.phonesData[0])
            stack.pop();
        }
    }
}
