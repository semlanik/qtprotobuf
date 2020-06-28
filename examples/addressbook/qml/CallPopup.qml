/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
 *
 * This file is part of QtProtobuf project https://git.semlanik.org/semlanik/qtprotobuf
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

import QtQuick 2.9
import QtQuick.Controls 2.4

import qtprotobuf.examples 1.0
import examples.addressbook 1.0

Rectangle {
    id: root
    property CallStatus callStatus: null
    color: "#B0BEC5"
    border.width: 1
    border.color: "#cfdfe7"
    opacity: 0.0
    visible: opacity > 0
    PrimaryText {
        id: _statusText
        text: _d.getCallStatusText()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: _hangButton.top
        anchors.bottomMargin: 20
    }
    radius: 10

    Connections {
        target: root.callStatus
        onStatusChanged: {
            console.log("root.callStatus call status: " + root.callStatus.status);
        }
    }

    transform: Rotation {
        id: _rotation
        axis {
            x: 1
            y: 0
            z: 0
        }
        origin {
            x: root.width / 2
            y: root.height / 2
        }

        angle: 0
    }

    states: [
        State {
            name: "opened"
            when: root.callStatus.status === CallStatus.Active || root.callStatus.status === CallStatus.Ended
            PropertyChanges {
                target: root
                opacity: 1.0
                anchors.verticalCenterOffset: 0
            }
            PropertyChanges {
                target: _rotation
                angle: 0
            }
        },
        State {
            name: "closed"
            when: root.callStatus.status !== CallStatus.Active && root.callStatus.status !== CallStatus.Ended
            PropertyChanges {
                target: root
                opacity: 0.0
                anchors.verticalCenterOffset: 50
            }
            PropertyChanges {
                target: _rotation
                angle: -45
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"

            ParallelAnimation {
                alwaysRunToEnd: true
                NumberAnimation {
                    target: root
                    properties: "opacity,anchors.verticalCenterOffset"
                    duration: 300
                }

                NumberAnimation {
                    target: _rotation
                    properties: "angle"
                    duration: 300
                }
            }
        }
    ]

    MouseArea {
        anchors.fill: parent
    }

    FloatingRoundButton {
        id: _hangButton
        primaryColor: "#d31a5b"
        secondaryColor: "#E91E63"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 10
        icon: "qrc:/images/drop.png"
        onClicked: {
            AddressBookEngine.endCall()
        }
    }

    QtObject {
        id: _d
        function getCallStatusText() {
            switch (callStatus.status) {
            case CallStatus.Active:
                return qsTr("Active call to %1...").arg("+" + callStatus.phoneNumber.countryCode + " " + callStatus.phoneNumber.number)
            case CallStatus.Ended:
                return qsTr("Ending call...")
            default:
                return qsTr("No active call")
            }
        }
    }
}
