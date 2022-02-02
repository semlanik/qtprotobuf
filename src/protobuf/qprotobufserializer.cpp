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

QT_BEGIN_NAMESPACE

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

QProtobufSerializer::QProtobufSerializer() : dPtr(new QProtobufSerializerPrivate(this))
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
        QVariant propertyValue = object->property(propertyName);
        result.append(dPtr->serializeProperty(propertyValue, QProtobufMetaProperty(metaProperty,
                                                                                   fieldIndex,
                                                                                   field.second)));
    }

    return result;
}

void QProtobufSerializer::deserializeMessage(QObject *object, const QProtobufMetaObject &metaObject, const QByteArray &data) const
{
    for (QProtobufSelfcheckIterator it(data); it != data.end();) {
        dPtr->deserializeProperty(object, metaObject, it);
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

bool QProtobufSerializer::deserializeListObject(QObject *object, const QProtobufMetaObject &metaObject, QProtobufSelfcheckIterator &it) const
{
    deserializeObject(object, metaObject, it);
    return true;
}

QByteArray QProtobufSerializer::serializeMapPair(const QVariant &key, const QVariant &value, const QProtobufMetaProperty &metaProperty) const
{
    QByteArray result = QProtobufSerializerPrivate::encodeHeader(metaProperty.protoFieldIndex(), LengthDelimited);
    result.append(QProtobufSerializerPrivate::prependLengthDelimitedSize(
                      dPtr->serializeProperty(key, QProtobufMetaProperty(metaProperty, 1, QString())) +
                      dPtr->serializeProperty(value, QProtobufMetaProperty(metaProperty, 2, QString()))));
    return result;
}

bool QProtobufSerializer::deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it) const
{
    dPtr->deserializeMapPair(key, value, it);
    return true;
}

QByteArray QProtobufSerializer::serializeEnum(QtProtobuf::int64 value, const QMetaEnum &/*metaEnum*/, const QProtobufMetaProperty &metaProperty) const
{
    WireTypes type = Varint;
    int fieldIndex = metaProperty.protoFieldIndex();
    QByteArray result = QProtobufSerializerPrivate::serializeBasic<QtProtobuf::int64>(value, fieldIndex);
    if (fieldIndex != QtProtobufPrivate::NotUsedFieldIndex) {
        result.prepend(QProtobufSerializerPrivate::encodeHeader(metaProperty.protoFieldIndex(), type));
    }
    return result;
}

QByteArray QProtobufSerializer::serializeEnumList(const QList<QtProtobuf::int64> &value, const QMetaEnum &/*metaEnum*/, const QProtobufMetaProperty &metaProperty) const
{
    WireTypes type = LengthDelimited;
    int fieldIndex = metaProperty.protoFieldIndex();
    QByteArray result = QProtobufSerializerPrivate::serializeListType<QtProtobuf::int64>(value, fieldIndex);
    if (fieldIndex != QtProtobufPrivate::NotUsedFieldIndex) {
        result.prepend(QProtobufSerializerPrivate::encodeHeader(metaProperty.protoFieldIndex(), type));
    }
    return result;
}

void QProtobufSerializer::deserializeEnum(QtProtobuf::int64 &value, const QMetaEnum &/*metaEnum*/, QProtobufSelfcheckIterator &it) const
{
    QVariant variantValue;
    QProtobufSerializerPrivate::deserializeBasic<QtProtobuf::int64>(it, variantValue);
    value = variantValue.value<QtProtobuf::int64>();
}

void QProtobufSerializer::deserializeEnumList(QList<QtProtobuf::int64> &value, const QMetaEnum &/*metaEnum*/, QProtobufSelfcheckIterator &it) const
{
    QVariant variantValue;
    QProtobufSerializerPrivate::deserializeList<QtProtobuf::int64>(it, variantValue);
    value = variantValue.value<QList<QtProtobuf::int64>>();
}

QProtobufSerializerPrivate::QProtobufSerializerPrivate(QProtobufSerializer *q) : q_ptr(q)
{
    //if handlers is not empty intialization already done
    if (handlers.empty()) {
        wrapSerializer<float, serializeBasic, deserializeBasic<float>, Fixed32>();
        wrapSerializer<double, serializeBasic, deserializeBasic<double>, Fixed64>();
        wrapSerializer<QtProtobuf::int32, serializeBasic, deserializeBasic<QtProtobuf::int32>, Varint>();
        wrapSerializer<QtProtobuf::int64, serializeBasic, deserializeBasic<QtProtobuf::int64>, Varint>();
        wrapSerializer<QtProtobuf::uint32, serializeBasic, deserializeBasic<QtProtobuf::uint32>, Varint>();
        wrapSerializer<QtProtobuf::uint64, serializeBasic, deserializeBasic<QtProtobuf::uint64>, Varint>();
        wrapSerializer<QtProtobuf::sint32, serializeBasic, deserializeBasic<QtProtobuf::sint32>, Varint>();
        wrapSerializer<QtProtobuf::sint64, serializeBasic, deserializeBasic<QtProtobuf::sint64>, Varint>();
        wrapSerializer<QtProtobuf::fixed32, serializeBasic, deserializeBasic<QtProtobuf::fixed32>, Fixed32>();
        wrapSerializer<QtProtobuf::fixed64, serializeBasic, deserializeBasic<QtProtobuf::fixed64>, Fixed64>();
        wrapSerializer<QtProtobuf::sfixed32, serializeBasic, deserializeBasic<QtProtobuf::sfixed32>, Fixed32>();
        wrapSerializer<QtProtobuf::sfixed64, serializeBasic, deserializeBasic<QtProtobuf::sfixed64>, Fixed64>();
        wrapSerializer<bool, QtProtobuf::uint32, serializeBasic<QtProtobuf::uint32>, deserializeBasic<QtProtobuf::uint32>, Varint>();
        wrapSerializer<QString, serializeBasic, deserializeBasic<QString>, LengthDelimited>();
        wrapSerializer<QByteArray, serializeBasic, deserializeBasic<QByteArray>, LengthDelimited>();

        wrapSerializer<QtProtobuf::FloatList, serializeListType, deserializeList<float>, LengthDelimited>();
        wrapSerializer<QtProtobuf::DoubleList, serializeListType, deserializeList<double>, LengthDelimited>();
        wrapSerializer<QtProtobuf::fixed32List, serializeListType, deserializeList<QtProtobuf::fixed32>, LengthDelimited>();
        wrapSerializer<QtProtobuf::fixed64List, serializeListType, deserializeList<QtProtobuf::fixed64>, LengthDelimited>();
        wrapSerializer<QtProtobuf::sfixed32List, serializeListType, deserializeList<QtProtobuf::sfixed32>, LengthDelimited>();
        wrapSerializer<QtProtobuf::sfixed64List, serializeListType, deserializeList<QtProtobuf::sfixed64>, LengthDelimited>();
        wrapSerializer<QtProtobuf::int32List, serializeListType, deserializeList<QtProtobuf::int32>, LengthDelimited>();
        wrapSerializer<QtProtobuf::int64List, serializeListType, deserializeList<QtProtobuf::int64>, LengthDelimited>();
        wrapSerializer<QtProtobuf::sint32List, serializeListType, deserializeList<QtProtobuf::sint32>, LengthDelimited>();
        wrapSerializer<QtProtobuf::sint64List, serializeListType, deserializeList<QtProtobuf::sint64>, LengthDelimited>();
        wrapSerializer<QtProtobuf::uint32List, serializeListType, deserializeList<QtProtobuf::uint32>, LengthDelimited>();
        wrapSerializer<QtProtobuf::uint64List, serializeListType, deserializeList<QtProtobuf::uint64>, LengthDelimited>();
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
    QtProtobuf::uint32 length = QProtobufSerializerPrivate::deserializeVarintCommon<QtProtobuf::uint32>(it);
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
        it += sizeof(decltype(QtProtobuf::fixed32::_t));
        break;
    case WireTypes::Fixed64:
        it += sizeof(decltype(QtProtobuf::fixed64::_t));
        break;
    case WireTypes::LengthDelimited:
        skipLengthDelimited(it);
        break;
    case WireTypes::UnknownWireType:
    default:
        QT_THROW(std::invalid_argument("Cannot skip due to undefined length of the redundant field."));
    }

    return std::distance(initialIt, QByteArray::const_iterator(it));
}


QByteArray QProtobufSerializerPrivate::serializeProperty(const QVariant &propertyValue, const QProtobufMetaProperty &metaProperty)
{
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << metaProperty.protoFieldIndex()
                  << static_cast<QMetaType::Type>(propertyValue.typeId());

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
        auto handler = QtProtobufPrivate::findHandler(userType);
        handler.serializer(q_ptr, propertyValue, metaProperty, result);
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
        QT_THROW(std::invalid_argument("Message received doesn't contains valid header byte. "
                              "Seems stream is broken"));
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
        auto handler = QtProtobufPrivate::findHandler(userType);
        handler.deserializer(q_ptr, it, newPropertyValue);
    }

    metaProperty.write(object, newPropertyValue);
}

void QProtobufSerializerPrivate::deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it)
{
    int mapIndex = 0;
    WireTypes type = WireTypes::UnknownWireType;
    unsigned int count = QProtobufSerializerPrivate::deserializeVarintCommon<QtProtobuf::uint32>(it);
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
                auto handler = QtProtobufPrivate::findHandler(userType);
                handler.deserializer(q_ptr, it, value);//throws if not implemented
            }
        }
    }
}

QProtobufSerializerPrivate::SerializerRegistry QProtobufSerializerPrivate::handlers = {};

QT_END_NAMESPACE