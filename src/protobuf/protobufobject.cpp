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

namespace {
    static const char *sint32TypeNameP = "qtprotobuf::sint32";
    static const char *sint32TypeName = "sint32";

    static const char *sint64TypeNameP = "qtprotobuf::sint64";
    static const char *sint64TypeName = "sint64";

    static const char *sint32ListTypeNameP = "qtprotobuf::sint32List";
    static const char *sint32ListTypeName = "sint32List";

    static const char *sint64ListTypeNameP = "qtprotobuf::sint64List";
    static const char *sint64ListTypeName = "sint64List";

    static const char *fint32TypeNameP = "qtprotobuf::fint32";
    static const char *fint32TypeName = "fint32";

    static const char *fint64TypeNameP = "qtprotobuf::fint64";
    static const char *fint64TypeName = "fint64";

    static const char *sfint32TypeNameP = "qtprotobuf::sfint32";
    static const char *sfint32TypeName = "sfint32";

    static const char *sfint64TypeNameP = "qtprotobuf::sfint64";
    static const char *sfint64TypeName = "sfint64";

    static const char *fint32ListTypeNameP = "qtprotobuf::fint32List";
    static const char *fint32ListTypeName = "fint32List";

    static const char *fint64ListTypeNameP = "qtprotobuf::fint64List";
    static const char *fint64ListTypeName = "fint64List";

    static const char *sfint32ListTypeNameP = "qtprotobuf::sfint32List";
    static const char *sfint32ListTypeName = "sfint32List";

    static const char *sfint64ListTypeNameP = "qtprotobuf::sfint64List";
    static const char *sfint64ListTypeName = "sfint64List";
}

QByteArray ProtobufObjectPrivate::serializeValue(const QVariant &propertyValue, int fieldIndex, const QMetaProperty &metaProperty)
{
    QLatin1Literal typeName(metaProperty.typeName());
    QByteArray result;
    WireTypes type = UnknownWireType;
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex << "typeName"
                  << typeName << static_cast<QMetaType::Type>(propertyValue.type());

    switch (static_cast<QMetaType::Type>(propertyValue.type())) {
    case QMetaType::Int:
        type = Varint;
        if (typeName == sint32TypeNameP
                || typeName == sint32TypeName) {
            result.append(serializeVarintZigZag(propertyValue.toInt()));
        } else if (typeName == sfint32TypeNameP
                   || typeName == sfint32TypeName) {
            type = Fixed32;
            result.append(serializeFixed(propertyValue.toInt()));
        } else {
            result.append(serializeVarint(propertyValue.toLongLong()));
        }
        if (0 == result.size()) {
            fieldIndex = NotUsedFieldIndex;
        }
        break;
    case QMetaType::LongLong:
        type = Varint;
        if (typeName == sint64TypeNameP
                || typeName == sint64TypeName) {
            result.append(serializeVarintZigZag(propertyValue.toLongLong()));
        } else if (typeName == sfint64TypeNameP
                   || typeName == sfint64TypeName) {
            type = Fixed64;
            result.append(serializeFixed(propertyValue.toLongLong()));
        } else {
            result.append(serializeVarint(propertyValue.toLongLong()));
        }
        if (0 == result.size()) {
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
        result.append(serializeLengthDelimited(propertyValue.toString()));
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
        if(metaProperty.isEnumType()) {
            type = Varint;
            result.append(serializeVarint(propertyValue.toLongLong()));
        } else {
            type = LengthDelimited;
            result.append(serializeUserType(propertyValue, fieldIndex, typeName));
        }
        break;
    case QMetaType::UInt:
        if (typeName == fint32TypeNameP
                || typeName == fint32TypeName) {
            type = Fixed32;
            result.append(serializeFixed(propertyValue.toUInt()));
        } else {
            type = Varint;
            result.append(serializeVarint(propertyValue.toUInt()));
        }
        if (0 == result.size()) {
            fieldIndex = NotUsedFieldIndex;
        }
        break;
    case QMetaType::ULongLong:
        if (typeName == fint64TypeNameP
                || typeName == fint64TypeName) {
            type = Fixed64;
            result.append(serializeFixed(propertyValue.toULongLong()));
        } else {
            type = Varint;
            result.append(serializeVarint(propertyValue.toULongLong()));
        }
        if (0 == result.size()) {
            fieldIndex = NotUsedFieldIndex;
        }
        break;
    case QMetaType::Bool:
        type = Varint;
        result.append(serializeVarint(propertyValue.toUInt()));
        if (0 == result.size()) {
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

QByteArray ProtobufObjectPrivate::serializeUserType(const QVariant &propertyValue, int &fieldIndex, const QLatin1Literal &typeName)
{
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex;
    int userType = propertyValue.userType();

    //First looking type serializer in registred serializers
    auto it = serializers.find(userType);
    if (it != std::end(serializers)) {
        return (it->second).serializer(propertyValue, fieldIndex);
    }

    //Check if it's special list
    if (userType == qMetaTypeId<int32List>()) {
        if (typeName == sint32ListTypeNameP
                || typeName == sint32ListTypeName) {
            return serializeListTypeZigZag(propertyValue.value<sint32List>(), fieldIndex);
        }
        if (typeName == sfint32ListTypeNameP
                || typeName == sfint32ListTypeName) {
            return serializeFixedListType(propertyValue.value<sfint32List>(), fieldIndex);
        }
        return serializeListType(propertyValue.value<int32List>(), fieldIndex);
    }

    if (userType == qMetaTypeId<uint32List>()) {
        if (typeName == fint32ListTypeNameP
                || typeName == fint32ListTypeName) {
            return serializeFixedListType(propertyValue.value<fint32List>(), fieldIndex);
        }
        return serializeListType(propertyValue.value<uint32List>(), fieldIndex);
    }

    if (userType == qMetaTypeId<int64List>()) {
        if (typeName == sint64ListTypeNameP
                || typeName == sint64ListTypeName) {
            return serializeListTypeZigZag(propertyValue.value<sint64List>(), fieldIndex);
        }
        if (typeName == sfint64ListTypeNameP
                || typeName == sfint64ListTypeName) {
            return serializeFixedListType(propertyValue.value<sfint64List>(), fieldIndex);
        }
        return serializeListType(propertyValue.value<int64List>(), fieldIndex);
    }

    if (userType == qMetaTypeId<uint64List>()) {
        if (typeName == fint64ListTypeNameP
                || typeName == fint64ListTypeName) {
            return serializeFixedListType(propertyValue.value<fint64List>(), fieldIndex);
        }
        return serializeListType(propertyValue.value<uint64List>(), fieldIndex);
    }

    if (userType == qMetaTypeId<FloatList>()) {
        return serializeListType(propertyValue.value<FloatList>(), fieldIndex);
    }

    if (userType == qMetaTypeId<DoubleList>()) {
        return serializeListType(propertyValue.value<DoubleList>(), fieldIndex);
    }

    return serializers[userType].serializer(propertyValue, fieldIndex);
}

void ProtobufObjectPrivate::deserializeProperty(WireTypes wireType, const QMetaProperty &metaProperty, QByteArray::const_iterator &it)
{
    QLatin1Literal typeName(metaProperty.typeName());
    qProtoDebug() << __func__ << " wireType: " << wireType << " metaProperty: " << typeName << "currentByte:" << QString::number((*it), 16);
    QVariant newPropertyValue;
    int type = metaProperty.type();
    switch (type) {
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
        } else if (typeName == sint32TypeNameP
                   || typeName == sint32TypeName) {
            newPropertyValue = deserializeVarintZigZag<sint32>(it);
        } else {
            newPropertyValue = deserializeVarint<int64>(it);
        }
        break;
    case QMetaType::LongLong:
        if (wireType == Fixed64) {
            newPropertyValue = deserializeFixed<sfint64>(it);
        } else if (typeName == sint64TypeNameP
                   || typeName == sint64TypeName) {
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
        if (metaProperty.isEnumType()) {
            newPropertyValue = deserializeVarint<int32>(it);
        } else {
            newPropertyValue = metaProperty.read(this);
            deserializeUserType(metaProperty, it, newPropertyValue);
        }
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
        if (typeName == sint32ListTypeNameP
                || typeName == sint32ListTypeName) {
            newValue = deserializeVarintListTypeZigZag<int32>(it);
        } else if (typeName == sfint32ListTypeNameP
                   || typeName == sfint32ListTypeName) {
            newValue = deserializeListType<sfint32>(it);
        } else {
            newValue = deserializeVarintListType<int32>(it);
        }
    } else if (userType == qMetaTypeId<int64List>()) {
        if (typeName == sint64ListTypeNameP
                || typeName == sint64ListTypeName) {
            newValue = deserializeVarintListTypeZigZag<int64>(it);
        } else if (typeName == sfint64ListTypeNameP
                   || typeName == sfint64ListTypeName) {
            newValue = deserializeListType<sfint64>(it);
        } else {
            newValue = deserializeVarintListType<int64>(it);
        }
    } else if (userType == qMetaTypeId<uint32List>()) {
        if (typeName == fint32ListTypeNameP
                           || typeName == fint32ListTypeName) {
            newValue = deserializeListType<fint32>(it);
        } else {
            newValue = deserializeVarintListType<uint32>(it);
        }
    } else if (userType == qMetaTypeId<uint64List>()) {
        if (typeName == fint64ListTypeNameP
                           || typeName == fint64ListTypeName) {
            newValue = deserializeListType<fint64>(it);
        } else {
            newValue = deserializeVarintListType<uint64>(it);
        }
    } else if (userType == qMetaTypeId<FloatList>()) {
        newValue = deserializeListType<float>(it);
    } else if (userType == qMetaTypeId<DoubleList>()) {
        newValue = deserializeListType<double>(it);
    }
}
