import QtQuick 2.0
import QtQuick.Controls 2.4

StackItem {
    id: root
    Flickable {
        anchors.fill: parent
        Column {
            width: root.width
            TextInputRow {
                id: firstName
                width: root.width
                label: qsTr("First name")
            }
            TextInputRow {
                width: root.width
                label: qsTr("Middle name")
            }
            TextInputRow {
                width: root.width
                label: qsTr("Last name")
            }
        }
    }

    FloatingRoundButton {
        id: addContactButton
        enabled: firstName.text.length > 0
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        icon: "qrc:/images/check.png"
    }
}
