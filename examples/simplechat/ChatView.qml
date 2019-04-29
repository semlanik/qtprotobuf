import QtQuick 2.4
import examples.simplechat 1.0

Rectangle {
    anchors.fill: parent
    color: "#303030"
    ListView {
        anchors.top: parent.top
        anchors.bottom: _inputField.top
        model: scEngine.messages
        delegate: Item {
            height: childrenRect.height
            width: _inputField.width
            Text {
                id: _userName
                font.pointSize: 12
                font.weight: Font.Bold
                color: "#ffffff"
                text: model.modelData.from + ": "
            }

            Text {
                anchors.left: _userName.right
                font.pointSize: 12
                color: "#ffffff"
                text: scEngine.getText(model.modelData.content)
            }
        }
        onCountChanged: {
            positionViewAtEnd()
        }
    }

    ChatInputField {
        id: _inputField
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        placeholderText: qsTr("Start type here")
        onAccepted: {
            scEngine.sendMessage(_inputField.text)
            _inputField.text = ""
        }
    }
}
