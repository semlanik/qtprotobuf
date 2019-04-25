import QtQuick 2.9
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3

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
