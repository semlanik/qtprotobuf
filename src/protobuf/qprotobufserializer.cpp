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

#include "qprotobufmetaproperty.h"
#include "qprotobufmetaobject.h"

using namespace QtProtobuf;

template<>
QByteArray QProtobufSerializerPrivate::serializeListType<QByteArray>(const QByteArrayList &listValue, int &outFieldIndex)
{
    qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

    if (listValue.count() <= 0) {
        outFieldIndex = QtProtobufPrivate::NotUsedFieldIndex;
        return QByteArray();
    }

    QByteArray serializedList;
    for (auto &value : listValue) {
        serializedList.append(QProtobufSerializerPrivate::encodeHeader(outFieldIndex, LengthDelimited));
        serializedList.append(serializeLengthDelimited(value));
    }

    outFieldIndex = QtProtobufPrivate::NotUsedFieldIndex;
    return serializedList;
}

template<>
void QProtobufSerializerPrivate::deserializeList<QByteArray>(QProtobufSelfcheckIterator &it, QVariant &previousValue)
{
    qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

    QByteArrayList list = previousValue.value<QByteArrayList>();
    list.append(deserializeLengthDelimited(it));
    previousValue.setValue(list);
}

template<>
void QProtobufSerializerPrivate::deserializeList<QString>(QProtobufSelfcheckIterator &it, QVariant &previousValue)
{
    qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

    QStringList list = previousValue.value<QStringList>();
    QByteArray value = deserializeLengthDelimited(it);
    list.append(QString::fromUtf8(value));
    previousValue.setValue(list);
}

QProtobufSerializer::~QProtobufSerializer() = default;

QProtobufSerializer::QProtobufSerializer() : d_ptr(new QProtobufSerializerPrivate(this))
{
}

QByteArray QProtobufSerializer::serializeMessage(const QObject *object, const QProtobufMetaObject &metaObject) const
{
    QByteArray result;
    for (const auto &field : metaObject.propertyOrdering) {
        int propertyIndex = field.second;
        int fieldIndex = field.first;
        Q_ASSERT_X(fieldIndex < 536870912 && fieldIndex > 0, "", "fieldIndex is out of range");
        QMetaProperty metaProperty = metaObject.staticMetaObject.property(propertyIndex);
        const char *propertyName = metaProperty.name();
        const QVariant &propertyValue = object->property(propertyName);
        result.append(d_ptr->serializeProperty(propertyValue, QProtobufMetaProperty(metaProperty, fieldIndex)));
    }

    return result;
}

void QProtobufSerializer::deserializeMessage(QObject *object, const QProtobufMetaObject &metaObject, const QByteArray &data) const
{
    for (QProtobufSelfcheckIterator it(data); it != data.end();) {
        d_ptr->deserializeProperty(object, metaObject, it);
    }
}

QByteArray QProtobufSerializer::serializeObject(const QObject *object, const QProtobufMetaObject &metaObject, const QProtobufMetaProperty &metaProperty) const
{
    QByteArray result = QProtobufSerializerPrivate::encodeHeader(metaProperty.protoFieldIndex(), LengthDelimited);
    result.append(QProtobufSerializerPrivate::prependLengthDelimitedSize(serializeMessage(object, metaObject)));
    return result;
}

void QProtobufSerializer::deserializeObject(QObject *object, const QProtobufMetaObject &metaObject, QProtobufSelfcheckIterator &it) const
{
    QByteArray array = QProtobufSerializerPrivate::deserializeLengthDelimited(it);
    deserializeMessage(object, metaObject, array);
}

QByteArray QProtobufSerializer::serializeListObject(const QObject *object, const QProtobufMetaObject &metaObject, const QProtobufMetaProperty &metaProperty) const
{
    return serializeObject(object, metaObject, metaProperty);
}

void QProtobufSerializer::deserializeListObject(QObject *object, const QProtobufMetaObject &metaObject, QProtobufSelfcheckIterator &it) const
{
    deserializeObject(object, metaObject, it);
}

QByteArray QProtobufSerializer::serializeMapPair(const QVariant &key, const QVariant &value, const QProtobufMetaProperty &metaProperty) const
{
    QByteArray result = QProtobufSerializerPrivate::encodeHeader(metaProperty.protoFieldIndex(), LengthDelimited);
    result.append(QProtobufSerializerPrivate::prependLengthDelimitedSize(d_ptr->serializeProperty(key, QProtobufMetaProperty(metaProperty, 1)) + d_ptr->serializeProperty(value, QProtobufMetaProperty(metaProperty, 2))));
    return result;
}

void QProtobufSerializer::deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it) const
{
    d_ptr->deserializeMapPair(key, value, it);
}

QByteArray QProtobufSerializer::serializeEnum(int64 value, const QMetaEnum &/*metaEnum*/, const QtProtobuf::QProtobufMetaProperty &metaProperty) const
{
    WireTypes type = Varint;
    int fieldIndex = metaProperty.protoFieldIndex();
    QByteArray result = QProtobufSerializerPrivate::serializeBasic<int64>(value, fieldIndex);
    if (fieldIndex != QtProtobufPrivate::NotUsedFieldIndex) {
        result.prepend(QProtobufSerializerPrivate::encodeHeader(metaProperty.protoFieldIndex(), type));
    }
    return result;
}

QByteArray QProtobufSerializer::serializeEnumList(const QList<int64> &value, const QMetaEnum &/*metaEnum*/, const QtProtobuf::QProtobufMetaProperty &metaProperty) const
{
    WireTypes type = LengthDelimited;
    int fieldIndex = metaProperty.protoFieldIndex();
    QByteArray result = QProtobufSerializerPrivate::serializeListType<int64>(value, fieldIndex);
    if (fieldIndex != QtProtobufPrivate::NotUsedFieldIndex) {
        result.prepend(QProtobufSerializerPrivate::encodeHeader(metaProperty.protoFieldIndex(), type));
    }
    return result;
}

void QProtobufSerializer::deserializeEnum(int64 &value, const QMetaEnum &/*metaEnum*/, QProtobufSelfcheckIterator &it) const
{
    QVariant variantValue;
    QProtobufSerializerPrivate::deserializeBasic<int64>(it, variantValue);
    value = variantValue.value<int64>();
}

void QProtobufSerializer::deserializeEnumList(QList<int64> &value, const QMetaEnum &/*metaEnum*/, QProtobufSelfcheckIterator &it) const
{
    QVariant variantValue;
    QProtobufSerializerPrivate::deserializeList<int64>(it, variantValue);
    value = variantValue.value<QList<int64>>();
}

QProtobufSerializerPrivate::QProtobufSerializerPrivate(QProtobufSerializer *q) : q_ptr(q)
{
    //if handlers is not empty intialization already done
    if (handlers.empty()) {
        wrapSerializer<float, serializeBasic, deserializeBasic<float>, Fixed32>();
        wrapSerializer<double, serializeBasic, deserializeBasic<double>, Fixed64>();
        wrapSerializer<int32, serializeBasic, deserializeBasic<int32>, Varint>();
        wrapSerializer<int64, serializeBasic, deserializeBasic<int64>, Varint>();
        wrapSerializer<uint32, serializeBasic, deserializeBasic<uint32>, Varint>();
        wrapSerializer<uint64, serializeBasic, deserializeBasic<uint64>, Varint>();
        wrapSerializer<sint32, serializeBasic, deserializeBasic<sint32>, Varint>();
        wrapSerializer<sint64, serializeBasic, deserializeBasic<sint64>, Varint>();
        wrapSerializer<fixed32, serializeBasic, deserializeBasic<fixed32>, Fixed32>();
        wrapSerializer<fixed64, serializeBasic, deserializeBasic<fixed64>, Fixed64>();
        wrapSerializer<sfixed32, serializeBasic, deserializeBasic<sfixed32>, Fixed32>();
        wrapSerializer<sfixed64, serializeBasic, deserializeBasic<sfixed64>, Fixed64>();
        wrapSerializer<bool, uint32, serializeBasic<uint32>, deserializeBasic<uint32>, Varint>();
        wrapSerializer<QString, serializeBasic, deserializeBasic<QString>, LengthDelimited>();
        wrapSerializer<QByteArray, serializeBasic, deserializeBasic<QByteArray>, LengthDelimited>();

        wrapSerializer<FloatList, serializeListType, deserializeList<float>, LengthDelimited>();
        wrapSerializer<DoubleList, serializeListType, deserializeList<double>, LengthDelimited>();
        wrapSerializer<fixed32List, serializeListType, deserializeList<fixed32>, LengthDelimited>();
        wrapSerializer<fixed64List, serializeListType, deserializeList<fixed64>, LengthDelimited>();
        wrapSerializer<sfixed32List, serializeListType, deserializeList<sfixed32>, LengthDelimited>();
        wrapSerializer<sfixed64List, serializeListType, deserializeList<sfixed64>, LengthDelimited>();
        wrapSerializer<int32List, serializeListType, deserializeList<int32>, LengthDelimited>();
        wrapSerializer<int64List, serializeListType, deserializeList<int64>, LengthDelimited>();
        wrapSerializer<sint32List, serializeListType, deserializeList<sint32>, LengthDelimited>();
        wrapSerializer<sint64List, serializeListType, deserializeList<sint64>, LengthDelimited>();
        wrapSerializer<uint32List, serializeListType, deserializeList<uint32>, LengthDelimited>();
        wrapSerializer<uint64List, serializeListType, deserializeList<uint64>, LengthDelimited>();
        wrapSerializer<QStringList, QStringList, serializeListType<QString>, deserializeList<QString>, LengthDelimited>();
        wrapSerializer<QByteArrayList, serializeListType, deserializeList<QByteArray>, LengthDelimited>();
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
    uint32 length = QProtobufSerializerPrivate::deserializeVarintCommon<uint32>(it);
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


QByteArray QProtobufSerializerPrivate::serializeProperty(const QVariant &propertyValue, const QProtobufMetaProperty &metaProperty)
{
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << metaProperty.protoFieldIndex()
                  << static_cast<QMetaType::Type>(propertyValue.type());

    QByteArray result;
    WireTypes type = UnknownWireType;

    int userType = propertyValue.userType();

    //TODO: replace with some common function
    int fieldIndex = metaProperty.protoFieldIndex();
    auto basicIt = handlers.find(userType);
    if (basicIt != handlers.end()) {
        type = basicIt->second.type;
        result.append(basicIt->second.serializer(propertyValue, fieldIndex));
        if (fieldIndex != QtProtobufPrivate::NotUsedFieldIndex
                && type != UnknownWireType) {
            result.prepend(QProtobufSerializerPrivate::encodeHeader(metaProperty.protoFieldIndex(), type));
        }
    } else {
        auto &handler = QtProtobufPrivate::findHandler(userType);
        handler.serializer(q_ptr, propertyValue, QProtobufMetaProperty(metaProperty, metaProperty.protoFieldIndex()), result);
    }
    return result;
}

void QProtobufSerializerPrivate::deserializeProperty(QObject *object, const QProtobufMetaObject &metaObject, QProtobufSelfcheckIterator &it)
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

    auto propertyNumberIt = metaObject.propertyOrdering.find(fieldNumber);
    if (propertyNumberIt == std::end(metaObject.propertyOrdering)) {
        auto bytesCount = QProtobufSerializerPrivate::skipSerializedFieldBytes(it, wireType);
        qProtoWarning() << "Message received contains unexpected/optional field. WireType:" << wireType
                        << ", field number: " << fieldNumber << "Skipped:" << (bytesCount + 1) << "bytes";
        return;
    }

    int propertyIndex = propertyNumberIt->second;
    QMetaProperty metaProperty = metaObject.staticMetaObject.property(propertyIndex);

    qProtoDebug() << __func__ << " wireType: " << wireType << " metaProperty: " << metaProperty.typeName()
                  << "currentByte:" << QString::number((*it), 16);

    QVariant newPropertyValue;
    newPropertyValue = metaProperty.read(object);
    int userType = metaProperty.userType();

    //TODO: replace with some common function
    auto basicIt = handlers.find(userType);
    if (basicIt != handlers.end()) {
        basicIt->second.deserializer(it, newPropertyValue);
    } else {
        auto &handler = QtProtobufPrivate::findHandler(userType);
        handler.deserializer(q_ptr, it, newPropertyValue);
    }

    metaProperty.write(object, newPropertyValue);
}

void QProtobufSerializerPrivate::deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it)
{
    int mapIndex = 0;
    WireTypes type = WireTypes::UnknownWireType;
    unsigned int count = QProtobufSerializerPrivate::deserializeVarintCommon<uint32>(it);
    qProtoDebug() << __func__ << "count:" << count;
    QProtobufSelfcheckIterator last = it + count;
    while (it != last) {
        QProtobufSerializerPrivate::decodeHeader(it, mapIndex, type);
        if (mapIndex == 1) {
            //Only simple types are supported as keys
            int userType = key.userType();
            auto &handler = handlers.at(userType);//throws if not found
            handler.deserializer(it, key);
        } else {
            //TODO: replace with some common function
            int userType = value.userType();
            auto basicIt = handlers.find(userType);
            if (basicIt != handlers.end()) {
                basicIt->second.deserializer(it, value);
            } else {
                auto &handler = QtProtobufPrivate::findHandler(userType);
                handler.deserializer(q_ptr, it, value);//throws if not implemented
            }
        }
    }
}

QProtobufSerializerPrivate::SerializerRegistry QProtobufSerializerPrivate::handlers = {};
