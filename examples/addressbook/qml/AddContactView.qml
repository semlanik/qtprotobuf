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

StackItem {
    id: root
    Flickable {
        anchors.fill: parent
        contentHeight: innerColumn.height + 70 + 10
        contentWidth: parent.width
        Column {
            id: innerColumn
            anchors.right: parent.right
            anchors.left: parent.left
            TextInputRow {
                id: firstNameField
                label: qsTr("First name")
            }
            TextInputRow {
                id: middleNameField
                label: qsTr("Middle name")
            }
            TextInputRow {
                id: lastNameField
                label: qsTr("Last name")
            }
            PhoneInput {
                id: homePhone
                label: qsTr("Home phone")
                PhoneNumber {
                    id: _homePhoneData
                    countryCode: homePhone.countryCode
                    number: parseInt(homePhone.number, 10)
                }
            }

            DropDownColumn {
                anchors.right: parent.right
                anchors.left: parent.left
                width: root.width
                header: qsTr("Job")
                value: _jobTitle.text

                TextInputRow {
                    id: _jobTitle
                    label: qsTr("Title")
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
                TextInputRow {
                    id: _jstreetAddress1
                    label: qsTr("Street address 1")
                }
                TextInputRow {
                    id: _jstreetAddress2
                    label: qsTr("Street address 2")
                }
                TextInputRow {
                    id: _jzipCode
                    label: qsTr("Zip code")
                }
                TextInputRow {
                    id: _jstate
                    label: qsTr("State")
                }
                TextInputRow {
                    id: _jCountry
                    label: qsTr("Country")
                }
            }

            DropDownColumn {
                anchors.right: parent.right
                anchors.left: parent.left
                width: root.width
                header: qsTr("Home address")

                TextInputRow {
                    id: _streetAddress1
                    label: qsTr("Street address 1")
                }
                TextInputRow {
                    id: _streetAddress2
                    label: qsTr("Street address 2")
                }
                TextInputRow {
                    id: _zipCode
                    label: qsTr("Zip code")
                }
                TextInputRow {
                    id: _state
                    label: qsTr("State")
                }
                TextInputRow {
                    id: _country
                    label: qsTr("Country")
                }
            }
        }
    }

    property Contact newContact: Contact {
        firstName: firstNameField.text
        middleName: middleNameField.text
        lastName: lastNameField.text
        job.title: _jobTitle.text
//        job.officeAddress.country: _jcountry.text
        job.officeAddress.streetAddress1: _jstreetAddress1.text
        job.officeAddress.streetAddress2: _jstreetAddress2.text
        job.officeAddress.zipCode: parseInt(_jzipCode.text, 10)
        job.officeAddress.state: _jstate.text

//        address.country: _country.text
        address.streetAddress1: _streetAddress1.text
        address.streetAddress2: _streetAddress2.text
        address.zipCode: parseInt(_zipCode.text, 10)
        address.state: _state.text
    }

    onNewContactChanged: {
        firstNameField.text = newContact.firstName
        middleNameField.text = newContact.middleName
        lastNameField.text = newContact.lastName
        _jobTitle.text = newContact.job.title
//        newContact.job.officeAddress.country: _jcountry.text
        _jstreetAddress1.text = newContact.job.officeAddress.streetAddress1
        _jstreetAddress2.text = newContact.job.officeAddress.streetAddress2
        _jzipCode.text = newContact.job.officeAddress.zipCode.toString()
        _jstate.text = newContact.job.officeAddress.state

//        newContact.address.country: _country.text
        _streetAddress1.text = newContact.address.streetAddress1
        _streetAddress2.text = newContact.address.streetAddress2
        _zipCode.text = newContact.address.zipCode.toString()
        _state.text = newContact.address.state
        var phoneNumber = newContact.phones.length > 0 ?
                    newContact.phones[0] : null
        if (phoneNumber) {
            homePhone.number = phoneNumber.number.toString()
        }
    }

    FloatingRoundButton {
        id: addContactButton
        enabled: firstNameField.text.length > 0
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        icon: "qrc:/images/check.png"
        onClicked: {
            var phones = new Array;
            if (homePhone.number.length !== 0) {
                phones.push(_homePhoneData);
            }
            newContact.phones = phones;

            AddressBookEngine.addContact(newContact)
            stack.pop()
        }
    }
}
