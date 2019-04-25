import QtQuick 2.9
import QtQuick.Layouts 1.2

RowLayout {
    property alias label: labelItem.text
    default property alias content: _content.data
    anchors.left: parent.left
    anchors.right: parent.right
    height: 70
    spacing: 10
    PrimaryText {
        id: labelItem
        Layout.alignment: Qt.AlignBottom
        Layout.minimumWidth: 100
        Layout.maximumWidth: 100
        Layout.margins: 10
    }

    Item {
        id: _content
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.margins: 10
        Layout.alignment: Qt.AlignBottom
    }
}
