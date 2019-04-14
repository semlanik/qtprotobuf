import QtQuick 2.0

Rectangle {
    id: root
    width: 70
    property color primaryColor: "#03A9F4"
    property color secondaryColor: "#2196F3"
    height: width
    radius: width/2
    signal clicked()
    property alias icon: iconItem.source
    color: control.pressed ? root.secondaryColor : root.primaryColor
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
