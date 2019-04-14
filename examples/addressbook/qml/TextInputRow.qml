import QtQuick 2.9
import QtQuick.Layouts 1.2

RowLayout {
    property alias label: labelItem.text
    property alias text: inputItem.text
    height: 70
    spacing: 10
    PrimaryText {
        id: labelItem
        Layout.alignment: Qt.AlignVCenter
        Layout.minimumWidth: 100
        Layout.maximumWidth: 100
        Layout.margins: 10
    }
    TextInput {
        id: inputItem
        Layout.fillWidth: true
        Layout.margins: 10
        Layout.alignment: Qt.AlignVCenter
        font.pointSize: 12
        color: "#ffffff"
        Rectangle {
            anchors.top: inputItem.bottom
            width: inputItem.width
            color: "#cfdfe7"
            height: 1
        }
    }
}
