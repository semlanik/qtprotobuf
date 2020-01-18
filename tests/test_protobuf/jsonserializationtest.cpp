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

#include <gtest/gtest.h>
#include <QByteArray>
#include <QString>

#include <qprotobufjsonserializer.h>

#include "simpletest.qpb.h"
#include "qtprotobuf_global.qpb.h"

using namespace qtprotobufnamespace::tests;

namespace QtProtobuf {
namespace tests {

class JsonSerializationTest : public ::testing::Test
{
public:
    JsonSerializationTest() = default;
    void SetUp() override;
    static void SetUpTestCase() {
        QtProtobuf::qRegisterProtobufTypes();
        qtprotobufnamespace::tests::qRegisterProtobufTypes();
        qtprotobufnamespace1::externaltests::qRegisterProtobufTypes();
        qtprotobufnamespace::tests::globalenums::qRegisterProtobufTypes();
    }

protected:
    std::unique_ptr<QProtobufJsonSerializer> serializer;
};

void JsonSerializationTest::SetUp() {
    serializer.reset(new QProtobufJsonSerializer);
}

TEST_F(JsonSerializationTest, DISABLED_FixedInt32MessageDeserializeTest)
{
    SimpleFixedInt32Message msg;
    msg.setTestFieldFixedInt32(555);
    QByteArray result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldFixedInt32\":555}");
}


}
}
