import QtQuick 2.0
import QtQuick.Controls 2.9

Item {
    property StackView stack: null
    property alias hasBack: backButton.visible
    default property alias implicitChildren: dafaultItem.children
    Item {
        id: dafaultItem
        anchors.fill: parent
    }

    FloatingRoundButton {
        id: backButton
        primaryColor: "#d31a5b"
        secondaryColor: "#E91E63"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 10
        icon: "qrc:/images/back.png"
        onClicked: stack.pop()
    }
}
