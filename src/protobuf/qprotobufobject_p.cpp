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

#include "qprotobufobject_p.h"

#include "qprotobufserializer_p.h"

using namespace qtprotobuf;

ProtobufObjectPrivate::SerializerRegistry ProtobufObjectPrivate::serializers = {};

void ProtobufObjectPrivate::registerSerializers()
{
    QProtobufSerializerPrivate::wrapSerializer<float>(QProtobufSerializerPrivate::serializeBasic<float>, QProtobufSerializerPrivate::deserializeBasic<float>, Fixed32);
    QProtobufSerializerPrivate::wrapSerializer<double>(QProtobufSerializerPrivate::serializeBasic<double>, QProtobufSerializerPrivate::deserializeBasic<double>, Fixed64);
    QProtobufSerializerPrivate::wrapSerializer<int32>(QProtobufSerializerPrivate::serializeBasic<int32>, QProtobufSerializerPrivate::deserializeBasic<int32>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<int64>(QProtobufSerializerPrivate::serializeBasic<int64>, QProtobufSerializerPrivate::deserializeBasic<int64>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<uint32>(QProtobufSerializerPrivate::serializeBasic<uint32>, QProtobufSerializerPrivate::deserializeBasic<uint32>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<uint64>(QProtobufSerializerPrivate::serializeBasic<uint64>, QProtobufSerializerPrivate::deserializeBasic<uint64>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<sint32>(QProtobufSerializerPrivate::serializeBasic<sint32>, QProtobufSerializerPrivate::deserializeBasic<sint32>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<sint64>(QProtobufSerializerPrivate::serializeBasic<sint64>, QProtobufSerializerPrivate::deserializeBasic<sint64>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<fixed32>(QProtobufSerializerPrivate::serializeBasic<fixed32>, QProtobufSerializerPrivate::deserializeBasic<fixed32>, Fixed32);
    QProtobufSerializerPrivate::wrapSerializer<fixed64>(QProtobufSerializerPrivate::serializeBasic<fixed64>, QProtobufSerializerPrivate::deserializeBasic<fixed64>, Fixed64);
    QProtobufSerializerPrivate::wrapSerializer<sfixed32>(QProtobufSerializerPrivate::serializeBasic<sfixed32>, QProtobufSerializerPrivate::deserializeBasic<sfixed32>, Fixed32);
    QProtobufSerializerPrivate::wrapSerializer<sfixed64>(QProtobufSerializerPrivate::serializeBasic<sfixed64>, QProtobufSerializerPrivate::deserializeBasic<sfixed64>, Fixed64);
    QProtobufSerializerPrivate::wrapSerializer<bool>(QProtobufSerializerPrivate::serializeBasic<uint32>, QProtobufSerializerPrivate::deserializeBasic<uint32>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<QString>(QProtobufSerializerPrivate::serializeBasic<QString>, QProtobufSerializerPrivate::deserializeBasic<QString>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<QByteArray>(QProtobufSerializerPrivate::serializeBasic<QByteArray>, QProtobufSerializerPrivate::deserializeBasic<QByteArray>, LengthDelimited);

    QProtobufSerializerPrivate::wrapSerializer<FloatList>(QProtobufSerializerPrivate::serializeListType<float>, QProtobufSerializerPrivate::deserializeList<float>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<DoubleList>(QProtobufSerializerPrivate::serializeListType<double>, QProtobufSerializerPrivate::deserializeList<double>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<fixed32List>(QProtobufSerializerPrivate::serializeListType<fixed32>, QProtobufSerializerPrivate::deserializeList<fixed32>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<fixed64List>(QProtobufSerializerPrivate::serializeListType<fixed64>, QProtobufSerializerPrivate::deserializeList<fixed64>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<sfixed32List>(QProtobufSerializerPrivate::serializeListType<sfixed32>, QProtobufSerializerPrivate::deserializeList<sfixed32>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<sfixed64List>(QProtobufSerializerPrivate::serializeListType<sfixed64>, QProtobufSerializerPrivate::deserializeList<sfixed64>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<int32List>(QProtobufSerializerPrivate::serializeListType<int32>, QProtobufSerializerPrivate::deserializeList<int32>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<int64List>(QProtobufSerializerPrivate::serializeListType<int64>, QProtobufSerializerPrivate::deserializeList<int64>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<sint32List>(QProtobufSerializerPrivate::serializeListType<sint32>, QProtobufSerializerPrivate::deserializeList<sint32>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<sint64List>(QProtobufSerializerPrivate::serializeListType<sint64>, QProtobufSerializerPrivate::deserializeList<sint64>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<uint32List>(QProtobufSerializerPrivate::serializeListType<uint32>, QProtobufSerializerPrivate::deserializeList<uint32>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<uint64List>(QProtobufSerializerPrivate::serializeListType<uint64>, QProtobufSerializerPrivate::deserializeList<uint64>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<QStringList>(QProtobufSerializerPrivate::serializeListType<QString>, QProtobufSerializerPrivate::deserializeList<QString>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<QByteArrayList>(QProtobufSerializerPrivate::serializeListType<QByteArray>, QProtobufSerializerPrivate::deserializeList<QByteArray>, LengthDelimited);
}

QByteArray ProtobufObjectPrivate::serializeProperty(const QVariant &propertyValue, int fieldIndex, bool isEnum)
{
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex
                  << static_cast<QMetaType::Type>(propertyValue.type());

    QByteArray result;
    WireTypes type = UnknownWireType;

    int userType = propertyValue.userType();
    if (isEnum) {
        userType = qMetaTypeId<int64>();
    }
    auto serializer = serializers.at(userType);//Throws exception if not found
    type = serializer.type;
    result.append(serializer.serializer(propertyValue, fieldIndex));

    if (fieldIndex != NotUsedFieldIndex
            && type != UnknownWireType) {
        result.prepend(QProtobufSerializerPrivate::encodeHeader(fieldIndex, type));
    }
    return result;
}

QByteArray ProtobufObjectPrivate::serializeObjectCommon(const QObject *object, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject)
{
    QByteArray result;
    for (const auto &field : propertyOrdering) {
        int propertyIndex = field.second;
        int fieldIndex = field.first;
        Q_ASSERT_X(fieldIndex < 536870912 && fieldIndex > 0, "", "fieldIndex is out of range");
        QMetaProperty metaProperty = metaObject.property(propertyIndex);
        const char *propertyName = metaProperty.name();
        const QVariant &propertyValue = object->property(propertyName);
        result.append(ProtobufObjectPrivate::serializeProperty(propertyValue, fieldIndex, metaProperty.isEnumType()));
    }

    return result;
}

void ProtobufObjectPrivate::deserializeObjectCommon(QObject *object, const QByteArray &data, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject)
{
    for (SelfcheckIterator it(data); it != data.end();) {
        deserializeProperty(object, it, propertyOrdering, metaObject);
    }
}

QByteArray ProtobufObjectPrivate::serializeObject(const QObject *object, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject)
{
    return QProtobufSerializerPrivate::prependLengthDelimitedSize(serializeObjectCommon(object, propertyOrdering, metaObject));
}

void ProtobufObjectPrivate::deserializeObject(QObject *object, SelfcheckIterator &it, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject)
{
    QByteArray array = QProtobufSerializerPrivate::deserializeLengthDelimited(it);
    deserializeObjectCommon(object, array, propertyOrdering, metaObject);
}

QByteArray ProtobufObjectPrivate::serializeListObject(const QObject *object, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject, int fieldIndex)
{
    QByteArray result = QProtobufSerializerPrivate::encodeHeader(fieldIndex, LengthDelimited);
    result.append(serializeObject(object, propertyOrdering, metaObject));
    return result;
}

QByteArray ProtobufObjectPrivate::serializeMapPair(const QVariant &key, const QVariant &value, int fieldIndex)
{
    QByteArray result = QProtobufSerializerPrivate::encodeHeader(fieldIndex, LengthDelimited);
    result.append(QProtobufSerializerPrivate::prependLengthDelimitedSize(serializeProperty(key, 1, false) + serializeProperty(value, 2, false)));
    return result;
}

void ProtobufObjectPrivate::deserializeMapPair(QVariant &key, QVariant &value, SelfcheckIterator &it)
{
    int mapIndex = 0;
    WireTypes type = WireTypes::UnknownWireType;
    unsigned int count = QProtobufSerializerPrivate::deserializeVarintCommon<uint32>(it);
    qProtoDebug() << __func__ << "count:" << count;
    SelfcheckIterator last = it + count;
    while (it != last) {
        QProtobufSerializerPrivate::decodeHeader(it, mapIndex, type);
        if(mapIndex == 1) {
            deserializeMapField(key, it);
        } else {
            deserializeMapField(value, it);
        }
    }
}


void ProtobufObjectPrivate::deserializeProperty(QObject *object, SelfcheckIterator &it, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject)
{
    //Each iteration we expect iterator is setup to beginning of next chunk
    int fieldNumber = NotUsedFieldIndex;
    WireTypes wireType = UnknownWireType;
    if (!QProtobufSerializerPrivate::decodeHeader(it, fieldNumber, wireType)) {
        qProtoCritical() << "Message received doesn't contains valid header byte. "
                            "Trying next, but seems stream is broken" << QString::number((*it), 16);
        throw std::invalid_argument("Message received doesn't contains valid header byte. "
                              "Seems stream is broken");
    }

    auto propertyNumberIt = propertyOrdering.find(fieldNumber);
    if (propertyNumberIt == std::end(propertyOrdering)) {
        auto bytesCount = QProtobufSerializerPrivate::skipSerializedFieldBytes(it, wireType);
        qProtoWarning() << "Message received contains unexpected/optional field. WireType:" << wireType
                        << ", field number: " << fieldNumber << "Skipped:" << (bytesCount + 1) << "bytes";
        return;
    }

    int propertyIndex = propertyNumberIt->second;
    QMetaProperty metaProperty = metaObject.property(propertyIndex);

    qProtoDebug() << __func__ << " wireType: " << wireType << " metaProperty: " << metaProperty.typeName()
                  << "currentByte:" << QString::number((*it), 16);

    QVariant newPropertyValue;
    if (metaProperty.isEnumType()) {
        newPropertyValue = QVariant::fromValue(int32_t(QProtobufSerializerPrivate::deserializeBasic<int64>(it).value<int64>()._t));
    } else {
        newPropertyValue = metaProperty.read(object);
        int userType = metaProperty.userType();
        auto deserializer = serializers.at(userType).deserializer;//Throws exception if not found
        deserializer(it, newPropertyValue);
    }
    metaProperty.write(object, newPropertyValue);
}
