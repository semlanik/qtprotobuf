/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>
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

class JsonDeserializationTest : public ::testing::Test
{
public:
    JsonDeserializationTest() = default;
    void SetUp() override;
    static void SetUpTestCase() {
        QtProtobuf::qRegisterProtobufTypes();
    }

protected:
    std::unique_ptr<QProtobufJsonSerializer> serializer;
};

void JsonDeserializationTest::SetUp() {
    serializer.reset(new QProtobufJsonSerializer);
}

TEST_F(JsonDeserializationTest, FixedInt32MessageSerializeTest)
{
    SimpleFixedInt32Message msg;
    msg.deserialize(serializer.get(), QByteArray("{\"testFieldFixedInt32\": 5}"));
    EXPECT_EQ(msg.testFieldFixedInt32(), 5);

    msg.deserialize(serializer.get(), QByteArray("{\"testFieldFixedInt32\": 0}"));
    EXPECT_EQ(msg.testFieldFixedInt32(), 0);

    msg.deserialize(serializer.get(), QByteArray("{\"testFieldFixedInt32\":555}"));
    EXPECT_EQ(msg.testFieldFixedInt32(), 555);
}

TEST_F(JsonDeserializationTest, SFixedInt32MessageSerializeTest)
{
    SimpleSFixedInt32Message msg;
    msg.deserialize(serializer.get(), QByteArray("{\"testFieldFixedInt32\":555}"));
    EXPECT_EQ(msg.testFieldFixedInt32(), 555);

    msg.deserialize(serializer.get(), QByteArray("{\"testFieldFixedInt32\":-555}"));
    EXPECT_EQ(msg.testFieldFixedInt32(), -555);
}

TEST_F(JsonDeserializationTest, Int32MessageSerializeTest)
{
    SimpleIntMessage msg;
    msg.deserialize(serializer.get(), QByteArray("{\"testFieldInt\":555}"));
    EXPECT_EQ(msg.testFieldInt(), 555);
}

TEST_F(JsonDeserializationTest, SInt32MessageSerializeTest)
{
    SimpleSIntMessage msg;
    msg.deserialize(serializer.get(), QByteArray("{\"testFieldInt\":555}"));
    EXPECT_EQ(msg.testFieldInt(), 555);

    msg.deserialize(serializer.get(), QByteArray("{\"testFieldInt\":-555}"));
    EXPECT_EQ(msg.testFieldInt(), -555);
}

TEST_F(JsonDeserializationTest, UInt32MessageSerializeTest)
{
    SimpleUIntMessage msg;
    msg.deserialize(serializer.get(), QByteArray("{\"testFieldInt\":555}"));
    EXPECT_EQ(msg.testFieldInt(), 555);
}

TEST_F(JsonDeserializationTest, FixedInt64MessageSerializeTest)
{
    SimpleFixedInt64Message msg;
    msg.deserialize(serializer.get(), QByteArray("{\"testFieldFixedInt64\":555}"));
    EXPECT_EQ(msg.testFieldFixedInt64(), 555);
}

TEST_F(JsonDeserializationTest, Int64MessageSerializeTest)
{
    SimpleInt64Message msg;
    msg.deserialize(serializer.get(), QByteArray("{\"testFieldInt\":555}"));
    EXPECT_EQ(msg.testFieldInt(), 555);
}

TEST_F(JsonDeserializationTest, SInt64MessageSerializeTest)
{
    SimpleSInt64Message msg;
    msg.deserialize(serializer.get(), QByteArray("{\"testFieldInt\":555}"));
    EXPECT_EQ(msg.testFieldInt(), 555);

    msg.deserialize(serializer.get(), QByteArray("{\"testFieldInt\":-555}"));
    EXPECT_EQ(msg.testFieldInt(), -555);
}

TEST_F(JsonDeserializationTest, UInt64MessageSerializeTest)
{
    SimpleUInt64Message msg;
    msg.deserialize(serializer.get(), QByteArray("{\"testFieldInt\":555}"));
    EXPECT_EQ(msg.testFieldInt(), 555);
}


TEST_F(JsonDeserializationTest, FloatMessageSerializeTest)
{
    SimpleFloatMessage test;
    test.deserialize(serializer.get(), QByteArray("{\"testFieldFloat\": 0.1}"));
    EXPECT_EQ(test.testFieldFloat(), 0.1f);

    test.deserialize(serializer.get(), QByteArray("{\"testFieldFloat\": 1.17549e-38}"));
    EXPECT_EQ(test.testFieldFloat(), 1.17549e-38f);

    test.deserialize(serializer.get(), QByteArray("{\"testFieldFloat\": 3.40282e+38}"));
    EXPECT_EQ(test.testFieldFloat(), 3.40282e+38f);

    test.deserialize(serializer.get(), QByteArray("{\"testFieldFloat\": -4.2}"));
    EXPECT_EQ(test.testFieldFloat(), -4.2f);

    test.deserialize(serializer.get(), QByteArray("{\"testFieldFloat\": 0}"));
    EXPECT_EQ(test.testFieldFloat(), 0.0f);

    test.deserialize(serializer.get(), QByteArray("{\"testFieldFloat\": 0.0}"));
    EXPECT_EQ(test.testFieldFloat(), 0.0f);
}

TEST_F(JsonDeserializationTest, DoubleMessageSerializeTest)
{
    SimpleDoubleMessage test;
    test.deserialize(serializer.get(), QByteArray("{\"testFieldDouble\": 0.1}"));
    EXPECT_EQ(test.testFieldDouble(), 0.1);

    test.deserialize(serializer.get(), QByteArray("{\"testFieldDouble\": 2.2250738585072014e-308}"));
    EXPECT_EQ(test.testFieldDouble(), 2.2250738585072014e-308);

    test.deserialize(serializer.get(), QByteArray("{\"testFieldDouble\": 1.7976931348623157e+308}"));
    EXPECT_EQ(test.testFieldDouble(), 1.7976931348623157e+308);

    test.deserialize(serializer.get(), QByteArray("{\"testFieldDouble\": 0}"));
    EXPECT_EQ(test.testFieldDouble(), 0);

    test.deserialize(serializer.get(), QByteArray("{\"testFieldDouble\": 0.0}"));
    EXPECT_EQ(test.testFieldDouble(), 0);
}

TEST_F(JsonDeserializationTest, StringMessageSerializeTest)
{
    SimpleStringMessage test;
    test.deserialize(serializer.get(), QByteArray("{\"testFieldString\":\"qwerty\"}"));
    EXPECT_STREQ(test.testFieldString().toStdString().c_str(), "qwerty");
}

TEST_F(JsonDeserializationTest, BytesMessageSerializeTest)
{
    SimpleBytesMessage test;
    test.deserialize(serializer.get(), QByteArray("{\"testFieldBytes\":\"ABKEBDKBBFlWAFJmSnZqeHFiZ3ZGd1MxWXZPWlhndGo1ZmZHTFM3QAFpTkh6OW9aSW9LYm03ejhIATc5eEJ1cAlQa3BRWHZHb08wOU9ZOXhSYXd4M2VPQXM5eGpvVEExeEpocncyOFRBY3ExQ2ViWWxDOVdVZlFDNmhJYW50YU5keUhpS1RvZmZpMFp0N2xhNDJTUnhYWlNQNEd1eGJjWklwNTNwSm55Q3dmQ3kxcWRGY3pUMGRtbjdoOGZweUFkZW1FYXZ3RmVkYTRkMFBBcEdmU1Uyakx0MzlYOGtZVUJ4Tk0yV2dBTFJCZ0hkVmRlODdxNlBpNVU2OVRqaE1kMjhXMVNGRDFEeHlvZ0NDcnFPY3QyWlBJQ29MbnJxZEYzT2ROempSVkxmZXl2UThMZ0x2Uk5GUjlXZldBeUF6NzluS2dCYW1kOE50bHZ0NE1nMzVFNWdWUzJnN0FRN3JrbTcyY0Jkblc5c0NFeUdhYmVYQXVINWo0R1JidUxUN3FCWldEY0ZMRjRTc0NkUzNXZkZHZE5IZndhaWp6eWtCeW83MVB2RlZsVFhIMldKV29GdlI1RkFMakJUbjdiQ2RQMHBBaVNiTENZOFh6Mk1zYzNkQmI1RmY5R0lTUGJVcE5tVXZCZE1aTUhRdnFPbVROWEVQcE4wYjc0TURPTVFmV0pTaE9vM05rQXZNanMIKg\"}"));
    EXPECT_STREQ(test.testFieldBytes().toHex().toStdString().c_str(), "0012840432810459560052664a766a78716267764677533159764f5a5867746a356666474c53374001694e487a396f5a496f4b626d377a38480137397842757009506b70515876476f4f30394f5939785261777833654f417339786a6f544131784a68727732385441637131436562596c43395755665143366849616e74614e647948694b546f666669305a74376c613432535278585a53503447757862635a49703533704a6e79437766437931716446637a5430646d6e3768386670794164656d456176774665646134643050417047665355326a4c74333958386b595542784e4d325767414c524267486456646538377136506935553639546a684d6432385731534644314478796f67434372714f6374325a5049436f4c6e72716446334f644e7a6a52564c6665797651384c674c76524e4652395766574179417a37396e4b6742616d64384e746c7674344d6733354535675653326737415137726b6d37326342646e5739734345794761626558417548356a34475262754c543771425a574463464c463453734364533357664647644e48667761696a7a796b42796f3731507646566c54584832574a576f4676523546414c6a42546e37624364503070416953624c435938587a324d73633364426235466639474953506255704e6d557642644d5a4d485176714f6d544e584550704e306237344d444f4d5166574a53684f6f334e6b41764d6a73082a");
}

TEST_F(JsonDeserializationTest, ComplexTypeSerializeTest)
{
    ComplexMessage test;

    test.deserialize(serializer.get(), QByteArray("{\"testComplexField\":{\"testFieldString\":\"qwerty\"},\"testFieldInt\":42}"));
    EXPECT_EQ(test.testFieldInt(), 42);
    EXPECT_STREQ(test.testComplexField().testFieldString().toStdString().c_str(), "qwerty");

    test.setTestFieldInt(99);
    test.setTestComplexField({});
    test.deserialize(serializer.get(), QByteArray("{\"testFieldInt\":42,\"testComplexField\":{\"testFieldString\":\"qwerty\"}}"));
    EXPECT_EQ(test.testFieldInt(), 42);
    EXPECT_STREQ(test.testComplexField().testFieldString().toStdString().c_str(), "qwerty");

    test.setTestFieldInt(0);
    test.setTestComplexField({});
    test.deserialize(serializer.get(), QByteArray("{\"testComplexField\":{\"testFieldString\":\"YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs\"},\"testFieldInt\":42}"));
    EXPECT_STREQ(test.testComplexField().testFieldString().toStdString().c_str(), "YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs");
    EXPECT_EQ(test.testFieldInt(), 42);

    test.setTestFieldInt(0);
    test.setTestComplexField({});
    test.deserialize(serializer.get(), QByteArray("{\"testFieldInt\":42,\"testComplexField\":{\"testFieldString\":\"YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs\"}}"));
    EXPECT_STREQ(test.testComplexField().testFieldString().toStdString().c_str(), "YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs");
    EXPECT_EQ(test.testFieldInt(), 42);

    test.setTestFieldInt(0);
    test.setTestComplexField({});
    test.deserialize(serializer.get(), QByteArray("{\"testComplexField\":{\"testFieldString\":\"qwerty\"},\"testFieldInt\":-45}"));
    EXPECT_STREQ(test.testComplexField().testFieldString().toStdString().c_str(), "qwerty");
    EXPECT_EQ(test.testFieldInt(), -45);

    test.setTestFieldInt(0);
    test.setTestComplexField({});
    test.deserialize(serializer.get(), QByteArray("{\"testFieldInt\":-45,\"testComplexField\":{\"testFieldString\":\"qwerty\"}}"));
    EXPECT_STREQ(test.testComplexField().testFieldString().toStdString().c_str(), "qwerty");
    EXPECT_EQ(test.testFieldInt(), -45);
}

TEST_F(JsonDeserializationTest, RepeatedIntMessageTest)
{
    RepeatedIntMessage test;
    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[0,1,321,-65999,123245,-3,3]}");
    EXPECT_TRUE(test.testRepeatedInt() == QtProtobuf::int32List({0, 1, 321, -65999, 123245, -3, 3}));

    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[]}");
    EXPECT_TRUE(test.testRepeatedInt().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedSIntMessageTest)
{
    RepeatedSIntMessage test;
    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[1,321,-65999,123245,-3,3,0]}");
    EXPECT_TRUE(test.testRepeatedInt() == QtProtobuf::sint32List({1, 321, -65999, 123245, -3, 3, 0}));

    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[]}");
    EXPECT_TRUE(test.testRepeatedInt().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedUIntMessageTest)
{
    RepeatedUIntMessage test;
    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[1,0,321,65999,123245,3]}");
    EXPECT_TRUE(test.testRepeatedInt() == QtProtobuf::uint32List({1, 0, 321, 65999, 123245, 3}));

    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[]}");
    EXPECT_TRUE(test.testRepeatedInt().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedInt64MessageTest)
{
    RepeatedInt64Message test;
    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[1,321,-65999,12324523123123,-3,0,3]}");
    EXPECT_TRUE(test.testRepeatedInt() == QtProtobuf::int64List({1, 321, -65999, 12324523123123, -3, 0, 3}));

    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[]}");
    EXPECT_TRUE(test.testRepeatedInt().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedSInt64MessageTest)
{
    RepeatedSInt64Message test;
    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[1,321,-65999,12324523123123,0,-3,3]}");
    EXPECT_TRUE(test.testRepeatedInt() == QtProtobuf::sint64List({1, 321, -65999, 12324523123123, 0, -3, 3}));

    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[]}");
    EXPECT_TRUE(test.testRepeatedInt().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedUInt64MessageTest)
{
    RepeatedUInt64Message test;
    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[1,321,0,65999,123245,123245324235425234,3]}");
    EXPECT_TRUE(test.testRepeatedInt() == QtProtobuf::uint64List({1, 321, 0, 65999, 123245, 123245324235425234, 3}));

    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[]}");
    EXPECT_TRUE(test.testRepeatedInt().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedFixedIntMessageTest)
{
    RepeatedFixedIntMessage test;
    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[1,321,65999,12324523,3,3,0]}");
    EXPECT_TRUE(test.testRepeatedInt() == QtProtobuf::fixed32List({1, 321, 65999, 12324523, 3, 3, 0}));

    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[]}");
    EXPECT_TRUE(test.testRepeatedInt().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedSFixedIntMessageTest)
{
    RepeatedSFixedIntMessage test;
    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[0,1,321,-65999,12324523,-3,3]}");
    EXPECT_TRUE(test.testRepeatedInt() == QtProtobuf::sfixed32List({0, 1, 321, -65999, 12324523, -3, 3}));

    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[]}");
    EXPECT_TRUE(test.testRepeatedInt().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedFixedInt64MessageTest)
{
    RepeatedFixedInt64Message test;
    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[1,321,65999,123245324235425234,3,3,0]}");
    EXPECT_TRUE(test.testRepeatedInt() == QtProtobuf::fixed64List({1, 321, 65999, 123245324235425234, 3, 3, 0}));

    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[]}");
    EXPECT_TRUE(test.testRepeatedInt().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedSFixedInt64MessageTest)
{
    RepeatedSFixedInt64Message test;
    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[1,321,-65999,123245324235425234,-3,3,0]}");
    EXPECT_TRUE(test.testRepeatedInt() == QtProtobuf::sfixed64List({1, 321, -65999, 123245324235425234, -3, 3, 0}));

    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[1,321,-65999,11,123245324235425234,-3,3,0]}");
    EXPECT_TRUE(test.testRepeatedInt() == QtProtobuf::sfixed64List({1, 321, -65999, 11, 123245324235425234, -3, 3, 0}));

    test.deserialize(serializer.get(), "{\"testRepeatedInt\":[]}");
    EXPECT_TRUE(test.testRepeatedInt().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedStringMessageTest)
{
    RepeatedStringMessage test;
    test.deserialize(serializer.get(), "{\"testRepeatedString\":[\"aaaa\",\"bbbbb\",\"ccc\",\"dddddd\",\"eeeee\",\"\"]}");
    EXPECT_TRUE(test.testRepeatedString() == QStringList({"aaaa","bbbbb","ccc","dddddd","eeeee",""}));

    test.deserialize(serializer.get(), "{\"testRepeatedString\":[]}");
    EXPECT_TRUE(test.testRepeatedString().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedDoubleMessageTest)
{
    RepeatedDoubleMessage test;
    test.deserialize(serializer.get(), "{\"testRepeatedDouble\":[0.1,0.2,0.3,0.4,0.5]}");
    EXPECT_TRUE(test.testRepeatedDouble() == QList<double>({0.1, 0.2, 0.3, 0.4, 0.5}));

    test.deserialize(serializer.get(), "{\"testRepeatedDouble\":[]}");
    EXPECT_TRUE(test.testRepeatedDouble().isEmpty());
}

TEST_F(JsonDeserializationTest, RepeatedBytesMessageTest)
{
    RepeatedBytesMessage test;
    test.deserialize(serializer.get(), "{\"testRepeatedBytes\":[\"AQIDBAUG\",\"/////w==\",\"6urq6uo=\",\"AQIDBAUG\"]}");
    EXPECT_TRUE(test.testRepeatedBytes() == QList<QByteArray>({QByteArray::fromHex("010203040506"),
                                                               QByteArray::fromHex("ffffffff"),
                                                               QByteArray::fromHex("eaeaeaeaea"),
                                                               QByteArray::fromHex("010203040506")}));

    test.deserialize(serializer.get(), "{\"testRepeatedBytes\":[]}");
    EXPECT_TRUE(test.testRepeatedBytes().isEmpty());

    test.deserialize(serializer.get(), "{\"testRepeatedBytes\":[\"AQIDBAUG\",\"\",\"6urq6uo=\",\"AQIDBAUG\"]}");
    EXPECT_TRUE(test.testRepeatedBytes() == QList<QByteArray>({QByteArray::fromHex("010203040506"),
                                                               QByteArray::fromHex(""),
                                                               QByteArray::fromHex("eaeaeaeaea"),
                                                               QByteArray::fromHex("010203040506")}));
}

TEST_F(JsonDeserializationTest, RepeatedFloatMessageTest)
{
    RepeatedFloatMessage test;
    test.deserialize(serializer.get(), "{\"testRepeatedFloat\":[0.4,1.2,0.5,1.4,0.6]}");
    EXPECT_TRUE(test.testRepeatedFloat() == QList<float>({0.4f, 1.2f, 0.5f, 1.4f, 0.6f}));

    test.deserialize(serializer.get(), "{\"testRepeatedFloat\":[]}");
    EXPECT_TRUE(test.testRepeatedFloat().isEmpty());
}

TEST_F(JsonDeserializationTest, BoolMessageSerializeTest)
{
    SimpleBoolMessage test;
    test.deserialize(serializer.get(), "{\"testFieldBool\":true}");
    EXPECT_TRUE(test.testFieldBool());

    test.deserialize(serializer.get(), "{\"testFieldBool\":false}");
    EXPECT_FALSE(test.testFieldBool());
}

//TEST_F(JsonDeserializationTest, RepeatedComplexMessageTest)
//{
//    SimpleStringMessage stringMsg;
//    stringMsg.setTestFieldString("qwerty");
//    QSharedPointer<ComplexMessage> msg(new ComplexMessage);
//    msg->setTestFieldInt(25);
//    msg->setTestComplexField(stringMsg);
//    RepeatedComplexMessage test;
//    test.setTestRepeatedComplex({msg, msg, msg});
//    QByteArray result = test.serialize(serializer.get());
//    EXPECT_TRUE(QString::fromUtf8(result).toStdString() == "{\"testRepeatedComplex\":[{\"testComplexField\":{\"testFieldString\":\"qwerty\"},\"testFieldInt\":25},{\"testComplexField\":{\"testFieldString\":\"qwerty\"},\"testFieldInt\":25},{\"testComplexField\":{\"testFieldString\":\"qwerty\"},\"testFieldInt\":25}]}"
//                 || QString::fromUtf8(result).toStdString() == "{\"testRepeatedComplex\":[{\"testFieldInt\":25,\"testComplexField\":{\"testFieldString\":\"qwerty\"}},{\"testFieldInt\":25,\"testComplexField\":{\"testFieldString\":\"qwerty\"}},{\"testFieldInt\":25,\"testComplexField\":{\"testFieldString\":\"qwerty\"}}]}");

//    test.setTestRepeatedComplex({});
//    result = test.serialize(serializer.get());
//    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"testRepeatedComplex\":[]}");
//}

//TEST_F(JsonDeserializationTest, SimpleEnumMessageSerializeTest)
//{
//    SimpleEnumMessage test;
//    test.setLocalEnum(SimpleEnumMessage::LOCAL_ENUM_VALUE2);
//    QByteArray result = test.serialize(serializer.get());
//    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"localEnum\":\"LOCAL_ENUM_VALUE2\"}");
//}


//TEST_F(JsonDeserializationTest, SimpleEnumListMessageTest)
//{
//    SimpleEnumListMessage msg;

//    msg.setLocalEnumList({SimpleEnumListMessage::LOCAL_ENUM_VALUE0,
//                          SimpleEnumListMessage::LOCAL_ENUM_VALUE1,
//                          SimpleEnumListMessage::LOCAL_ENUM_VALUE2,
//                          SimpleEnumListMessage::LOCAL_ENUM_VALUE1,
//                          SimpleEnumListMessage::LOCAL_ENUM_VALUE2,
//                          SimpleEnumListMessage::LOCAL_ENUM_VALUE3});
//    QByteArray result = msg.serialize(serializer.get());
//    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"localEnumList\":[\"LOCAL_ENUM_VALUE0\",\"LOCAL_ENUM_VALUE1\",\"LOCAL_ENUM_VALUE2\",\"LOCAL_ENUM_VALUE1\",\"LOCAL_ENUM_VALUE2\",\"LOCAL_ENUM_VALUE3\"]}");
//    msg.setLocalEnumList({});
//    result = msg.serialize(serializer.get());
//    EXPECT_STREQ(QString::fromUtf8(result).toStdString().c_str(), "{\"localEnumList\":[]}");
//}

//TEST_F(JsonDeserializationTest, SimpleFixed32StringMapSerializeTest)
//{
//    SimpleFixed32StringMapMessage test;
//    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
//    QByteArray result = test.serialize(serializer.get());

//    ASSERT_STREQ(result.toStdString().c_str(),
//                 "{\"mapField\":{\"10\":\"ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
//}

//TEST_F(JsonDeserializationTest, SimpleSFixed32StringMapSerializeTest)
//{
//    SimpleSFixed32StringMapMessage test;
//    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
//    QByteArray result = test.serialize(serializer.get());

//    ASSERT_STREQ(result.toStdString().c_str(),
//                 "{\"mapField\":{\"-42\":\"minus fourty two\",\"10\":\"ten\",\"15\":\"fifteen\"}}");
//}

//TEST_F(JsonDeserializationTest, SimpleInt32StringMapSerializeTest)
//{
//    SimpleInt32StringMapMessage test;
//    test.setMapField({{-10, {"minus ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
//    QByteArray result = test.serialize(serializer.get());

//    ASSERT_STREQ(result.toStdString().c_str(),
//                 "{\"mapField\":{\"-10\":\"minus ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
//}

//TEST_F(JsonDeserializationTest, SimpleSInt32StringMapSerializeTest)
//{
//    SimpleSInt32StringMapMessage test;
//    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
//    QByteArray result = test.serialize(serializer.get());

//    ASSERT_STREQ(result.toStdString().c_str(),
//                 "{\"mapField\":{\"-42\":\"minus fourty two\",\"10\":\"ten\",\"15\":\"fifteen\"}}");
//}

//TEST_F(JsonDeserializationTest, SimpleUInt32StringMapSerializeTest)
//{
//    SimpleUInt32StringMapMessage test;
//    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
//    QByteArray result = test.serialize(serializer.get());

//    ASSERT_STREQ(result.toStdString().c_str(),
//                 "{\"mapField\":{\"10\":\"ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
//}

//TEST_F(JsonDeserializationTest, SimpleFixed64StringMapSerializeTest)
//{
//    SimpleFixed64StringMapMessage test;
//    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
//    QByteArray result = test.serialize(serializer.get());

//    ASSERT_STREQ(result.toStdString().c_str(),
//                 "{\"mapField\":{\"10\":\"ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
//}

//TEST_F(JsonDeserializationTest, SimpleSFixed64StringMapSerializeTest)
//{
//    SimpleSFixed64StringMapMessage test;
//    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
//    QByteArray result = test.serialize(serializer.get());

//    ASSERT_STREQ(result.toStdString().c_str(),
//                 "{\"mapField\":{\"-42\":\"minus fourty two\",\"10\":\"ten\",\"15\":\"fifteen\"}}");
//}

//TEST_F(JsonDeserializationTest, SimpleInt64StringMapSerializeTest)
//{
//    SimpleInt64StringMapMessage test;
//    test.setMapField({{-10, {"minus ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
//    QByteArray result = test.serialize(serializer.get());

//    ASSERT_STREQ(result.toStdString().c_str(),
//                 "{\"mapField\":{\"-10\":\"minus ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
//}

//TEST_F(JsonDeserializationTest, SimpleSInt64StringMapSerializeTest)
//{
//    SimpleSInt64StringMapMessage test;
//    test.setMapField({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}});
//    QByteArray result = test.serialize(serializer.get());
//    ASSERT_STREQ(result.toStdString().c_str(),
//                 "{\"mapField\":{\"-42\":\"minus fourty two\",\"10\":\"ten\",\"15\":\"fifteen\"}}");
//}

//TEST_F(JsonDeserializationTest, SimpleUInt64StringMapSerializeTest)
//{
//    SimpleUInt64StringMapMessage test;
//    test.setMapField({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}});
//    QByteArray result = test.serialize(serializer.get());

//    ASSERT_STREQ(result.toStdString().c_str(),
//                 "{\"mapField\":{\"10\":\"ten\",\"15\":\"fifteen\",\"42\":\"fourty two\"}}");
//}

//TEST_F(JsonDeserializationTest, SimpleStringStringMapSerializeTest)
//{
//    SimpleStringStringMapMessage test;
//    test.setMapField({{"ben", "ten"}, {"what is the answer?", "fourty two"}, {"sweet", "fifteen"}});
//    QByteArray result = test.serialize(serializer.get());

//    ASSERT_STREQ(result.toStdString().c_str(),
//                 "{\"mapField\":{\"ben\":\"ten\",\"sweet\":\"fifteen\",\"what is the answer?\":\"fourty two\"}}");
//}

}
}
