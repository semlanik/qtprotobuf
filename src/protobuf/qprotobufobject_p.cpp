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

QByteArray ProtobufObjectPrivate::serializeProperty(const QVariant &propertyValue, int fieldIndex, const QMetaProperty &metaProperty)
{
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex
                  << "typeName" << metaProperty.typeName() << static_cast<QMetaType::Type>(propertyValue.type());

    QByteArray result;
    WireTypes type = UnknownWireType;

    if (metaProperty.isEnumType()) {
        type = Varint;
        result.append(QProtobufSerializerPrivate::serializeBasic(int64(propertyValue.value<int32_t>()), fieldIndex));
    } else {
        result.append(serializeUserType(propertyValue, fieldIndex, type));
    }

    if (fieldIndex != NotUsedFieldIndex
            && type != UnknownWireType) {
        result.prepend(encodeHeader(fieldIndex, type));
    }
    return result;
}

QByteArray ProtobufObjectPrivate::serializeUserType(const QVariant &propertyValue, int &fieldIndex, WireTypes &type)
{
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex;

    int userType = propertyValue.userType();
    auto serializer = serializers.at(userType);//Throws exception if not found
    type = serializer.type;
    return serializer.serializer(propertyValue, fieldIndex);
}

void ProtobufObjectPrivate::deserializeProperty(QObject *object, const QMetaProperty &metaProperty, SelfcheckIterator &it, WireTypes wireType)
{
    qProtoDebug() << __func__ << " wireType: " << wireType << " metaProperty: " << metaProperty.typeName()
                  << "currentByte:" << QString::number((*it), 16);

    QVariant newPropertyValue;
    if (metaProperty.isEnumType()) {
        newPropertyValue = QVariant::fromValue(int32_t(QProtobufSerializerPrivate::deserializeBasic<int64>(it).value<int64>()._t));
    } else {
        newPropertyValue = metaProperty.read(object);
        deserializeUserType(metaProperty, it, newPropertyValue);
    }
    metaProperty.write(object, newPropertyValue);
}

void ProtobufObjectPrivate::deserializeUserType(const QMetaProperty &metaProperty, SelfcheckIterator &it, QVariant &out)
{
    qProtoDebug() << __func__ << "userType" << metaProperty.userType() << "typeName" << metaProperty.typeName()
                  << "currentByte:" << QString::number((*it), 16);
    int userType = metaProperty.userType();
    auto deserializer = serializers.at(userType).deserializer;//Throws exception if not found
    deserializer(it, out);
}

void ProtobufObjectPrivate::skipVarint(SelfcheckIterator &it)
{
    while ((*it) & 0x80) {
        ++it;
    }
    ++it;
}

void ProtobufObjectPrivate::skipLengthDelimited(SelfcheckIterator &it)
{
    //Get length of lenght-delimited field
    unsigned int length = QProtobufSerializerPrivate::deserializeBasic<uint32>(it).toUInt();
    it += length;
}

int ProtobufObjectPrivate::skipSerializedFieldBytes(SelfcheckIterator &it, WireTypes type)
{
    const auto initialIt = QByteArray::const_iterator(it);
    switch (type) {
    case WireTypes::Varint:
        skipVarint(it);
        break;
    case WireTypes::Fixed32:
        it += sizeof(decltype(fixed32::_t));
        break;
    case WireTypes::Fixed64:
        it += sizeof(decltype(fixed64::_t));
        break;
    case WireTypes::LengthDelimited:
        skipLengthDelimited(it);
        break;
    case WireTypes::UnknownWireType:
    default:
        throw std::invalid_argument("Cannot skip due to undefined length of the redundant field.");
    }

    return std::distance(initialIt, QByteArray::const_iterator(it));
}
