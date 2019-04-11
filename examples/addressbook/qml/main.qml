/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
 *
 * This file is part of qtprotobuf project https://git.semlanik.org/semlanik/qtprotobuf
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
import QtQuick.Controls 2.9
import QtQuick.Controls.Material 2.9
import qtprotobuf.examples.addressbook 1.0
import examples.addressbook 1.0

ApplicationWindow {
    Material.theme: Material.Light
    Material.accent: Material.Blue
    visible: true
    width: 640
    height: 480
    title: qsTr("QtProtobuf Address Book Example")
    Rectangle {
        anchors.fill: parent
        color: "#81D4FA"
    }

    TestNesting {
        id: test
        nested: NestedObject {
            title: "qqq"
        }
        Component.onCompleted: {
            console.log("test.nested " + test.nested.title)
        }
    }

//    NestedObject {
//        id: nested
//        title: "qqq"
//        Component.onCompleted: {
//            console.log("test.nested " + nested.title)
//        }
//    }

    ContactList {
        model: abEngine.contacts
        Component.onCompleted: {
            console.log("abEngine.contacts.list.length " + abEngine.contacts.count)
        }
    }
}



