import QtQuick 2.4
import examples.simplechat 1.0
import qtprotobuf.examples 1.0

Rectangle {
    id: root
    anchors.fill: parent
    color: "#303030"
    onVisibleChanged: {
        if (visible) {
            _inputField.forceActiveFocus()
        }
    }
    MouseArea {
        anchors.fill: parent
    }

    ListView {
        anchors.top: parent.top
        anchors.bottom: _inputField.top
        anchors.left: parent.left
        anchors.right: parent.right
        model: SimpleChatEngine.messages
        delegate: Item {
            height: _imageWrapper.height + 10
            width: root.width
            Item {
                id: _imageWrapper
                height: _messageColumn.height + 20
                width: parent.width/2 - 20
                property bool ownMessage: SimpleChatEngine.userName === model.modelData.from
                anchors{
                    right: _imageWrapper.ownMessage ? parent.right : undefined
                    left: _imageWrapper.ownMessage ? undefined : parent.left
                    rightMargin: _imageWrapper.ownMessage ? 10 : 0
                    leftMargin: _imageWrapper.ownMessage ? 0 : 10
                    verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    anchors.fill: parent
                    radius: 20
                    color: "#424242"
                    border.color: _imageWrapper.ownMessage ? "#E91E63" : "#F48FB1"
                    border.width: 1
                }

                Column {
                    id: _messageColumn
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: 10
                        rightMargin: 10
                        verticalCenter: parent.verticalCenter
                    }
                    Text {
                        id: _userName
                        property string from: _imageWrapper.ownMessage ? qsTr("You") : model.modelData.from
                        anchors.left: parent.left
                        anchors.right: parent.right
                        font.pointSize: 12
                        font.weight: Font.Bold
                        color: "#ffffff"
                        text: _userName.from + ": "
                    }

                    Loader {
                        id: delegateLoader
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: item ? item.height : 0
                        sourceComponent:  model.modelData.type === ChatMessage.Image ? imageDelegate : textDelegate
                        onItemChanged:  {
                            if (item) {
                                item.modelData = model.modelData
                            }
                        }
                    }
                }
            }
        }
        onCountChanged: {
            positionViewAtEnd()
        }
    }

    Component {
        id: textDelegate
        Item {
            property var modelData: null
            height: childrenRect.height
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                height: implicitHeight
                font.pointSize: 12
                color: "#ffffff"
                wrapMode: Text.Wrap
                text: SimpleChatEngine.getText(modelData.content)
            }
        }
    }
    Component {
        id: imageDelegate
        Item {
            property var modelData: null
            height: childrenRect.height
            Image {
                width: implicitWidth
                height: implicitHeight
                source: SimpleChatEngine.getImageThumbnail(modelData.content)
            }
        }
    }

    ChatInputField {
        id: _inputField
        focus: true
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 20
        }

        placeholderText: qsTr("Start type here or paste image")
        onAccepted: {
            SimpleChatEngine.sendMessage(_inputField.text)
            _inputField.text = ""
        }

        Keys.onPressed: {
            if (event.key === Qt.Key_V && event.modifiers & Qt.ControlModifier) {
                console.log("Ctrl + V")
                switch (SimpleChatEngine.clipBoardContentType) {
                case ChatMessage.Text:
                    paste()
                    break
                case ChatMessage.Image:
                    SimpleChatEngine.sendImageFromClipboard()
                    break
                }

                event.accepted = true
            }
        }
    }
}
