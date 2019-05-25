/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Viktor Kopp <vifactor@gmail.com>
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
#include "simpleuintmessage.h"
#include "simplesintmessage.h"
#include "simpleint64message.h"
#include "simpleuint64message.h"
#include "simplesint64message.h"
#include "simplefixedint32message.h"
#include "simplefixedint64message.h"
#include "simplesfixedint32message.h"
#include "simplesfixedint64message.h"
#include "simplefloatmessage.h"
#include "simpledoublemessage.h"
#include "simplestringmessage.h"
#include "complexmessage.h"
#include "repeatedintmessage.h"
#include "repeatedsintmessage.h"
#include "repeateduintmessage.h"
#include "repeatedint64message.h"
#include "repeatedsint64message.h"
#include "repeateduint64message.h"
#include "repeatedfixedintmessage.h"
#include "repeatedsfixedintmessage.h"
#include "repeatedfixedint64message.h"
#include "repeatedsfixedint64message.h"
#include "repeatedstringmessage.h"
#include "repeateddoublemessage.h"
#include "repeatedbytesmessage.h"
#include "repeatedfloatmessage.h"
#include "repeatedcomplexmessage.h"
#include "simpleboolmessage.h"
#include "simpleenummessage.h"

#include "simplefixed32stringmapmessage.h"
#include "simplesfixed32stringmapmessage.h"
#include "simpleint32stringmapmessage.h"
#include "simplesint32stringmapmessage.h"
#include "simpleuint32stringmapmessage.h"

#include "simplefixed64stringmapmessage.h"
#include "simplesfixed64stringmapmessage.h"
#include "simpleint64stringmapmessage.h"
#include "simplesint64stringmapmessage.h"
#include "simpleuint64stringmapmessage.h"

#include "simplestringstringmapmessage.h"

#include "fieldindextest1message.h"
#include "fieldindextest2message.h"
#include "fieldindextest3message.h"
#include "fieldindextest4message.h"

using namespace qtprotobufnamespace::tests;
using namespace qtprotobuf::tests;
using namespace qtprotobuf;

SerializationTest::SerializationTest()
{
}

TEST_F(SerializationTest, IntMessageSerializeTest)
{
    SimpleIntMessage test;
    test.setTestFieldInt(15);
    QByteArray result = test.serialize();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x0f');

    test.setTestFieldInt(300);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xac');
    ASSERT_EQ(result.at(2), '\x02');

    test.setTestFieldInt(65545);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x89');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x04');

    test.setTestFieldInt(0);
    result = test.serialize();
    ASSERT_EQ(result.size(), 0);

    test.setTestFieldInt(INT8_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x01');

    test.setTestFieldInt(INT16_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x02');

    test.setTestFieldInt(INT8_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x7f');

    test.setTestFieldInt(INT16_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\x01');

    test.setTestFieldInt(INT32_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 6);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\x07');

    test.setTestFieldInt(-1);
    result = test.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(), "08ffffffff0f");

    test.setTestFieldInt(-462);
    result = test.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(), "08b2fcffff0f");

    test.setTestFieldInt(-63585);
    result = test.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(), "089f8ffcff0f");

    test.setTestFieldInt(INT8_MIN);
    result = test.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(), "0880ffffff0f");

    test.setTestFieldInt(INT16_MIN);
    result = test.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(), "088080feff0f");

    test.setTestFieldInt(INT32_MIN);
    result = test.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(), "088080808008");

    test.setTestFieldInt(INT8_MIN - 1);
    result = test.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(), "08fffeffff0f");

    test.setTestFieldInt(INT16_MIN - 1);
    result = test.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(), "08fffffdff0f");
}

TEST_F(SerializationTest, UIntMessageSerializeTest)
{
    SimpleUIntMessage test;
    test.setTestFieldInt(15);
    QByteArray result = test.serialize();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x0f');

    test.setTestFieldInt(300);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xac');
    ASSERT_EQ(result.at(2), '\x02');

    test.setTestFieldInt(65545);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x89');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x04');

    test.setTestFieldInt(0);
    result = test.serialize();
    ASSERT_EQ(result.size(), 0);

    test.setTestFieldInt(UINT8_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x02');

    test.setTestFieldInt(UINT16_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x04');

    test.setTestFieldInt(UINT8_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\x01');

    test.setTestFieldInt(UINT16_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\x03');

    test.setTestFieldInt(UINT32_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 6);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\x0f');
}

TEST_F(SerializationTest, SIntMessageSerializeTest)
{
    SimpleSIntMessage test;
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
    ASSERT_EQ(result.size(), 0);

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

TEST_F(SerializationTest, Int64MessageSerializeTest)
{
    SimpleInt64Message test;
    test.setTestFieldInt(15);
    QByteArray result = test.serialize();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x0f');

    test.setTestFieldInt(300);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xac');
    ASSERT_EQ(result.at(2), '\x02');

    test.setTestFieldInt(65545);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x89');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x04');

    test.setTestFieldInt(0);
    result = test.serialize();
    ASSERT_EQ(result.size(), 0);

    test.setTestFieldInt(INT8_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x01');

    test.setTestFieldInt(INT16_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x02');

    test.setTestFieldInt((qlonglong)(INT32_MAX) + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 6);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x80');
    ASSERT_EQ(result.at(4), '\x80');
    ASSERT_EQ(result.at(5), '\x08');

    test.setTestFieldInt(INT8_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x7f');

    test.setTestFieldInt(INT16_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\x01');

    test.setTestFieldInt(INT32_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 6);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\x07');

    test.setTestFieldInt(INT64_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 10);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\x7f');

    test.setTestFieldInt(-1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');

    test.setTestFieldInt(-462);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xb2');
    ASSERT_EQ(result.at(2), '\xfc');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');

    test.setTestFieldInt(-63585);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x9f');
    ASSERT_EQ(result.at(2), '\x8f');
    ASSERT_EQ(result.at(3), '\xfc');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');

    test.setTestFieldInt(INT8_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');

    test.setTestFieldInt(INT16_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\xfe');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');

    test.setTestFieldInt(INT32_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x80');
    ASSERT_EQ(result.at(4), '\x80');
    ASSERT_EQ(result.at(5), '\xf8');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');

    test.setTestFieldInt(INT8_MIN - 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xfe');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');

    test.setTestFieldInt(INT16_MIN - 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xfd');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');

    test.setTestFieldInt((qlonglong)INT32_MIN - 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xf7');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');

    test.setTestFieldInt(INT64_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x80');
    ASSERT_EQ(result.at(4), '\x80');
    ASSERT_EQ(result.at(5), '\x80');
    ASSERT_EQ(result.at(6), '\x80');
    ASSERT_EQ(result.at(7), '\x80');
    ASSERT_EQ(result.at(8), '\x80');
    ASSERT_EQ(result.at(9), '\x80');
    ASSERT_EQ(result.at(10), '\x01');
}

TEST_F(SerializationTest, UInt64MessageSerializeTest)
{
    SimpleUInt64Message test;
    test.setTestFieldInt(15);
    QByteArray result = test.serialize();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x0f');

    test.setTestFieldInt(300);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xac');
    ASSERT_EQ(result.at(2), '\x02');

    test.setTestFieldInt(65545);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x89');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x04');

    test.setTestFieldInt(0);
    result = test.serialize();
    ASSERT_EQ(result.size(), 0);

    test.setTestFieldInt(UINT8_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x02');

    test.setTestFieldInt(UINT16_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x04');

    test.setTestFieldInt(((uint64_t)UINT32_MAX) + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 6);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x80');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x80');
    ASSERT_EQ(result.at(4), '\x80');
    ASSERT_EQ(result.at(5), '\x10');

    test.setTestFieldInt(UINT8_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\x01');

    test.setTestFieldInt(UINT16_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\x03');

    test.setTestFieldInt(UINT32_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 6);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\x0f');

    test.setTestFieldInt(UINT64_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');
}

TEST_F(SerializationTest, SInt64MessageSerializeTest)
{
    SimpleSInt64Message test;
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
    ASSERT_EQ(result.size(), 0);

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

    test.setTestFieldInt(INT64_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xfe');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');

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

    test.setTestFieldInt(INT64_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), 11);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\xff');
    ASSERT_EQ(result.at(2), '\xff');
    ASSERT_EQ(result.at(3), '\xff');
    ASSERT_EQ(result.at(4), '\xff');
    ASSERT_EQ(result.at(5), '\xff');
    ASSERT_EQ(result.at(6), '\xff');
    ASSERT_EQ(result.at(7), '\xff');
    ASSERT_EQ(result.at(8), '\xff');
    ASSERT_EQ(result.at(9), '\xff');
    ASSERT_EQ(result.at(10), '\x01');

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

    test.setTestFieldInt((qlonglong)INT32_MIN - 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), 6);
    ASSERT_EQ(result.at(0), 0x08);
    ASSERT_EQ(result.at(1), '\x81');
    ASSERT_EQ(result.at(2), '\x80');
    ASSERT_EQ(result.at(3), '\x80');
    ASSERT_EQ(result.at(4), '\x80');
    ASSERT_EQ(result.at(5), '\x10');
}

TEST_F(SerializationTest, FixedInt32MessageSerializeTest)
{
    constexpr int Fixed32MessageSize = 5;
    SimpleFixedInt32Message test;
    test.setTestFieldFixedInt32(15);
    QByteArray result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d0f000000"));

    test.setTestFieldFixedInt32(300);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d2c010000"));

    test.setTestFieldFixedInt32(65545);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d09000100"));

    test.setTestFieldFixedInt32(0);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d00000000"));

    test.setTestFieldFixedInt32(UINT8_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d00010000"));

    test.setTestFieldFixedInt32(UINT16_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d00000100"));

    test.setTestFieldFixedInt32(UINT8_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0dff000000"));

    test.setTestFieldFixedInt32(UINT16_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0dffff0000"));

    test.setTestFieldFixedInt32(UINT32_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0dffffffff"));
}

TEST_F(SerializationTest, FixedInt64MessageSerializeTest)
{
    constexpr int Fixed64MessageSize = 9;
    SimpleFixedInt64Message test;
    test.setTestFieldFixedInt64(15);
    QByteArray result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090f00000000000000"));

    test.setTestFieldFixedInt64(300);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("092c01000000000000"));

    test.setTestFieldFixedInt64(65545);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090900010000000000"));

    test.setTestFieldFixedInt64(0);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090000000000000000"));

    test.setTestFieldFixedInt64(UINT8_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090001000000000000"));

    test.setTestFieldFixedInt64(UINT16_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090000010000000000"));

    test.setTestFieldFixedInt64((unsigned long long)(UINT32_MAX) + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090000000001000000"));

    test.setTestFieldFixedInt64(UINT8_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("09ff00000000000000"));

    test.setTestFieldFixedInt64(UINT16_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("09ffff000000000000"));

    test.setTestFieldFixedInt64(UINT32_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("09ffffffff00000000"));

    test.setTestFieldFixedInt64(UINT64_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("09ffffffffffffffff"));
}

TEST_F(SerializationTest, SFixedInt32MessageSerializeTest)
{
    constexpr int Fixed32MessageSize = 5;
    SimpleSFixedInt32Message test;
    test.setTestFieldFixedInt32(15);
    QByteArray result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d0f000000"));

    test.setTestFieldFixedInt32(300);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d2c010000"));

    test.setTestFieldFixedInt32(65545);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d09000100"));

    test.setTestFieldFixedInt32(0);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d00000000"));

    test.setTestFieldFixedInt32(INT8_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d80000000"));

    test.setTestFieldFixedInt32(INT16_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d00800000"));

    test.setTestFieldFixedInt32(INT8_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d7f000000"));

    test.setTestFieldFixedInt32(INT16_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0dff7f0000"));

    test.setTestFieldFixedInt32(INT8_MIN - 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d7fffffff"));

    test.setTestFieldFixedInt32(INT16_MIN - 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0dff7fffff"));

    test.setTestFieldFixedInt32(INT8_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d80ffffff"));

    test.setTestFieldFixedInt32(INT16_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d0080ffff"));

    test.setTestFieldFixedInt32(INT32_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed32MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0d00000080"));
}

TEST_F(SerializationTest, SFixedInt64MessageSerializeTest)
{
    constexpr int Fixed64MessageSize = 9;
    SimpleSFixedInt64Message test;
    test.setTestFieldFixedInt64(15);
    QByteArray result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090f00000000000000"));

    test.setTestFieldFixedInt64(300);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("092c01000000000000"));

    test.setTestFieldFixedInt64(65545);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090900010000000000"));

    test.setTestFieldFixedInt64(0);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090000000000000000"));

    test.setTestFieldFixedInt64(INT8_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("098000000000000000"));

    test.setTestFieldFixedInt64(INT16_MAX + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090080000000000000"));

    test.setTestFieldFixedInt64((unsigned long long)(INT32_MAX) + 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090000008000000000"));

    test.setTestFieldFixedInt64(INT8_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("097f00000000000000"));

    test.setTestFieldFixedInt64(INT16_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("09ff7f000000000000"));

    test.setTestFieldFixedInt64(INT32_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("09ffffff7f00000000"));

    test.setTestFieldFixedInt64(INT64_MAX);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("09ffffffffffffff7f"));

    test.setTestFieldFixedInt64(INT8_MIN - 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("097fffffffffffffff"));

    test.setTestFieldFixedInt64(INT16_MIN - 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("09ff7fffffffffffff"));

    test.setTestFieldFixedInt64((qlonglong)INT32_MIN - 1);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("09ffffff7fffffffff"));

    test.setTestFieldFixedInt64(INT8_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0980ffffffffffffff"));

    test.setTestFieldFixedInt64(INT16_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090080ffffffffffff"));

    test.setTestFieldFixedInt64(INT32_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("0900000080ffffffff"));

    test.setTestFieldFixedInt64(INT64_MIN);
    result = test.serialize();
    ASSERT_EQ(result.size(), Fixed64MessageSize);
    ASSERT_TRUE(result == QByteArray::fromHex("090000000000000080"));
}

TEST_F(SerializationTest, FloatMessageSerializeTest)
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

TEST_F(SerializationTest, DoubleMessageSerializeTest)
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

TEST_F(SerializationTest, StringMessageSerializeTest)
{
    SimpleStringMessage test;
    test.setTestFieldString("qwerty");
    QByteArray result = test.serialize();
    ASSERT_TRUE(result == QByteArray::fromHex("3206717765727479"));

    test.setTestFieldString("oepSNLIVG08UJpk2W7JtTkkBxyK06X0lQ6ML7IMd55K8XC1Tpsc1kDWym5v8z68b4FQup9O95QSgAvjHIA15OX6Bu68esbQFT9LPzSADJ6qSGBTYBHX5QSZg32trCdHMj80XuDHqyBgM4uf6RKq2mgWb8Ovxxr0NwLxjHOfhJ8Mrfd2R7hbUgjespbYoQhbgHEj2gKEV3QvnumYmrVXe1BkCzZhKVXodDhj0OfAE67viAy4i3Oag1hr1z4Azo8O5Xq68POEZ1CsZPo2DXNNR8ebVCdYOz0Q6JLPSl5jasLCFrQN7EiVNjQmCrSsZHRgLNylvgoEFxGYxXJ9gmK4mr0OGdZcGJORRGZOQCpQMhXmhezFalNIJXMPPXaRVXiRhRAPCNUEie8DtaCWAMqz4nNUxRMZ5UcXBXsXPshygzkyyXnNWTIDojFlrcsnKqSkQ1G6E85gSZbtIYBh7sqO6GDXHjOrXVaVCVCUubjcJKThlyslt29zHuIs5JGppXxX1");
    result = test.serialize();
    ASSERT_TRUE(result == QByteArray::fromHex("3280046f6570534e4c4956473038554a706b3257374a74546b6b4278794b303658306c51364d4c37494d6435354b3858433154707363316b4457796d3576387a3638623446517570394f393551536741766a48494131354f583642753638657362514654394c507a5341444a367153474254594248583551535a67333274724364484d6a383058754448717942674d34756636524b71326d675762384f76787872304e774c786a484f66684a384d726664325237686255676a65737062596f5168626748456a32674b45563351766e756d596d7256586531426b437a5a684b56586f6444686a304f6641453637766941793469334f6167316872317a34417a6f384f3558713638504f455a3143735a506f3244584e4e52386562564364594f7a3051364a4c50536c356a61734c434672514e374569564e6a516d437253735a4852674c4e796c76676f454678475978584a39676d4b346d72304f47645a63474a4f5252475a4f514370514d68586d68657a46616c4e494a584d50505861525658695268524150434e55456965384474614357414d717a346e4e5578524d5a355563584258735850736879677a6b7979586e4e575449446f6a466c7263736e4b71536b5131473645383567535a6274495942683773714f36474458486a4f72585661564356435575626a634a4b54686c79736c7432397a48754973354a47707058785831"));
}

TEST_F(SerializationTest, ComplexTypeSerializeTest)
{
    SimpleStringMessage stringMsg;
    stringMsg.setTestFieldString("qwerty");

    ComplexMessage test;
    test.setTestFieldInt(42);
    test.setTestComplexField(stringMsg);

    QByteArray result = test.serialize();
    //qDebug() << "result" << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("12083206717765727479082a")
                || result == QByteArray::fromHex("082a12083206717765727479"));

    stringMsg.setTestFieldString("YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs");
    test.setTestFieldInt(42);
    test.setTestComplexField(stringMsg);

    result = test.serialize();
    //qDebug() << "result" << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("128404328104595652664a766a78716267764677533159764f5a5867746a356666474c533741694e487a396f5a496f4b626d377a3848373978427579506b70515876476f4f30394f5939785261777833654f417339786a6f544131784a68727732385441637131436562596c43395755665143366849616e74614e647948694b546f666669305a74376c613432535278585a53503447757862635a49703533704a6e79437766437931716446637a5430646d6e3768386670794164656d456176774665646134643050417047665355326a4c74333958386b595542784e4d325767414c524267486456646538377136506935553639546a684d6432385731534644314478796f67434372714f6374325a5049436f4c6e72716446334f644e7a6a52564c6665797651384c674c76524e4652395766574179417a37396e4b6742616d64384e746c7674344d6733354535675653326737415137726b6d37326342646e5739734345794761626558417548356a34475262754c543771425a574463464c463453734364533357664647644e48667761696a7a796b42796f3731507646566c54584832574a576f4676523546414c6a42546e37624364503070416953624c435938587a324d73633364426235466639474953506255704e6d557642644d5a4d485176714f6d544e584550704e306237344d444f4d5166574a53684f6f334e6b41764d6a73082a")
                ||result == QByteArray::fromHex("082a128404328104595652664a766a78716267764677533159764f5a5867746a356666474c533741694e487a396f5a496f4b626d377a3848373978427579506b70515876476f4f30394f5939785261777833654f417339786a6f544131784a68727732385441637131436562596c43395755665143366849616e74614e647948694b546f666669305a74376c613432535278585a53503447757862635a49703533704a6e79437766437931716446637a5430646d6e3768386670794164656d456176774665646134643050417047665355326a4c74333958386b595542784e4d325767414c524267486456646538377136506935553639546a684d6432385731534644314478796f67434372714f6374325a5049436f4c6e72716446334f644e7a6a52564c6665797651384c674c76524e4652395766574179417a37396e4b6742616d64384e746c7674344d6733354535675653326737415137726b6d37326342646e5739734345794761626558417548356a34475262754c543771425a574463464c463453734364533357664647644e48667761696a7a796b42796f3731507646566c54584832574a576f4676523546414c6a42546e37624364503070416953624c435938587a324d73633364426235466639474953506255704e6d557642644d5a4d485176714f6d544e584550704e306237344d444f4d5166574a53684f6f334e6b41764d6a73"));


    stringMsg.setTestFieldString("qwerty");
    test.setTestFieldInt(-45);
    test.setTestComplexField(stringMsg);

    result = test.serialize();
    //    qDebug() << "result" << result.toHex();

    ASSERT_TRUE(result == QByteArray::fromHex("1208320671776572747908d3ffffff0f")
                || result == QByteArray::fromHex("08d3ffffff0f12083206717765727479"));
}

TEST_F(SerializationTest, RepeatedIntMessageTest)
{
    RepeatedIntMessage test;
    test.setTestRepeatedInt({1, 321, -65999, 123245, -3, 3});
    QByteArray result = test.serialize();
    //qDebug() << "RepeatedIntMessage result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a1101c102b1fcfbff0fedc207fdffffff0f03"));
    test.setTestRepeatedInt(int32List());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedSIntMessageTest)
{
    RepeatedSIntMessage test;
    test.setTestRepeatedInt({1, 321, -65999, 123245, -3, 3});
    QByteArray result = test.serialize();
    //qDebug() << "RepeatedSIntMessage result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a0b0282059d8708da850f0506"));

    test.setTestRepeatedInt(sint32List());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedUIntMessageTest)
{
    RepeatedUIntMessage test;
    test.setTestRepeatedInt({1, 321, 65999, 123245, 3});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a0a01c102cf8304edc20703"));

    test.setTestRepeatedInt(uint32List());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedInt64MessageTest)
{
    RepeatedInt64Message test;
    test.setTestRepeatedInt({1, 321, -65999, 12324523123123, -3, 3});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a1f01c102b1fcfbffffffffffff01b3c3cab6d8e602fdffffffffffffffff0103"));

    test.setTestRepeatedInt(int64List());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedSInt64MessageTest)
{
    RepeatedSInt64Message test;
    test.setTestRepeatedInt({1, 321, -65999, 12324523123123, -3, 3});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a0f0282059d8708e68695edb0cd050506"));

    test.setTestRepeatedInt(sint64List());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedUInt64MessageTest)
{
    RepeatedUInt64Message test;
    test.setTestRepeatedInt({1, 321, 65999, 123245, 123245324235425234, 3});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a1301c102cf8304edc207d28b9fda82dff6da0103"));

    test.setTestRepeatedInt(uint64List());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedFixedIntMessageTest)
{
    RepeatedFixedIntMessage test;
    test.setTestRepeatedInt({1, 321, 65999, 12324523, 3, 3});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a180100000041010000cf010100ab0ebc000300000003000000"));
    test.setTestRepeatedInt(fixed32List());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedSFixedIntMessageTest)
{
    RepeatedSFixedIntMessage test;
    test.setTestRepeatedInt({1, 321, -65999, 12324523, -3, 3});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a18010000004101000031fefeffab0ebc00fdffffff03000000"));

    test.setTestRepeatedInt(sfixed32List());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedFixedInt64MessageTest)
{
    RepeatedFixedInt64Message test;
    test.setTestRepeatedInt({1, 321, 65999, 123245324235425234, 3, 3});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a3001000000000000004101000000000000cf01010000000000d2c5472bf8dab50103000000000000000300000000000000"));

    test.setTestRepeatedInt(fixed64List());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedSFixedInt64MessageTest)
{
    RepeatedSFixedInt64Message test;
    test.setTestRepeatedInt({1, 321, -65999, 123245324235425234, -3, 3});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a300100000000000000410100000000000031fefeffffffffffd2c5472bf8dab501fdffffffffffffff0300000000000000"));

    test.setTestRepeatedInt(sfixed64List());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedStringMessageTest)
{
    RepeatedStringMessage test;
    test.setTestRepeatedString({"aaaa","bbbbb","ccc","dddddd","eeeee"});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a04616161610a0562626262620a036363630a066464646464640a056565656565"));

    test.setTestRepeatedString(QStringList());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedDoubleMessageTest)
{
    RepeatedDoubleMessage test;
    test.setTestRepeatedDouble({0.1, 0.2, 0.3, 0.4, 0.5});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a289a9999999999b93f9a9999999999c93f333333333333d33f9a9999999999d93f000000000000e03f"));

    test.setTestRepeatedDouble(DoubleList());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedBytesMessageTest)
{
    RepeatedBytesMessage test;
    test.setTestRepeatedBytes({QByteArray::fromHex("010203040506"),
                               QByteArray::fromHex("ffffffff"),
                               QByteArray::fromHex("eaeaeaeaea"),
                               QByteArray::fromHex("010203040506")});
    QByteArray result = test.serialize();
    ASSERT_TRUE(result == QByteArray::fromHex("0a060102030405060a04ffffffff0a05eaeaeaeaea0a06010203040506"));

    test.setTestRepeatedBytes(QByteArrayList());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());

    test.setTestRepeatedBytes({QByteArray::fromHex("010203040506"),
                               QByteArray::fromHex(""),
                               QByteArray::fromHex("eaeaeaeaea"),
                               QByteArray::fromHex("010203040506")});
    result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a060102030405060a000a05eaeaeaeaea0a06010203040506"));
}

TEST_F(SerializationTest, RepeatedFloatMessageTest)
{
    RepeatedFloatMessage test;
    test.setTestRepeatedFloat({0.4f, 1.2f, 0.5f, 1.4f, 0.6f});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();
    ASSERT_TRUE(result == QByteArray::fromHex("0a14cdcccc3e9a99993f0000003f3333b33f9a99193f"));

    test.setTestRepeatedFloat(FloatList());
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, RepeatedComplexMessageTest)
{
    SimpleStringMessage stringMsg;
    stringMsg.setTestFieldString("qwerty");
    QSharedPointer<ComplexMessage> msg(new ComplexMessage);
    msg->setTestFieldInt(25);
    msg->setTestComplexField(stringMsg);
    RepeatedComplexMessage test;
    test.setTestRepeatedComplex({msg, msg, msg});
    QByteArray result = test.serialize();
    //qDebug() << "result " << result.toHex();

    ASSERT_TRUE(result == QByteArray::fromHex("0a0c0819120832067177657274790a0c0819120832067177657274790a0c081912083206717765727479")
                || result == QByteArray::fromHex("0a0c1208320671776572747908190a0c1208320671776572747908190a0c120832067177657274790819"));

    test.setTestRepeatedComplex({});
    result = test.serialize();
    ASSERT_TRUE(result.isEmpty());
}

TEST_F(SerializationTest, BoolMessageSerializeTest)
{
    SimpleBoolMessage test;
    test.setTestFieldBool(true);
    QByteArray result = test.serialize();
    ASSERT_EQ(2, result.size());
    ASSERT_TRUE(result == QByteArray::fromHex("0801"));

    test.setTestFieldBool(false);
    result = test.serialize();
    ASSERT_EQ(0, result.size());
    ASSERT_TRUE(result == QByteArray::fromHex(""));
}

TEST_F(SerializationTest, SimpleEnumMessageSerializeTest)
{
    SimpleEnumMessage test;
    test.setLocalEnum(SimpleEnumMessage::LOCAL_ENUM_VALUE2);
    QByteArray result = test.serialize();
    ASSERT_EQ(2, result.size());
    ASSERT_STREQ(result.toHex().toStdString().c_str(), "0802");
}

TEST_F(SerializationTest, SimpleFixed32StringMapSerializeTest)
{
    SimpleFixed32StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "3a0a0d0a000000120374656e3a0e0d0f00000012076669667465656e3a110d2a000000120a666f757274792074776f");
}

TEST_F(SerializationTest, SimpleSFixed32StringMapSerializeTest)
{
    SimpleSFixed32StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "4a170dd6ffffff12106d696e757320666f757274792074776f4a0a0d0a000000120374656e4a0e0d0f00000012076669667465656e");
}

TEST_F(SerializationTest, SimpleInt32StringMapSerializeTest)
{
    SimpleInt32StringMapMessage test;
    test.setMapField({{-10, {"minus ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "1a1108f6ffffff0f12096d696e75732074656e1a0b080f12076669667465656e1a0e082a120a666f757274792074776f");
}

TEST_F(SerializationTest, SimpleSInt32StringMapSerializeTest)
{
    SimpleSInt32StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "0a14085312106d696e757320666f757274792074776f0a070814120374656e0a0b081e12076669667465656e");
}

TEST_F(SerializationTest, SimpleUInt32StringMapSerializeTest)
{
    SimpleUInt32StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "2a07080a120374656e2a0b080f12076669667465656e2a0e082a120a666f757274792074776f");
}

TEST_F(SerializationTest, SimpleFixed64StringMapSerializeTest)
{
    SimpleFixed64StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "420e090a00000000000000120374656e4212090f0000000000000012076669667465656e4215092a00000000000000120a666f757274792074776f");
}

TEST_F(SerializationTest, SimpleSFixed64StringMapSerializeTest)
{
    SimpleSFixed64StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "521b09d6ffffffffffffff12106d696e757320666f757274792074776f520e090a00000000000000120374656e5212090f0000000000000012076669667465656e");
}

TEST_F(SerializationTest, SimpleInt64StringMapSerializeTest)
{
    SimpleInt64StringMapMessage test;
    test.setMapField({{-10, {"minus ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "221608f6ffffffffffffffff0112096d696e75732074656e220b080f12076669667465656e220e082a120a666f757274792074776f");
}

TEST_F(SerializationTest, SimpleSInt64StringMapSerializeTest)
{
    SimpleSInt64StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "1214085312106d696e757320666f757274792074776f12070814120374656e120b081e12076669667465656e");
}

TEST_F(SerializationTest, SimpleUInt64StringMapSerializeTest)
{
    SimpleUInt64StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "3207080a120374656e320b080f12076669667465656e320e082a120a666f757274792074776f");
}

TEST_F(SerializationTest, SimpleStringStringMapSerializeTest)
{
    SimpleStringStringMapMessage test;
    test.setMapField({{"ben", "ten"}, {"what is the answer?", "fourty two"}, {"sweet", "fifteen"}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "6a0a0a0362656e120374656e6a100a05737765657412076669667465656e6a210a13776861742069732074686520616e737765723f120a666f757274792074776f");
}

TEST_F(SerializationTest, FieldIndexRangeTest)
{
    FieldIndexTest1Message msg1;
    msg1.setTestField(1);
    QByteArray result = msg1.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "f80102");

    FieldIndexTest2Message msg2;
    msg2.setTestField(1);
    result = msg2.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "f8ff0302");

    FieldIndexTest3Message msg3;
    msg3.setTestField(1);
    result = msg3.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "f8ffff0702");

    FieldIndexTest4Message msg4;
    msg4.setTestField(1);
    result = msg4.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                 "f8ffffff0f02");
}

TEST_F(SerializationTest, DISABLED_BenchmarkTest)
{
    SimpleIntMessage msg;
    for (int i = INT16_MIN; i < INT16_MAX; i++) {
        msg.setTestFieldInt(i);
        msg.serialize();
    }
}
