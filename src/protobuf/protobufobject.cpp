/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
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

#include "protobufobject.h"

using namespace qtprotobuf;

ProtobufObjectPrivate::SerializerRegistry ProtobufObjectPrivate::serializers = {};


QByteArray ProtobufObjectPrivate::serializeValue(const QVariant& propertyValue, int fieldIndex, const QLatin1Literal& typeName) const {
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex << "typeName" << typeName;
    QByteArray result;
    WireTypes type = UnknownWireType;
    int resultSize = result.size();
    switch (static_cast<QMetaType::Type>(propertyValue.type())) {
    case QMetaType::Int:
        type = Varint;
        if (typeName == "qtprotobuf::sint32"
                || typeName == "sint32") {
            result.append(serializeVarintZigZag(propertyValue.toInt()));
        } else if (typeName == "qtprotobuf::sfint32"
                   || typeName == "sfint32") {
            type = Fixed32;
            result.append(serializeFixed(propertyValue.toInt()));
        } else {
            result.append(serializeVarint(propertyValue.toLongLong()));
        }
        if (resultSize == result.size()) {
            fieldIndex = NotUsedFieldIndex;
        }
        break;
    case QMetaType::LongLong:
        type = Varint;
        if (typeName == "qtprotobuf::sint64"
                || typeName == "sint64") {
            result.append(serializeVarintZigZag(propertyValue.toLongLong()));
        } else if (typeName == "qtprotobuf::sfint64"
                   || typeName == "sfint64") {
            type = Fixed64;
            result.append(serializeFixed(propertyValue.toLongLong()));
        } else {
            result.append(serializeVarint(propertyValue.toLongLong()));
        }
        if (resultSize == result.size()) {
            fieldIndex = NotUsedFieldIndex;
        }
        break;
    case QMetaType::Float:
        type = Fixed32;
        result.append(serializeFixed(propertyValue.toFloat()));
        break;
    case QMetaType::Double:
        type = Fixed64;
        result.append(serializeFixed(propertyValue.toDouble()));
        break;
    case QMetaType::QString:
        type = LengthDelimited;
        result.append(serializeLengthDelimited(propertyValue.toString().toUtf8()));
        break;
    case QMetaType::QByteArray:
        type = LengthDelimited;
        result.append(serializeLengthDelimited(propertyValue.toByteArray()));
        break;
    case QMetaType::QStringList:
        type = LengthDelimited;
        result.append(serializeListType(propertyValue.toStringList(), fieldIndex));
        break;
    case QMetaType::QByteArrayList:
        type = LengthDelimited;
        result.append(serializeListType(propertyValue.value<QByteArrayList>(), fieldIndex));
        break;
    case QMetaType::User:
        type = LengthDelimited;
        result.append(serializeUserType(propertyValue, fieldIndex));
        break;
    case QMetaType::UInt:
        if (typeName == "qtprotobuf::fint32"
                || typeName == "fint32") {
            type = Fixed32;
            result.append(serializeFixed(propertyValue.toUInt()));
        } else {
            type = Varint;
            result.append(serializeVarint(propertyValue.toUInt()));
        }
        if (resultSize == result.size()) {
            fieldIndex = NotUsedFieldIndex;
        }
        break;
    case QMetaType::ULongLong:
        if (typeName == "qtprotobuf::fint64"
                || typeName == "fint64") {
            type = Fixed64;
            result.append(serializeFixed(propertyValue.toULongLong()));
        } else {
            type = Varint;
            result.append(serializeVarint(propertyValue.toULongLong()));
        }
        if (resultSize == result.size()) {
            fieldIndex = NotUsedFieldIndex;
        }
        break;
    case QMetaType::Bool:
        type = Varint;
        result.append(serializeVarint(propertyValue.toUInt()));
        if (resultSize == result.size()) {
            fieldIndex = NotUsedFieldIndex;
        }
        break;
    default:
        Q_ASSERT_X(false, staticMetaObject.className(), "Serialization of unknown type is impossible");
    }
    if (fieldIndex != NotUsedFieldIndex
            && type != UnknownWireType) {
        result.prepend(encodeHeaderByte(fieldIndex, type));
    }
    return result;
}

QByteArray ProtobufObjectPrivate::serializeUserType(const QVariant &propertyValue, int &fieldIndex) const {
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex;
    int userType = propertyValue.userType();

    //First looking type serializer in registred serializers
    auto it = serializers.find(userType);
    if (it != std::end(serializers)) {
        return (it->second).serializer(this, propertyValue, fieldIndex);
    }

    //Check if it's special list
    if (userType == qMetaTypeId<int32List>()) {
        return serializeListType(propertyValue.value<int32List>(), fieldIndex);
    }

    if(userType == qMetaTypeId<FloatList>()) {
        return serializeListType(propertyValue.value<FloatList>(), fieldIndex);
    }

    if(userType == qMetaTypeId<DoubleList>()) {
        return serializeListType(propertyValue.value<DoubleList>(), fieldIndex);
    }

    //Otherwise it's user type
    const void *src = propertyValue.constData();
    //TODO: each time huge objects will make own copies
    //Probably generate fields reflection is better solution
    auto value = std::unique_ptr<ProtobufObjectPrivate>(reinterpret_cast<ProtobufObjectPrivate *>(QMetaType::create(userType, src)));
    return serializeLengthDelimited(value->serializePrivate());
}

void ProtobufObjectPrivate::deserializeProperty(WireTypes wireType, const QMetaProperty &metaProperty, QByteArray::const_iterator &it)
{
    QLatin1Literal typeName(metaProperty.typeName());
    qProtoDebug() << __func__ << " wireType: " << wireType << " metaProperty: " << typeName << "currentByte:" << QString::number((*it), 16);
    QVariant newPropertyValue;
    int type = metaProperty.type();
    switch(type) {
    case QMetaType::UInt:
        if (wireType == Fixed32) {
            newPropertyValue = deserializeFixed<fint32>(it);
        } else {
            newPropertyValue = deserializeVarint<uint32>(it);
        }
        break;
    case QMetaType::ULongLong:
        if (wireType == Fixed64) {
            newPropertyValue = deserializeFixed<fint64>(it);
        } else {
            newPropertyValue = deserializeVarint<uint64>(it);
        }
        break;
    case QMetaType::Float:
        newPropertyValue = deserializeFixed<float>(it);
        break;
    case QMetaType::Double:
        newPropertyValue = deserializeFixed<double>(it);
        break;
    case QMetaType::Int:
        if (wireType == Fixed32) {
            newPropertyValue = deserializeFixed<sfint32>(it);
        } else if (typeName == "qtprotobuf::sint32"
                   || typeName == "sint32") {
            newPropertyValue = deserializeVarintZigZag<sint32>(it);
        } else {
            newPropertyValue = deserializeVarint<int64>(it);
        }
        break;
    case QMetaType::LongLong:
        if (wireType == Fixed64) {
            newPropertyValue = deserializeFixed<sfint64>(it);
        } else if (typeName == "qtprotobuf::sint64"
                   || typeName == "sint64") {
            newPropertyValue = deserializeVarintZigZag<sint64>(it);
        } else {
            newPropertyValue = deserializeVarint<int64>(it);
        }
        break;
    case QMetaType::QString:
        newPropertyValue = QString::fromUtf8(deserializeLengthDelimited(it));
        break;
    case QMetaType::QByteArray:
        newPropertyValue = deserializeLengthDelimited(it);
        break;
    case QMetaType::User:
        newPropertyValue = metaProperty.read(this);
        deserializeUserType(metaProperty, it, newPropertyValue);
        break;
    case QMetaType::QByteArrayList: {
        QByteArrayList currentValue = metaProperty.read(this).value<QByteArrayList>();
        currentValue.append(deserializeLengthDelimited(it));
        metaProperty.write(this, QVariant::fromValue<QByteArrayList>(currentValue));
    }
        return;
    case QMetaType::QStringList: {
        QStringList currentValue = metaProperty.read(this).value<QStringList>();
        currentValue.append(QString::fromUtf8(deserializeLengthDelimited(it)));
        metaProperty.write(this, currentValue);
    }
        return;
    case QMetaType::Bool:
        newPropertyValue = deserializeVarint<uint32>(it);
        break;
    default:
        break;
    }
    metaProperty.write(this, newPropertyValue);
}

void ProtobufObjectPrivate::deserializeUserType(const QMetaProperty &metaType, QByteArray::const_iterator& it, QVariant &newValue)
{
    int userType = metaType.userType();
    QLatin1Literal typeName(metaType.typeName());
    qProtoDebug() << __func__ << "userType" << userType << "currentByte:" << QString::number((*it), 16);

    auto serializerIt = serializers.find(userType);
    if (serializerIt != std::end(serializers)) {
        (serializerIt->second).deserializer(this, it, newValue);
        return;
    }

    if (userType == qMetaTypeId<int32List>()) {
        if (typeName == "sint32List"
                || typeName == "qtprotobuf::sint32List") {
            newValue = deserializeVarintListTypeZigZag<int32>(it);
        } else {
            newValue = deserializeVarintListType<int32>(it);
        }
    } else if (userType == qMetaTypeId<int64List>()) {
        if (typeName == "sint64List"
                || typeName == "qtprotobuf::sint64List") {
            newValue = deserializeVarintListTypeZigZag<int64>(it);
        } else {
            newValue = deserializeVarintListType<int64>(it);
        }
    } else if (userType == qMetaTypeId<uint32List>()) {
        //TODO: Check if type is fixed
        newValue = deserializeVarintListType<uint32>(it);
    } else if (userType == qMetaTypeId<uint64List>()) {
        //TODO: Check if type is fixed
        newValue = deserializeVarintListType<uint64>(it);
    } else if(userType == qMetaTypeId<FloatList>()) {
        newValue = deserializeListType<float>(it);
    } else if(userType == qMetaTypeId<DoubleList>()) {
        newValue = deserializeListType<double>(it);
    } else {
        newValue = deserializeProtobufObjectType(userType, it);
    }
}
