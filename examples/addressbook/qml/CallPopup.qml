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
            when: callStatus.status === CallStatus.Active || callStatus.status === CallStatus.Ended
        },
        State {
            name: "closed"
            when: callStatus.status !== CallStatus.Active && callStatus.status !== CallStatus.Ended
        }
    ]

    transitions: [
        Transition {
            from: "opened"
            to: "closed"

            NumberAnimation {
                target: root
                property: "opacity"
                duration: 300
                from: 1.0
                to: 0.0
            }


            NumberAnimation {
                target: root
                property: "anchors.verticalCenterOffset"
                duration: 300
                from: 0
                to: 50
            }

            NumberAnimation {
                target: _rotation
                property: "angle"
                duration: 300
                from: 0
                to: -45
            }
        },
        Transition {
            from: "closed"
            to: "opened"

            NumberAnimation {
                target: root
                property: "anchors.verticalCenterOffset"
                duration: 300
                from: 50
                to: 0
            }

            NumberAnimation {
                target: root
                property: "opacity"
                duration: 300
                from: 0.0
                to: 1.0
            }

            NumberAnimation {
                target: _rotation
                property: "angle"
                duration: 300
                from: -45
                to: 0
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
            abEngine.endCall()
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
