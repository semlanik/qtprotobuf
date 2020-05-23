import QtQuick 2.12
import QtQuick.Window 2.12

import QtQuick.Controls 2.12

import qtprotobuf.tutorial 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("QtProtobuf Tutorial")

    EchoRequest {
        id: request
        message: messageInput.text
    }

    Column {
        spacing: 20
        Row {
            spacing: 20
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "Enter request message:"
            }

            TextField {
                id: messageInput
                anchors.verticalCenter: parent.verticalCenter
                width: 400
                onAccepted: {
                    echoEngine.request(request);
                    text = ""
                }
            }
        }
        Row {
            spacing: 20
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "Server respond:"
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: echoEngine.response.message
            }
        }
    }
}
