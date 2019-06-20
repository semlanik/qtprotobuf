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
import QtGraphicalEffects 1.0

import examples.simplechat 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("QtProtobuf Simple Chat Example")
    Rectangle {
        id: background
        anchors.fill: parent
        color: "#303030"
    }

    Column {
        id: _loginControl
        spacing: 5
        visible: true
        anchors.centerIn: parent
        ChatInputField {
            id: _loginField
            width: 200
            placeholderText: qsTr("Login")
            onAccepted:             {
                scEngine.login(_loginField.text, _passwordField.text)
            }
            onVisibleChanged: {
                if (visible) {
                    _loginField.forceActiveFocus()
                }
            }
            Component.onCompleted: {
                if (visible) {
                    _loginField.forceActiveFocus()
                }
            }
        }
        ChatInputField {
            id: _passwordField
            echoMode: TextInput.Password
            placeholderText: qsTr("Password")
            onAccepted:             {
                scEngine.login(_loginField.text, _passwordField.text)
            }
        }
        Button {
            id: _pressedControl
            anchors.horizontalCenter: parent.horizontalCenter
            width: 40
            height: 40
            background: Rectangle {
                radius: _pressedControl.width / 2
                border {
                    width: 2
                    color: _pressedControl.pressed ? "#E91E63" : "#ffffff"
                }
                color:"#00000000"
                rotation: 90
                Image {
                    id: _icon
                    anchors.centerIn: parent
                    source: "qrc:/img/arrow.png"
                    width: sourceSize.width
                    height: sourceSize.width
                    smooth: true
                    visible: false
                }
                ColorOverlay {
                    anchors.fill: _icon
                    source: _icon
                    color: _pressedControl.pressed ? "#E91E63" : "#ffffff"
                }
            }

            onClicked: {
                scEngine.login(_loginField.text, _passwordField.text)
            }
        }
    }

    ChatView {
        id: _chatView
        visible: false
        Connections {
            target: scEngine
            onLoggedIn: {
                _chatView.visible = true
                _loginControl.visible = false
            }
        }
    }
}
