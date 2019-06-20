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
