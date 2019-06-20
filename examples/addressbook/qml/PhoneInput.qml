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
import QtQuick.Controls 2.4

InputRow {
    property alias number: _inputItem.text
    property int countryCode: parseInt(_ccModel.get(_countryCode.currentIndex).code, 10)
    RowLayout {
        anchors.fill: parent
        ComboBox {
            id: _countryCode
            Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
            textRole: "name"
            model: CountryCodesModel {
                id: _ccModel
            }
            font.pointSize: 12
            font.weight: Font.Bold
            contentItem: Item {
                clip: true
                implicitHeight: comboBoxItem.implicitHeight + 20
                implicitWidth: 130
                SecondaryText {
                    id: comboBoxItem
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.right: parent.right
                    elide: Text.ElideMiddle
                    text: _countryCode.currentText
                }
            }
            background: Rectangle {
                anchors.fill: parent
                color: _countryCode.pressed ? "#55ffffff" : "#00ffffff"
            }

            delegate: ItemDelegate {
                font.pointSize: 12
                text: model.name
            }
        }

        TextInput {
            id: _inputItem
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            font.pointSize: 12
            color: "#ffffff"
            validator: RegExpValidator {
                regExp:/\d*/
            }
            Rectangle {
                anchors.top: _inputItem.bottom
                width: _inputItem.width
                color: "#cfdfe7"
                height: 1
            }
        }
    }
}
