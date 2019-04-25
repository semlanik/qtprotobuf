import QtQuick 2.9
import QtQuick.Layouts 1.2

InputRow {
    property alias text: _inputItem.text
    property alias input: _inputItem
    TextInput {
        id: _inputItem
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        font.pointSize: 12
        color: "#ffffff"
        Rectangle {
            anchors.top: _inputItem.bottom
            width: _inputItem.width
            color: "#cfdfe7"
            height: 1
        }
    }
}
