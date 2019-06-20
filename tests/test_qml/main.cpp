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

#include <QtQuickTest/quicktest.h>

#include "simpleboolmessage.h"
#include "simplebytesmessage.h"
#include "simpledoublemessage.h"
#include "simplefloatmessage.h"
#include "simplefixedint32message.h"
#include "simplefixedint64message.h"
#include "simplesfixedint32message.h"
#include "simplesfixedint64message.h"
#include "simpleintmessage.h"
#include "simpleint64message.h"
#include "simplesintmessage.h"
#include "simplesint64message.h"
#include "simpleuintmessage.h"
#include "simpleuint64message.h"
#include "simplestringmessage.h"

using namespace qtprotobufnamespace::tests;

class TestSetup : public QObject {
public:
    TestSetup() {
        SimpleBoolMessage();
        SimpleBytesMessage();
        SimpleDoubleMessage();
        SimpleFloatMessage();
        SimpleFixedInt32Message();
        SimpleIntMessage();
        SimpleSIntMessage();
        SimpleUIntMessage();
        SimpleStringMessage();
        SimpleSFixedInt32Message();
    }
    ~TestSetup() = default;
};

QUICK_TEST_MAIN_WITH_SETUP(qtprotobuf_qml_test, TestSetup)
