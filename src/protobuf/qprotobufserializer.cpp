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

#include "qprotobufserializer.h"
#include "qprotobufserializer_p.h"

#include <QMetaProperty>

using namespace QtProtobuf;

QProtobufSerializer::~QProtobufSerializer() = default;

QProtobufSerializer::QProtobufSerializer() : d_ptr(new QProtobufSerializerPrivate(this))
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

QByteArray QProtobufSerializer::serializeMessage(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) const
{
    QByteArray result;
    for (const auto &field : propertyOrdering) {
        int propertyIndex = field.second;
        int fieldIndex = field.first;
        Q_ASSERT_X(fieldIndex < 536870912 && fieldIndex > 0, "", "fieldIndex is out of range");
        QMetaProperty metaProperty = metaObject.property(propertyIndex);
        const char *propertyName = metaProperty.name();
        const QVariant &propertyValue = object->property(propertyName);
        result.append(d_ptr->serializeProperty(propertyValue, fieldIndex, metaProperty));
    }

    return result;
}

void QProtobufSerializer::deserializeMessage(QObject *object, const QByteArray &data, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) const
{
    for (QProtobufSelfcheckIterator it(data); it != data.end();) {
        d_ptr->deserializeProperty(object, it, propertyOrdering, metaObject);
    }
}

QByteArray QProtobufSerializer::serializeObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) const
{
    return QProtobufSerializerPrivate::prependLengthDelimitedSize(serializeMessage(object, propertyOrdering, metaObject));
}

void QProtobufSerializer::deserializeObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) const
{
    QByteArray array = QProtobufSerializerPrivate::deserializeLengthDelimited(it);
    deserializeMessage(object, array, propertyOrdering, metaObject);
}

QByteArray QProtobufSerializer::serializeListObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject, int fieldIndex) const
{
    QByteArray result = QProtobufSerializerPrivate::encodeHeader(fieldIndex, LengthDelimited);
    result.append(serializeObject(object, propertyOrdering, metaObject));
    return result;
}

void QProtobufSerializer::deserializeListObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) const
{
    deserializeObject(object, it, propertyOrdering, metaObject);
}

QByteArray QProtobufSerializer::serializeMapPair(const QVariant &key, const QVariant &value, int fieldIndex) const
{
    QByteArray result = QProtobufSerializerPrivate::encodeHeader(fieldIndex, LengthDelimited);
    result.append(QProtobufSerializerPrivate::prependLengthDelimitedSize(d_ptr->serializeProperty(key, 1, QMetaProperty()) + d_ptr->serializeProperty(value, 2, QMetaProperty())));
    return result;
}

void QProtobufSerializer::deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it) const
{
    int mapIndex = 0;
    WireTypes type = WireTypes::UnknownWireType;
    unsigned int count = QProtobufSerializerPrivate::deserializeVarintCommon<uint32>(it);
    qProtoDebug() << __func__ << "count:" << count;
    QProtobufSelfcheckIterator last = it + count;
    while (it != last) {
        QProtobufSerializerPrivate::decodeHeader(it, mapIndex, type);
        if (mapIndex == 1) {
            //TODO: replace with some common function
            int userType = key.userType();
            auto basicIt = QProtobufSerializerPrivate::handlers.find(userType);
            if (basicIt != QProtobufSerializerPrivate::handlers.end()) {
                basicIt->second.deserializer(it, key);
            } else {
                auto &handler = QtProtobufPrivate::findHandler(userType);
                handler.deserializer(this, it, key);
            }
        } else {
            //TODO: replace with some common function
            int userType = value.userType();
            auto basicIt = QProtobufSerializerPrivate::handlers.find(userType);
            if (basicIt != QProtobufSerializerPrivate::handlers.end()) {
                basicIt->second.deserializer(it, value);
            } else {
                auto &handler = QtProtobufPrivate::findHandler(userType);
                handler.deserializer(this, it, value);//throws if not implemented
            }
        }
    }
}


QProtobufSerializerPrivate::QProtobufSerializerPrivate(QProtobufSerializer *q) : q_ptr(q)
{

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


QByteArray QProtobufSerializerPrivate::serializeProperty(const QVariant &propertyValue, int fieldIndex, const QMetaProperty &metaProperty)
{
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex
                  << static_cast<QMetaType::Type>(propertyValue.type());

    QByteArray result;
    WireTypes type = UnknownWireType;

    int userType = propertyValue.userType();
    if (metaProperty.isEnumType()) {
        userType = qMetaTypeId<int64>();
    }

    //TODO: replace with some common function
    auto basicIt = QProtobufSerializerPrivate::handlers.find(userType);
    if (basicIt != QProtobufSerializerPrivate::handlers.end()) {
        type = basicIt->second.type;
        result.append(basicIt->second.serializer(propertyValue, fieldIndex));
    } else {
        auto &handler = QtProtobufPrivate::findHandler(userType);
        type = handler.type;
        result.append(handler.serializer(q_ptr, propertyValue, fieldIndex));//throws if not implemented
    }

    if (fieldIndex != QtProtobufPrivate::NotUsedFieldIndex
            && type != UnknownWireType) {
        result.prepend(QProtobufSerializerPrivate::encodeHeader(fieldIndex, type));
    }
    return result;
}

void QProtobufSerializerPrivate::deserializeProperty(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject)
{
    //Each iteration we expect iterator is setup to beginning of next chunk
    int fieldNumber = QtProtobufPrivate::NotUsedFieldIndex;
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
        //TODO: replace with some common function
        auto basicIt = QProtobufSerializerPrivate::handlers.find(userType);
        if (basicIt != QProtobufSerializerPrivate::handlers.end()) {
            basicIt->second.deserializer(it, newPropertyValue);
        } else {
            auto &handler = QtProtobufPrivate::findHandler(userType);
            handler.deserializer(q_ptr, it, newPropertyValue);//throws if not implemented
        }
    }
    metaProperty.write(object, newPropertyValue);
}

QProtobufSerializerPrivate::SerializerRegistry QProtobufSerializerPrivate::handlers = {};
