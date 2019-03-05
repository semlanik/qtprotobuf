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
#include "simplefloatmessage.h"
#include "simpledoublemessage.h"

using namespace qtprotobuf::tests;

SerializationTest::SerializationTest()
{
}

TEST_F(SerializationTest, IntMessageSrializeTest)
{
    SimpleIntMessage test;
    test.setTestFieldInt(15);
    QByteArray result = test.serialize();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x1e');

    test.setTestFieldInt(300);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xd8');
    ASSERT_EQ(result.at(2), '\x04');

    test.setTestFieldInt(65545);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x92');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x08');

    test.setTestFieldInt(0);
    result = test.serialize();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x00');

    test.setTestFieldInt(INT8_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x02');

    test.setTestFieldInt(INT16_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x04');

    test.setTestFieldInt(INT8_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xfe');
    ASSERT_EQ(result.at(2), '\x01');

    test.setTestFieldInt(INT16_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xfe');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\x03');

    test.setTestFieldInt(INT32_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 6);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xfe');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\x0f');

    test.setTestFieldInt(-1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x01');

    test.setTestFieldInt(-462);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x9b');
    ASSERT_EQ(result.at(2), '\x07');

    test.setTestFieldInt(-63585);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xc1');
    ASSERT_EQ(result.at(2), '\xe1');
    ASSERT_EQ(result.at(3), '\x07');


    test.setTestFieldInt(INT8_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\x01');

    test.setTestFieldInt(INT16_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\x03');

    test.setTestFieldInt(INT32_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), 6);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\x0f');

    test.setTestFieldInt(INT8_MIN - 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x81');
    ASSERT_EQ(result.at(2), '\x02');

    test.setTestFieldInt(INT16_MIN - 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x81');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x04');
}

TEST_F(SerializationTest, FloatMessageSrializeTest)
{
    constexpr int FloatMessageSize = 5;
    SimpleFloatMessage test;
    test.setTestFieldFloat(0.1f);
    QByteArray result = test.serialize();
    ASSERT_EQ(result.size(), FloatMessageSize);
    ASSERT_EQ(result.at(0), 0x3d);
    ASSERT_EQ(result.at(1), '\xcd');
    ASSERT_EQ(result.at(2), '\xcc');
    ASSERT_EQ(result.at(3), '\xcc');
    ASSERT_EQ(result.at(4), '\x3d');

    test.setTestFieldFloat(FLT_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), FloatMessageSize);
    ASSERT_EQ(result.at(0), 0x3d);
    ASSERT_EQ(result.at(1), '\x00');
    ASSERT_EQ(result.at(2), '\x00');
    ASSERT_EQ(result.at(3), '\x80');
    ASSERT_EQ(result.at(4), '\x00');

    test.setTestFieldFloat(FLT_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), FloatMessageSize);
    ASSERT_EQ(result.at(0), 0x3d);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\x7f');
    ASSERT_EQ(result.at(4), '\x7f');

    test.setTestFieldFloat(-4.2f);
    result = test.serialize();
    ASSERT_EQ(result.size(), FloatMessageSize);
    ASSERT_EQ(result.at(0), 0x3d);
    ASSERT_EQ(result.at(1), '\x66');
    ASSERT_EQ(result.at(2), '\x66');
    ASSERT_EQ(result.at(3), '\x86');
    ASSERT_EQ(result.at(4), '\xc0');

    test.setTestFieldFloat(-0.0f);
    result = test.serialize();
    ASSERT_EQ(result.size(), FloatMessageSize);
    ASSERT_EQ(result.at(0), 0x3d);
    ASSERT_EQ(result.at(1), '\x00');
    ASSERT_EQ(result.at(2), '\x00');
    ASSERT_EQ(result.at(3), '\x00');
    ASSERT_EQ(result.at(4), '\x80');
}

TEST_F(SerializationTest, DoubleMessageSrializeTest)
{
    constexpr int DoubleMessageSize = 9;
    SimpleDoubleMessage test;
    test.setTestFieldDouble(0.1);
    QByteArray result = test.serialize();
    ASSERT_EQ(result.size(), DoubleMessageSize);
    ASSERT_EQ(result.at(0), 0x41);
    ASSERT_EQ(result.at(1), '\x9a');
    ASSERT_EQ(result.at(2), '\x99');
    ASSERT_EQ(result.at(3), '\x99');
    ASSERT_EQ(result.at(4), '\x99');
    ASSERT_EQ(result.at(5), '\x99');
    ASSERT_EQ(result.at(6), '\x99');
    ASSERT_EQ(result.at(7), '\xb9');
    ASSERT_EQ(result.at(8), '\x3f');

    test.setTestFieldDouble(DBL_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), DoubleMessageSize);
    ASSERT_EQ(result.at(0), 0x41);
    ASSERT_EQ(result.at(1), '\x00');
    ASSERT_EQ(result.at(2), '\x00');
    ASSERT_EQ(result.at(3), '\x00');
    ASSERT_EQ(result.at(4), '\x00');
    ASSERT_EQ(result.at(5), '\x00');
    ASSERT_EQ(result.at(6), '\x00');
    ASSERT_EQ(result.at(7), '\x10');
    ASSERT_EQ(result.at(8), '\x00');

    test.setTestFieldDouble(DBL_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), DoubleMessageSize);
    ASSERT_EQ(result.at(0), 0x41);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xef');
    ASSERT_EQ(result.at(8), '\x7f');

    test.setTestFieldDouble(-4.2);
    result = test.serialize();
    ASSERT_EQ(result.size(), DoubleMessageSize);
    ASSERT_EQ(result.at(0), 0x41);
    ASSERT_EQ(result.at(1), '\xcd');
    ASSERT_EQ(result.at(2), '\xcc');
    ASSERT_EQ(result.at(3), '\xcc');
    ASSERT_EQ(result.at(4), '\xcc');
    ASSERT_EQ(result.at(5), '\xcc');
    ASSERT_EQ(result.at(6), '\xcc');
    ASSERT_EQ(result.at(7), '\x10');
    ASSERT_EQ(result.at(8), '\xc0');

    test.setTestFieldDouble(0.0);
    result = test.serialize();
    ASSERT_EQ(result.size(), DoubleMessageSize);
    ASSERT_EQ(result.at(0), 0x41);
    ASSERT_EQ(result.at(1), '\x00');
    ASSERT_EQ(result.at(2), '\x00');
    ASSERT_EQ(result.at(3), '\x00');
    ASSERT_EQ(result.at(4), '\x00');
    ASSERT_EQ(result.at(5), '\x00');
    ASSERT_EQ(result.at(6), '\x00');
    ASSERT_EQ(result.at(7), '\x00');
    ASSERT_EQ(result.at(8), '\x00');
}
