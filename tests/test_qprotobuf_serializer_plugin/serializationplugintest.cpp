/*
 * MIT License
 *
 * Copyright (c) 2019 Tatyana Borisova <tanusshhka@mail.ru>
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

#include "serializationplugintest.h"
#include "qprotobufserializerregistry_p.h"

using namespace QtProtobuf::tests;
using namespace QtProtobuf;

namespace {
const QLatin1String ProtobufSerializator("protobuf");
const QLatin1String JsonSerializator("json");
const QLatin1String Serializationplugin("serializationplugin");
}

QString SerializationPluginTest::loadedTestPlugin;
void SerializationPluginTest::SetUpTestCase()
{
    //Register all types
    QtProtobuf::qRegisterProtobufTypes();
}

void SerializationPluginTest::SetUp()
{
    loadedTestPlugin = QProtobufSerializerRegistry::instance().loadPlugin(Serializationplugin);
    serializers[ProtobufSerializator] = QProtobufSerializerRegistry::instance().getSerializer(ProtobufSerializator, loadedTestPlugin);
    serializers[JsonSerializator] = QProtobufSerializerRegistry::instance().getSerializer(JsonSerializator, loadedTestPlugin);
}

TEST_F(SerializationPluginTest, ProtobufLoadedSerializeTest)
{
    ASSERT_NE(serializers[ProtobufSerializator].get(), nullptr);
}

TEST_F(SerializationPluginTest, JsonLoadedSerializeTest)
{
    ASSERT_NE(serializers[JsonSerializator].get(), nullptr);
}

TEST_F(SerializationPluginTest, PluginLoadedSerializeTest)
{
    ASSERT_FLOAT_EQ(QProtobufSerializerRegistry::instance().pluginVersion(loadedTestPlugin), 1.0);
}

TEST_F(SerializationPluginTest, PortobufLoadedSerializeTest)
{
    ASSERT_FLOAT_EQ(QProtobufSerializerRegistry::instance().pluginProtobufVersion(loadedTestPlugin), 1.0);
}

TEST_F(SerializationPluginTest, RatingLoadedSerializeTest)
{
    ASSERT_EQ(QProtobufSerializerRegistry::instance().pluginRating(loadedTestPlugin), 0);
}

TEST_F(SerializationPluginTest, UnknownPluginLoadedSerializeTest)
{
    ASSERT_ANY_THROW(QProtobufSerializerRegistry::instance().getSerializer("SomeName", loadedTestPlugin));
}
