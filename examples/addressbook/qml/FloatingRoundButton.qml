import QtQuick 2.0

Rectangle {
    id: root
    signal clicked()
    property alias icon: iconItem.source
    property color primaryColor: "#2196F3"
    property color secondaryColor: "#03A9F4"
    width: 70
    height: width
    radius: width/2
    opacity: enabled ? 1.0 : 0.3
    color: control.pressed ? root.secondaryColor : root.primaryColor
    Behavior on opacity {
        NumberAnimation { duration: 300 }
    }

    Image {
        anchors.centerIn: parent
        id: iconItem
    }

    MouseArea {
        id: control
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
