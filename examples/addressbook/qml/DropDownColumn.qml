import QtQuick 2.9
import QtQuick.Layouts 1.2

Column {
    id: root
    property alias header: _headerText.text
    property alias value: _valueText.text
    default property alias content: _content.data
    anchors.left: parent.left
    anchors.right: parent.right
    state: "closed"
    states: [
        State {
            name: "opened"
            PropertyChanges {
                target: _content
                visible: true
            }
            PropertyChanges {
                target: _ddArrow
                rotation: 0
            }
        },
        State {
            name: "closed"
            PropertyChanges {
                target: _content
                visible: false
            }
            PropertyChanges {
                target: _ddArrow
                rotation: 180
            }
        }
    ]

    MouseArea {
        id: _header
        height: 70
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        onClicked: {
            root.state = root.state == "closed" ? "opened" : "closed"
        }
        RowLayout {
            anchors.fill: parent
            PrimaryText {
                id: _headerText
                Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
            }

            PrimaryText {
                id: _valueText
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                Layout.rightMargin: 20
            }

            Image {
                id: _ddArrow
                source: "qrc:/images/arrow.png"
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                Behavior on rotation {
                    NumberAnimation { duration: 200 }
                }
            }
        }
        Rectangle {
            visible: false
            anchors.fill: parent
            color: "#66ff0000"
        }
    }

    Column {
        id: _content
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 30
    }
}
