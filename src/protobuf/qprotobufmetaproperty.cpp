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

#include "qprotobufmetaproperty.h"
#include "qtprotobuftypes.h"

#include <string>

//TODO: Code under unnamed namespace should be moved to the common header that is relevant for generator and metaproperty
namespace  {
int constexpr constexpr_strlen(const char* str)
{
    return *str ? 1 + constexpr_strlen(str + 1) : 0;
}
const std::vector<std::string> ListOfQmlExeptions{"id", "property", "import"};
constexpr const char *privateSuffix = "_p";
constexpr const char *protoSuffix = "_proto";
constexpr int privateSuffixLenght = constexpr_strlen(privateSuffix);
constexpr int protoSuffixLenght = constexpr_strlen(protoSuffix);
}

using namespace QtProtobuf;
QProtobufMetaProperty::QProtobufMetaProperty(const QMetaProperty &metaProperty, int fieldIndex) : QMetaProperty(metaProperty)
  , m_fieldIndex(fieldIndex)
{

}

QString QProtobufMetaProperty::protoPropertyName() const
{
    QString protoName(name());
    if ((userType() == qMetaTypeId<QtProtobuf::int32>()
            || userType() == qMetaTypeId<QtProtobuf::fixed32>()
            || userType() == qMetaTypeId<QtProtobuf::sfixed32>())
            && protoName.endsWith(privateSuffix)) {
        return protoName.mid(0, protoName.size() - privateSuffixLenght);
    }

    if (protoName.endsWith(protoSuffix)) {
        auto tmpProtoName = protoName.mid(0, protoName.size() - protoSuffixLenght);
        if (std::find(ListOfQmlExeptions.begin(), ListOfQmlExeptions.end(), protoName.toStdString()) != ListOfQmlExeptions.end()) {
            return tmpProtoName;
        }
    }

    return protoName;
}
