import QtQuick 2.9
import QtQuick.Controls 2.4
TextField {
    id: _inputField
    width: 200
    color: "#FFFFFF"
    font.pointSize: 14
    background: Rectangle {
        radius: 20
        border {
            width: 2
            color: _inputField.focus ? "#E91E63" : "#FFFFFF"
        }
        color: "#424242"
    }
}
