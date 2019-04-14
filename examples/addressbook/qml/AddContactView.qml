import QtQuick 2.0
import QtQuick.Controls 2.4

import qtprotobuf.examples.addressbook 1.0

StackItem {
    id: root
    Flickable {
        anchors.fill: parent
        Column {
            width: root.width
            TextInputRow {
                id: firstNameField
                width: root.width
                label: qsTr("First name")
            }
            TextInputRow {
                id: middleNameField
                width: root.width
                label: qsTr("Middle name")
            }
            TextInputRow {
                id: lastNameField
                width: root.width
                label: qsTr("Last name")
            }
        }
    }

    property Contact newContact: Contact {
        firstName: firstNameField.text
        middleName: middleNameField.text
        lastName: lastNameField.text
    }

    FloatingRoundButton {
        id: addContactButton
        enabled: firstNameField.text.length > 0
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        icon: "qrc:/images/check.png"
        onClicked: {
            abEngine.addContact(newContact)
            stack.pop()
        }
    }
}
