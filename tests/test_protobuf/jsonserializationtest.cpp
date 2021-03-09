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
    }

protected:
    std::unique_ptr<QProtobufJsonSerializer> serializer;
};

void JsonSerializationTest::SetUp() {
    serializer.reset(new QProtobufJsonSerializer);
}

TEST_F(JsonSerializationTest, FixedInt32MessageSerializeTest)
{
    SimpleFixedInt32Message msg;
    msg.setTestFieldFixedInt32(555);
    QByteArray result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldFixedInt32\":555}");
}

TEST_F(JsonSerializationTest, SFixedInt32MessageSerializeTest)
{
    SimpleSFixedInt32Message msg;
    msg.setTestFieldFixedInt32(555);
    QByteArray result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldFixedInt32\":555}");

    msg.setTestFieldFixedInt32(-555);
    result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldFixedInt32\":-555}");
}

TEST_F(JsonSerializationTest, Int32MessageSerializeTest)
{
    qtprotobufnamespace::tests::SimpleIntMessage msg;
    msg.setTestFieldInt(555);
    QByteArray result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldInt\":555}");
}

TEST_F(JsonSerializationTest, SInt32MessageSerializeTest)
{
    SimpleSIntMessage msg;
    msg.setTestFieldInt(555);
    QByteArray result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldInt\":555}");

    msg.setTestFieldInt(-555);
    result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldInt\":-555}");
}

TEST_F(JsonSerializationTest, UInt32MessageSerializeTest)
{
    SimpleUIntMessage msg;
    msg.setTestFieldInt(555);
    QByteArray result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldInt\":555}");
}

TEST_F(JsonSerializationTest, FixedInt64MessageSerializeTest)
{
    SimpleFixedInt64Message msg;
    msg.setTestFieldFixedInt64(555);
    QByteArray result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldFixedInt64\":555}");
}


TEST_F(JsonSerializationTest, Int64MessageSerializeTest)
{
    SimpleInt64Message msg;
    msg.setTestFieldInt(555);
    QByteArray result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldInt\":555}");
}

TEST_F(JsonSerializationTest, SInt64MessageSerializeTest)
{
    SimpleSInt64Message msg;
    msg.setTestFieldInt(555);
    QByteArray result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldInt\":555}");

    msg.setTestFieldInt(-555);
    result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldInt\":-555}");
}

TEST_F(JsonSerializationTest, UInt64MessageSerializeTest)
{
    SimpleUInt64Message msg;
    msg.setTestFieldInt(555);
    QByteArray result = msg.serialize(serializer.get());
    ASSERT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldInt\":555}");
}


TEST_F(JsonSerializationTest, FloatMessageSerializeTest)
{
    SimpleFloatMessage test;
    test.setTestFieldFloat(0.1f);
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldFloat\":0.1}");

    test.setTestFieldFloat(FLT_MIN);
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldFloat\":1.17549e-38}");

    test.setTestFieldFloat(FLT_MAX);
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldFloat\":3.40282e+38}");

    test.setTestFieldFloat(-4.2f);
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldFloat\":-4.2}");

    test.setTestFieldFloat(-0.0f);
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldFloat\":0}");
}

TEST_F(JsonSerializationTest, DoubleMessageSerializeTest)
{
    SimpleDoubleMessage test;
    test.setTestFieldDouble(0.1);
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldDouble\":0.1}");

    test.setTestFieldDouble(DBL_MIN);
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldDouble\":2.2250738585072014e-308}");

    test.setTestFieldDouble(DBL_MAX);
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldDouble\":1.7976931348623157e+308}");

    test.setTestFieldDouble(-4.2);
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldDouble\":-4.2}");

    test.setTestFieldDouble(-0.0);
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldDouble\":0}");
}

TEST_F(JsonSerializationTest, StringMessageSerializeTest)
{
    SimpleStringMessage test;
    test.setTestFieldString("qwerty");
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldString\":\"qwerty\"}");
}

TEST_F(JsonSerializationTest, BytesMessageSerializeTest)
{
    SimpleBytesMessage test;
    test.setTestFieldBytes(QByteArray::fromHex("0012840432810459560052664a766a78716267764677533159764f5a5867746a356666474c53374001694e487a396f5a496f4b626d377a38480137397842757009506b70515876476f4f30394f5939785261777833654f417339786a6f544131784a68727732385441637131436562596c43395755665143366849616e74614e647948694b546f666669305a74376c613432535278585a53503447757862635a49703533704a6e79437766437931716446637a5430646d6e3768386670794164656d456176774665646134643050417047665355326a4c74333958386b595542784e4d325767414c524267486456646538377136506935553639546a684d6432385731534644314478796f67434372714f6374325a5049436f4c6e72716446334f644e7a6a52564c6665797651384c674c76524e4652395766574179417a37396e4b6742616d64384e746c7674344d6733354535675653326737415137726b6d37326342646e5739734345794761626558417548356a34475262754c543771425a574463464c463453734364533357664647644e48667761696a7a796b42796f3731507646566c54584832574a576f4676523546414c6a42546e37624364503070416953624c435938587a324d73633364426235466639474953506255704e6d557642644d5a4d485176714f6d544e584550704e306237344d444f4d5166574a53684f6f334e6b41764d6a73082a"));
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldBytes\":\"ABKEBDKBBFlWAFJmSnZqeHFiZ3ZGd1MxWXZPWlhndGo1ZmZHTFM3QAFpTkh6OW9aSW9LYm03ejhIATc5eEJ1cAlQa3BRWHZHb08wOU9ZOXhSYXd4M2VPQXM5eGpvVEExeEpocncyOFRBY3ExQ2ViWWxDOVdVZlFDNmhJYW50YU5keUhpS1RvZmZpMFp0N2xhNDJTUnhYWlNQNEd1eGJjWklwNTNwSm55Q3dmQ3kxcWRGY3pUMGRtbjdoOGZweUFkZW1FYXZ3RmVkYTRkMFBBcEdmU1Uyakx0MzlYOGtZVUJ4Tk0yV2dBTFJCZ0hkVmRlODdxNlBpNVU2OVRqaE1kMjhXMVNGRDFEeHlvZ0NDcnFPY3QyWlBJQ29MbnJxZEYzT2ROempSVkxmZXl2UThMZ0x2Uk5GUjlXZldBeUF6NzluS2dCYW1kOE50bHZ0NE1nMzVFNWdWUzJnN0FRN3JrbTcyY0Jkblc5c0NFeUdhYmVYQXVINWo0R1JidUxUN3FCWldEY0ZMRjRTc0NkUzNXZkZHZE5IZndhaWp6eWtCeW83MVB2RlZsVFhIMldKV29GdlI1RkFMakJUbjdiQ2RQMHBBaVNiTENZOFh6Mk1zYzNkQmI1RmY5R0lTUGJVcE5tVXZCZE1aTUhRdnFPbVROWEVQcE4wYjc0TURPTVFmV0pTaE9vM05rQXZNanMIKg==\"}");
}

TEST_F(JsonSerializationTest, ComplexTypeSerializeTest)
{
    SimpleStringMessage stringMsg;
    stringMsg.setTestFieldString("qwerty");

    ComplexMessage test;
    test.setTestFieldInt(42);
    test.setTestComplexField(stringMsg);
    QByteArray result = test.serialize(serializer.get());
    EXPECT_TRUE(QString::fromUtf8(result).toStdString() == std::string("{\"testComplexField\":{\"testFieldString\":\"qwerty\"},\"testFieldInt\":42}")
                || QString::fromUtf8(result).toStdString() == std::string("{\"testFieldInt\":42,\"testComplexField\":{\"testFieldString\":\"qwerty\"}}"));

    stringMsg.setTestFieldString("YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs");
    test.setTestFieldInt(42);
    test.setTestComplexField(stringMsg);
    result = test.serialize(serializer.get());
    EXPECT_TRUE(QString::fromUtf8(result).toStdString() == "{\"testComplexField\":{\"testFieldString\":\"YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs\"},\"testFieldInt\":42}"
                || QString::fromUtf8(result).toStdString() == "{\"testFieldInt\":42,\"testComplexField\":{\"testFieldString\":\"YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs\"}}");

    stringMsg.setTestFieldString("qwerty");
    test.setTestFieldInt(-45);
    test.setTestComplexField(stringMsg);

    result = test.serialize(serializer.get());
    EXPECT_TRUE(QString::fromUtf8(result).toStdString() == "{\"testComplexField\":{\"testFieldString\":\"qwerty\"},\"testFieldInt\":-45}"
                || QString::fromUtf8(result).toStdString() == "{\"testFieldInt\":-45,\"testComplexField\":{\"testFieldString\":\"qwerty\"}}");
}

TEST_F(JsonSerializationTest, RepeatedIntMessageTest)
{
    RepeatedIntMessage test;
    test.setTestRepeatedInt({0, 1, 321, -65999, 123245, -3, 3});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[0,1,321,-65999,123245,-3,3]}");

    test.setTestRepeatedInt(int32List());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedSIntMessageTest)
{
    RepeatedSIntMessage test;
    test.setTestRepeatedInt({1, 321, -65999, 123245, -3, 3, 0});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[1,321,-65999,123245,-3,3,0]}");

    test.setTestRepeatedInt(sint32List());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedUIntMessageTest)
{
    RepeatedUIntMessage test;
    test.setTestRepeatedInt({1, 0, 321, 65999, 123245, 3});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[1,0,321,65999,123245,3]}");

    test.setTestRepeatedInt(uint32List());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedInt64MessageTest)
{
    RepeatedInt64Message test;
    test.setTestRepeatedInt({1, 321, -65999, 12324523123123, -3, 0, 3});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[1,321,-65999,12324523123123,-3,0,3]}");

    test.setTestRepeatedInt(int64List());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedSInt64MessageTest)
{
    RepeatedSInt64Message test;
    test.setTestRepeatedInt({1, 321, -65999, 12324523123123, 0, -3, 3});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[1,321,-65999,12324523123123,0,-3,3]}");

    test.setTestRepeatedInt(sint64List());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedUInt64MessageTest)
{
    RepeatedUInt64Message test;
    test.setTestRepeatedInt({1, 321, 0, 65999, 123245, 123245324235425234, 3});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[1,321,0,65999,123245,123245324235425234,3]}");

    test.setTestRepeatedInt(uint64List());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedFixedIntMessageTest)
{
    RepeatedFixedIntMessage test;
    test.setTestRepeatedInt({1, 321, 65999, 12324523, 3, 3, 0});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[1,321,65999,12324523,3,3,0]}");

    test.setTestRepeatedInt(fixed32List());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedSFixedIntMessageTest)
{
    RepeatedSFixedIntMessage test;
    test.setTestRepeatedInt({0, 1, 321, -65999, 12324523, -3, 3});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[0,1,321,-65999,12324523,-3,3]}");

    test.setTestRepeatedInt(sfixed32List());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedFixedInt64MessageTest)
{
    RepeatedFixedInt64Message test;
    test.setTestRepeatedInt({1, 321, 65999, 123245324235425234, 3, 3, 0});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[1,321,65999,123245324235425234,3,3,0]}");

    test.setTestRepeatedInt(fixed64List());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedSFixedInt64MessageTest)
{
    RepeatedSFixedInt64Message test;
    test.setTestRepeatedInt({1, 321, -65999, 123245324235425234, -3, 3, 0});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[1,321,-65999,123245324235425234,-3,3,0]}");

    test.setTestRepeatedInt(sfixed64List());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedInt\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedStringMessageTest)
{
    RepeatedStringMessage test;
    test.setTestRepeatedString({"aaaa","bbbbb","ccc","dddddd","eeeee",""});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedString\":[\"aaaa\",\"bbbbb\",\"ccc\",\"dddddd\",\"eeeee\",\"\"]}");

    test.setTestRepeatedString(QStringList());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedString\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedDoubleMessageTest)
{
    RepeatedDoubleMessage test;
    test.setTestRepeatedDouble({0.1, 0.2, 0.3, 0.4, 0.5});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedDouble\":[0.1,0.2,0.3,0.4,0.5]}");

    test.setTestRepeatedDouble(DoubleList());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedDouble\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedBytesMessageTest)
{
    RepeatedBytesMessage test;
    test.setTestRepeatedBytes({QByteArray::fromHex("010203040506"),
                               QByteArray::fromHex("ffffffff"),
                               QByteArray::fromHex("eaeaeaeaea"),
                               QByteArray::fromHex("010203040506")});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedBytes\":[\"AQIDBAUG\",\"/////w==\",\"6urq6uo=\",\"AQIDBAUG\"]}");

    test.setTestRepeatedBytes(QByteArrayList());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedBytes\":[]}");

    test.setTestRepeatedBytes({QByteArray::fromHex("010203040506"),
                               QByteArray::fromHex(""),
                               QByteArray::fromHex("eaeaeaeaea"),
                               QByteArray::fromHex("010203040506")});
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedBytes\":[\"AQIDBAUG\",\"\",\"6urq6uo=\",\"AQIDBAUG\"]}");
}

TEST_F(JsonSerializationTest, RepeatedFloatMessageTest)
{
    RepeatedFloatMessage test;
    test.setTestRepeatedFloat({0.4f, 1.2f, 0.5f, 1.4f, 0.6f});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedFloat\":[0.4,1.2,0.5,1.4,0.6]}");

    test.setTestRepeatedFloat(FloatList());
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedFloat\":[]}");
}

TEST_F(JsonSerializationTest, RepeatedComplexMessageTest)
{
    SimpleStringMessage stringMsg;
    stringMsg.setTestFieldString("qwerty");
    QSharedPointer<ComplexMessage> msg(new ComplexMessage);
    msg->setTestFieldInt(25);
    msg->setTestComplexField(stringMsg);
    RepeatedComplexMessage test;
    test.setTestRepeatedComplex({msg, msg, msg});
    QByteArray result = test.serialize(serializer.get());
    EXPECT_TRUE(QString::fromUtf8(result).toStdString() == "{\"testRepeatedComplex\":[{\"testComplexField\":{\"testFieldString\":\"qwerty\"},\"testFieldInt\":25},{\"testComplexField\":{\"testFieldString\":\"qwerty\"},\"testFieldInt\":25},{\"testComplexField\":{\"testFieldString\":\"qwerty\"},\"testFieldInt\":25}]}"
                 || QString::fromUtf8(result).toStdString() == "{\"testRepeatedComplex\":[{\"testFieldInt\":25,\"testComplexField\":{\"testFieldString\":\"qwerty\"}},{\"testFieldInt\":25,\"testComplexField\":{\"testFieldString\":\"qwerty\"}},{\"testFieldInt\":25,\"testComplexField\":{\"testFieldString\":\"qwerty\"}}]}");

    test.setTestRepeatedComplex({});
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedComplex\":[]}");
}

TEST_F(JsonSerializationTest, BoolMessageSerializeTest)
{
    SimpleBoolMessage test;
    test.setTestFieldBool(true);
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldBool\":true}");

    test.setTestFieldBool(false);
    result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testFieldBool\":false}");
}

TEST_F(JsonSerializationTest, SimpleEnumMessageSerializeTest)
{
    SimpleEnumMessage test;
    test.setLocalEnum(SimpleEnumMessage::LOCAL_ENUM_VALUE2);
    QByteArray result = test.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"localEnum\":\"LOCAL_ENUM_VALUE2\"}");
}


TEST_F(JsonSerializationTest, SimpleEnumListMessageTest)
{
    SimpleEnumListMessage msg;

    msg.setLocalEnumList({SimpleEnumListMessage::LOCAL_ENUM_VALUE0,
                          SimpleEnumListMessage::LOCAL_ENUM_VALUE1,
                          SimpleEnumListMessage::LOCAL_ENUM_VALUE2,
                          SimpleEnumListMessage::LOCAL_ENUM_VALUE1,
                          SimpleEnumListMessage::LOCAL_ENUM_VALUE2,
                          SimpleEnumListMessage::LOCAL_ENUM_VALUE3});
    QByteArray result = msg.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"localEnumList\":[\"LOCAL_ENUM_VALUE0\",\"LOCAL_ENUM_VALUE1\",\"LOCAL_ENUM_VALUE2\",\"LOCAL_ENUM_VALUE1\",\"LOCAL_ENUM_VALUE2\",\"LOCAL_ENUM_VALUE3\"]}");
    msg.setLocalEnumList({});
    result = msg.serialize(serializer.get());
    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"localEnumList\":[]}");
}

TEST_F(JsonSerializationTest, SimpleFixed32StringMapSerializeTest)
{
    SimpleFixed32StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize(serializer.get());

    ASSERT_STREQ(result.toStdString().c_str(),
                 "{\"mapField\":{\"10\":\"ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
}

TEST_F(JsonSerializationTest, SimpleSFixed32StringMapSerializeTest)
{
    SimpleSFixed32StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize(serializer.get());

    ASSERT_STREQ(result.toStdString().c_str(),
                 "{\"mapField\":{\"-42\":\"minus fourty two\",\"10\":\"ten\",\"15\":\"fifteen\"}}");
}

TEST_F(JsonSerializationTest, SimpleInt32StringMapSerializeTest)
{
    SimpleInt32StringMapMessage test;
    test.setMapField({{-10, {"minus ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize(serializer.get());

    ASSERT_STREQ(result.toStdString().c_str(),
                 "{\"mapField\":{\"-10\":\"minus ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
}

TEST_F(JsonSerializationTest, SimpleSInt32StringMapSerializeTest)
{
    SimpleSInt32StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize(serializer.get());

    ASSERT_STREQ(result.toStdString().c_str(),
                 "{\"mapField\":{\"-42\":\"minus fourty two\",\"10\":\"ten\",\"15\":\"fifteen\"}}");
}

TEST_F(JsonSerializationTest, SimpleUInt32StringMapSerializeTest)
{
    SimpleUInt32StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize(serializer.get());

    ASSERT_STREQ(result.toStdString().c_str(),
                 "{\"mapField\":{\"10\":\"ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
}

TEST_F(JsonSerializationTest, SimpleFixed64StringMapSerializeTest)
{
    SimpleFixed64StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize(serializer.get());

    ASSERT_STREQ(result.toStdString().c_str(),
                 "{\"mapField\":{\"10\":\"ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
}

TEST_F(JsonSerializationTest, SimpleSFixed64StringMapSerializeTest)
{
    SimpleSFixed64StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize(serializer.get());

    ASSERT_STREQ(result.toStdString().c_str(),
                 "{\"mapField\":{\"-42\":\"minus fourty two\",\"10\":\"ten\",\"15\":\"fifteen\"}}");
}

TEST_F(JsonSerializationTest, SimpleInt64StringMapSerializeTest)
{
    SimpleInt64StringMapMessage test;
    test.setMapField({{-10, {"minus ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize(serializer.get());

    ASSERT_STREQ(result.toStdString().c_str(),
                 "{\"mapField\":{\"-10\":\"minus ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
}

TEST_F(JsonSerializationTest, SimpleSInt64StringMapSerializeTest)
{
    SimpleSInt64StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize(serializer.get());
    ASSERT_STREQ(result.toStdString().c_str(),
                 "{\"mapField\":{\"-42\":\"minus fourty two\",\"10\":\"ten\",\"15\":\"fifteen\"}}");
}

TEST_F(JsonSerializationTest, SimpleUInt64StringMapSerializeTest)
{
    SimpleUInt64StringMapMessage test;
    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
    QByteArray result = test.serialize(serializer.get());

    ASSERT_STREQ(result.toStdString().c_str(),
                 "{\"mapField\":{\"10\":\"ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
}

TEST_F(JsonSerializationTest, SimpleStringStringMapSerializeTest)
{
    SimpleStringStringMapMessage test;
    test.setMapField({{"ben", "ten"}, {"what is the answer?", "fourty two"}, {"sweet", "fifteen"}});
    QByteArray result = test.serialize(serializer.get());

    ASSERT_STREQ(result.toStdString().c_str(),
                 "{\"mapField\":{\"ben\":\"ten\",\"sweet\":\"fifteen\",\"what is the answer?\":\"fourty two\"}}");
}

}
}
