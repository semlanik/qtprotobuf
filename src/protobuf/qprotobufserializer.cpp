/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Viktor Kopp <vifactor@gmail.com>
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

#include "qabstractprotobufserializer.h"
#include "qprotobufserializer_p.h"
#include "qprotobufserializerregistry.h"

#include <QMetaProperty>

using namespace QtProtobuf;

QProtobufSerializer::QProtobufSerializer()
{
    QProtobufSerializerPrivate::wrapSerializer<float>(m_handlers, QProtobufSerializerPrivate::serializeBasic<float>, QProtobufSerializerPrivate::deserializeBasic<float>, Fixed32);
    QProtobufSerializerPrivate::wrapSerializer<double>(m_handlers, QProtobufSerializerPrivate::serializeBasic<double>, QProtobufSerializerPrivate::deserializeBasic<double>, Fixed64);
    QProtobufSerializerPrivate::wrapSerializer<int32>(m_handlers, QProtobufSerializerPrivate::serializeBasic<int32>, QProtobufSerializerPrivate::deserializeBasic<int32>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<int64>(m_handlers, QProtobufSerializerPrivate::serializeBasic<int64>, QProtobufSerializerPrivate::deserializeBasic<int64>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<uint32>(m_handlers, QProtobufSerializerPrivate::serializeBasic<uint32>, QProtobufSerializerPrivate::deserializeBasic<uint32>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<uint64>(m_handlers, QProtobufSerializerPrivate::serializeBasic<uint64>, QProtobufSerializerPrivate::deserializeBasic<uint64>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<sint32>(m_handlers, QProtobufSerializerPrivate::serializeBasic<sint32>, QProtobufSerializerPrivate::deserializeBasic<sint32>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<sint64>(m_handlers, QProtobufSerializerPrivate::serializeBasic<sint64>, QProtobufSerializerPrivate::deserializeBasic<sint64>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<fixed32>(m_handlers, QProtobufSerializerPrivate::serializeBasic<fixed32>, QProtobufSerializerPrivate::deserializeBasic<fixed32>, Fixed32);
    QProtobufSerializerPrivate::wrapSerializer<fixed64>(m_handlers, QProtobufSerializerPrivate::serializeBasic<fixed64>, QProtobufSerializerPrivate::deserializeBasic<fixed64>, Fixed64);
    QProtobufSerializerPrivate::wrapSerializer<sfixed32>(m_handlers, QProtobufSerializerPrivate::serializeBasic<sfixed32>, QProtobufSerializerPrivate::deserializeBasic<sfixed32>, Fixed32);
    QProtobufSerializerPrivate::wrapSerializer<sfixed64>(m_handlers, QProtobufSerializerPrivate::serializeBasic<sfixed64>, QProtobufSerializerPrivate::deserializeBasic<sfixed64>, Fixed64);
    QProtobufSerializerPrivate::wrapSerializer<bool>(m_handlers, QProtobufSerializerPrivate::serializeBasic<uint32>, QProtobufSerializerPrivate::deserializeBasic<uint32>, Varint);
    QProtobufSerializerPrivate::wrapSerializer<QString>(m_handlers, QProtobufSerializerPrivate::serializeBasic<QString>, QProtobufSerializerPrivate::deserializeBasic<QString>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<QByteArray>(m_handlers, QProtobufSerializerPrivate::serializeBasic<QByteArray>, QProtobufSerializerPrivate::deserializeBasic<QByteArray>, LengthDelimited);

    QProtobufSerializerPrivate::wrapSerializer<FloatList>(m_handlers, QProtobufSerializerPrivate::serializeListType<float>, QProtobufSerializerPrivate::deserializeList<float>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<DoubleList>(m_handlers, QProtobufSerializerPrivate::serializeListType<double>, QProtobufSerializerPrivate::deserializeList<double>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<fixed32List>(m_handlers, QProtobufSerializerPrivate::serializeListType<fixed32>, QProtobufSerializerPrivate::deserializeList<fixed32>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<fixed64List>(m_handlers, QProtobufSerializerPrivate::serializeListType<fixed64>, QProtobufSerializerPrivate::deserializeList<fixed64>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<sfixed32List>(m_handlers, QProtobufSerializerPrivate::serializeListType<sfixed32>, QProtobufSerializerPrivate::deserializeList<sfixed32>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<sfixed64List>(m_handlers, QProtobufSerializerPrivate::serializeListType<sfixed64>, QProtobufSerializerPrivate::deserializeList<sfixed64>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<int32List>(m_handlers, QProtobufSerializerPrivate::serializeListType<int32>, QProtobufSerializerPrivate::deserializeList<int32>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<int64List>(m_handlers, QProtobufSerializerPrivate::serializeListType<int64>, QProtobufSerializerPrivate::deserializeList<int64>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<sint32List>(m_handlers, QProtobufSerializerPrivate::serializeListType<sint32>, QProtobufSerializerPrivate::deserializeList<sint32>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<sint64List>(m_handlers, QProtobufSerializerPrivate::serializeListType<sint64>, QProtobufSerializerPrivate::deserializeList<sint64>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<uint32List>(m_handlers, QProtobufSerializerPrivate::serializeListType<uint32>, QProtobufSerializerPrivate::deserializeList<uint32>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<uint64List>(m_handlers, QProtobufSerializerPrivate::serializeListType<uint64>, QProtobufSerializerPrivate::deserializeList<uint64>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<QStringList>(m_handlers, QProtobufSerializerPrivate::serializeListType<QString>, QProtobufSerializerPrivate::deserializeList<QString>, LengthDelimited);
    QProtobufSerializerPrivate::wrapSerializer<QByteArrayList>(m_handlers, QProtobufSerializerPrivate::serializeListType<QByteArray>, QProtobufSerializerPrivate::deserializeList<QByteArray>, LengthDelimited);

}

QByteArray QProtobufSerializer::serializeProperty(const QVariant &propertyValue, int fieldIndex, bool isEnum)
{
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex
                  << static_cast<QMetaType::Type>(propertyValue.type());

    QByteArray result;
    WireTypes type = UnknownWireType;

    int userType = propertyValue.userType();
    if (isEnum) {
        userType = qMetaTypeId<int64>();
    }

    auto handler = QProtobufSerializerRegistry::handler(userType);
    type = handler.type;
    result.append(handler.serializer(propertyValue, fieldIndex));

    if (fieldIndex != NotUsedFieldIndex
            && type != UnknownWireType) {
        result.prepend(QProtobufSerializerPrivate::encodeHeader(fieldIndex, type));
    }
    return result;
}

void QProtobufSerializer::deserializeProperty(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject)
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
        auto deserializer = QProtobufSerializerRegistry::handler(userType).deserializer;
        deserializer(it, newPropertyValue);
    }
    metaProperty.write(object, newPropertyValue);
}

QByteArray QProtobufSerializer::serializeObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject)
{
    return QProtobufSerializerPrivate::prependLengthDelimitedSize(serializeObjectCommon(object, propertyOrdering, metaObject));
}

void QProtobufSerializer::deserializeObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject)
{
    QByteArray array = QProtobufSerializerPrivate::deserializeLengthDelimited(it);
    deserializeObjectCommon(object, array, propertyOrdering, metaObject);
}

QByteArray QProtobufSerializer::serializeListObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject, int fieldIndex)
{
    QByteArray result = QProtobufSerializerPrivate::encodeHeader(fieldIndex, LengthDelimited);
    result.append(serializeObject(object, propertyOrdering, metaObject));
    return result;
}

void QProtobufSerializer::deserializeListObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject)
{
    deserializeObject(object, it, propertyOrdering, metaObject);
}

QByteArray QProtobufSerializer::serializeMapPair(const QVariant &key, const QVariant &value, int fieldIndex)
{
    QByteArray result = QProtobufSerializerPrivate::encodeHeader(fieldIndex, LengthDelimited);
    result.append(QProtobufSerializerPrivate::prependLengthDelimitedSize(serializeProperty(key, 1, false) + serializeProperty(value, 2, false)));
    return result;
}

void QProtobufSerializer::deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it)
{
    int mapIndex = 0;
    WireTypes type = WireTypes::UnknownWireType;
    unsigned int count = QProtobufSerializerPrivate::deserializeVarintCommon<uint32>(it);
    qProtoDebug() << __func__ << "count:" << count;
    QProtobufSelfcheckIterator last = it + count;
    while (it != last) {
        QProtobufSerializerPrivate::decodeHeader(it, mapIndex, type);
        if (mapIndex == 1) {
            QProtobufSerializerPrivate::deserializeMapField(key, it);
        } else {
            QProtobufSerializerPrivate::deserializeMapField(value, it);
        }
    }
}

void QProtobufSerializerPrivate::skipVarint(QProtobufSelfcheckIterator &it)
{
    while ((*it) & 0x80) {
        ++it;
    }
    ++it;
}

void QProtobufSerializerPrivate::skipLengthDelimited(QProtobufSelfcheckIterator &it)
{
    //Get length of lenght-delimited field
    unsigned int length = QProtobufSerializerPrivate::deserializeBasic<uint32>(it).toUInt();
    it += length;
}

int QProtobufSerializerPrivate::skipSerializedFieldBytes(QProtobufSelfcheckIterator &it, WireTypes type)
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

void QProtobufSerializerPrivate::deserializeMapField(QVariant &value, QProtobufSelfcheckIterator &it)
{
    QProtobufSerializerRegistry::handler(value.userType()).deserializer(it, value);
}
