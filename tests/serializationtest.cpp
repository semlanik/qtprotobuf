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

#include "serializationtest.h"

#include "simpleintmessage.h"

using namespace qtprotobuf::tests;

SerializationTest::SerializationTest()
{
}

TEST_F(SerializationTest, IntMessageSrializeTest)
{
    SimpleIntMessage test;
    test.setTestFieldInt(15);
    QByteArray result = test.serialize();
    qDebug() << "Out result:" << result.toHex();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), 0x0F);

    test.setTestFieldInt(300);
    result = test.serialize();
    qDebug() << "Out result:" << result.toHex();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xAC');
    ASSERT_EQ(result.at(2), 0x02);

    test.setTestFieldInt(65545);
    result = test.serialize();
    qDebug() << "Out result:" << result.toHex();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x89');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x04');

    test.setTestFieldInt(0);
    result = test.serialize();
    qDebug() << "Out result:" << result.toHex();

    test.setTestFieldInt(INT8_MAX + 1);
    result = test.serialize();
    qDebug() << "Out result:" << result.toHex();

    test.setTestFieldInt(INT16_MAX + 1);
    result = test.serialize();
    qDebug() << "Out result:" << result.toHex();

    test.setTestFieldInt(INT8_MAX);
    result = test.serialize();
    qDebug() << "Out result:" << result.toHex();

    test.setTestFieldInt(INT16_MAX);
    result = test.serialize();
    qDebug() << "Out result:" << result.toHex();

    test.setTestFieldInt(INT32_MAX);
    result = test.serialize();
    qDebug() << "Out result:" << result.toHex();

    test.setTestFieldInt(INT32_MIN);
    result = test.serialize();
    qDebug() << "Out result:" << result.toHex();
}

