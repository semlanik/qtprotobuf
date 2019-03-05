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

