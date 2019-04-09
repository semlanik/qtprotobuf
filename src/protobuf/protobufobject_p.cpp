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

#include "protobufobject_p.h"

using namespace qtprotobuf;

ProtobufObjectPrivate::SerializerRegistry ProtobufObjectPrivate::serializers = {};

QByteArray ProtobufObjectPrivate::serializeValue(const QObject* object, const QVariant &propertyValue, int fieldIndex, const QMetaProperty &metaProperty)
{
    QByteArray result;
    WireTypes type = UnknownWireType;
    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex << "typeName"
                  << metaProperty.typeName() << static_cast<QMetaType::Type>(propertyValue.type());

    switch (metaProperty.userType()) {
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
    case QMetaType::Bool:
        type = Varint;
        result.append(serializeBasic(propertyValue.toUInt(), fieldIndex));
        break;
    default:
        if(metaProperty.isEnumType()) {
            type = Varint;
            result.append(serializeBasic(propertyValue.toLongLong(), fieldIndex));
        } else {
            result.append(serializeUserType(propertyValue, fieldIndex, type));
        }
        break;
    }
    if (fieldIndex != NotUsedFieldIndex
            && type != UnknownWireType) {
        result.prepend(encodeHeaderByte(fieldIndex, type));
    }
    return result;
}

QByteArray ProtobufObjectPrivate::serializeUserType(const QVariant &propertyValue, int &fieldIndex, WireTypes &type)
{

    qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex;
    type = LengthDelimited;
    int userType = propertyValue.userType();

    //First looking type serializer in registred serializers
    auto it = serializers.find(userType);
    if (it != std::end(serializers)) {
        return (it->second).serializer(propertyValue, fieldIndex);
    }

    //Check if it's special type
    if (userType == qMetaTypeId<float>()) {
        type = Fixed32;
        return serializeBasic(propertyValue.toFloat(), fieldIndex);
    }
    if (userType == qMetaTypeId<double>()) {
        type = Fixed64;
        return serializeBasic(propertyValue.toDouble(), fieldIndex);
    }
    if (userType == qMetaTypeId<int32>()) {
        type = Varint;
        //FIXME: Current implmentation serializes to int64_t bacause of issue in reference implementation
        return serializeBasic(propertyValue.value<int64>(), fieldIndex);
    }
    if (userType == qMetaTypeId<int64>()) {
        type = Varint;
        return serializeBasic(propertyValue.value<int64>(), fieldIndex);
    }
    if (userType == qMetaTypeId<uint32>()) {
        type = Varint;
        return serializeBasic(propertyValue.value<uint32>(), fieldIndex);
    }
    if (userType == qMetaTypeId<uint64>()) {
        type = Varint;
        return serializeBasic(propertyValue.value<uint64>(), fieldIndex);
    }
    if (userType == qMetaTypeId<sint32>()) {
        type = Varint;
        return serializeBasic(propertyValue.value<sint32>(), fieldIndex);
    }
    if (userType == qMetaTypeId<sint64>()) {
        type = Varint;
        return serializeBasic(propertyValue.value<sint64>(), fieldIndex);
    }
    if (userType == qMetaTypeId<fint32>()) {
        type = Fixed32;
        return serializeBasic(propertyValue.value<fint32>(), fieldIndex);
    }
    if (userType == qMetaTypeId<fint64>()) {
        type = Fixed64;
        return serializeBasic(propertyValue.value<fint64>(), fieldIndex);
    }
    if (userType == qMetaTypeId<sfint32>()) {
        type = Fixed32;
        return serializeBasic(propertyValue.value<sfint32>(), fieldIndex);
    }
    if (userType == qMetaTypeId<sfint64>()) {
        type = Fixed64;
        return serializeBasic(propertyValue.value<sfint64>(), fieldIndex);
    }
    if (userType == qMetaTypeId<fint32List>()) {
        return serializeListType(propertyValue.value<fint32List>(), fieldIndex);
    }
    if (userType == qMetaTypeId<fint64List>()) {
        return serializeListType(propertyValue.value<fint64List>(), fieldIndex);
    }
    if (userType == qMetaTypeId<sfint32List>()) {
        return serializeListType(propertyValue.value<sfint32List>(), fieldIndex);
    }
    if (userType == qMetaTypeId<sfint64List>()) {
        return serializeListType(propertyValue.value<sfint64List>(), fieldIndex);
    }
    if (userType == qMetaTypeId<int32List>()) {
        return serializeListType(propertyValue.value<int32List>(), fieldIndex);
    }
    if (userType == qMetaTypeId<int64List>()) {
        return serializeListType(propertyValue.value<int64List>(), fieldIndex);
    }
    if (userType == qMetaTypeId<sint32List>()) {
        return serializeListType(propertyValue.value<sint32List>(), fieldIndex);
    }
    if (userType == qMetaTypeId<sint64List>()) {
        return serializeListType(propertyValue.value<sint64List>(), fieldIndex);
    }
    if (userType == qMetaTypeId<uint32List>()) {
        return serializeListType(propertyValue.value<uint32List>(), fieldIndex);
    }
    if (userType == qMetaTypeId<uint64List>()) {
        return serializeListType(propertyValue.value<uint64List>(), fieldIndex);
    }
    if (userType == qMetaTypeId<FloatList>()) {
        return serializeListType(propertyValue.value<FloatList>(), fieldIndex);
    }
    if (userType == qMetaTypeId<DoubleList>()) {
        return serializeListType(propertyValue.value<DoubleList>(), fieldIndex);
    }

    auto serializer = serializers[userType].serializer;
    Q_ASSERT_X(serializer, "ProtobufObjectPrivate", "Serialization of unknown type is impossible");
    return serializer(propertyValue, fieldIndex);
}

void ProtobufObjectPrivate::deserializeProperty(QObject *object, WireTypes wireType, const QMetaProperty &metaProperty, QByteArray::const_iterator &it)
{
    QLatin1Literal typeName(metaProperty.typeName());
    qProtoDebug() << __func__ << " wireType: " << wireType << " metaProperty: " << typeName << "currentByte:" << QString::number((*it), 16);
    QVariant newPropertyValue;
    switch (metaProperty.userType()) {
    case QMetaType::QString:
        newPropertyValue = QString::fromUtf8(deserializeLengthDelimited(it));
        break;
    case QMetaType::QByteArray:
        newPropertyValue = deserializeLengthDelimited(it);
        break;
    case QMetaType::QByteArrayList: {
        QByteArrayList currentValue = metaProperty.read(object).value<QByteArrayList>();
        currentValue.append(deserializeLengthDelimited(it));
        metaProperty.write(object, QVariant::fromValue<QByteArrayList>(currentValue));
    }
        return;
    case QMetaType::QStringList: {
        QStringList currentValue = metaProperty.read(object).value<QStringList>();
        currentValue.append(QString::fromUtf8(deserializeLengthDelimited(it)));
        metaProperty.write(object, currentValue);
    }
        return;
    case QMetaType::Bool:
        newPropertyValue = deserializeBasic<uint32>(it);
        break;
    default:
        if (metaProperty.isEnumType()) {
            newPropertyValue = deserializeBasic<int32>(it);
        } else {
            newPropertyValue = metaProperty.read(object);
            deserializeUserType(metaProperty, it, newPropertyValue);
        }
        break;
    }
    metaProperty.write(object, newPropertyValue);
}

void ProtobufObjectPrivate::deserializeUserType(const QMetaProperty &metaType, QByteArray::const_iterator& it, QVariant &newValue)
{
    int userType = metaType.userType();
    qProtoDebug() << __func__ << "userType" << userType << "typeName" << metaType.typeName() << "currentByte:" << QString::number((*it), 16);

    auto serializerIt = serializers.find(userType);
    if (serializerIt != std::end(serializers)) {
        (serializerIt->second).deserializer(it, newValue);
        return;
    }

    if (userType == qMetaTypeId<float>()) {
        newValue = deserializeBasic<float>(it);
    } else if (userType == qMetaTypeId<double>()) {
        newValue = deserializeBasic<double>(it);
    } else if (userType == qMetaTypeId<int32>()) {
        newValue = deserializeBasic<int32>(it);
    } else if (userType == qMetaTypeId<int64>()) {
        newValue = deserializeBasic<int64>(it);
    } else if (userType == qMetaTypeId<sint32>()) {
        newValue = deserializeBasic<sint32>(it);
    } else if (userType == qMetaTypeId<sint64>()) {
        newValue = deserializeBasic<sint64>(it);
    } else if (userType == qMetaTypeId<uint32>()) {
        newValue = deserializeBasic<uint32>(it);
    } else if (userType == qMetaTypeId<uint64>()) {
        newValue = deserializeBasic<uint64>(it);
    } else if (userType == qMetaTypeId<fint32>()) {
        newValue = deserializeBasic<fint32>(it);
    } else if (userType == qMetaTypeId<fint32>()) {
        newValue = deserializeBasic<fint32>(it);
    } else if (userType == qMetaTypeId<fint64>()) {
        newValue = deserializeBasic<fint64>(it);
    } else if (userType == qMetaTypeId<sfint32>()) {
        newValue = deserializeBasic<sfint32>(it);
    } else if (userType == qMetaTypeId<sfint64>()) {
        newValue = deserializeBasic<sfint64>(it);
    } else if (userType == qMetaTypeId<fint32List>()) {
        newValue = deserializeListType<fint32>(it);
    } else if (userType == qMetaTypeId<fint64List>()) {
        newValue = deserializeListType<fint64>(it);
    } else if(userType == qMetaTypeId<sfint32List>()) {
        newValue = deserializeListType<sfint32>(it);
    } else if(userType == qMetaTypeId<sfint64List>()) {
        newValue = deserializeListType<sfint64>(it);
    } else if (userType == qMetaTypeId<int32List>()) {
        newValue = deserializeListType<int32>(it);
    } else if (userType == qMetaTypeId<int64List>()) {
        newValue = deserializeListType<int64>(it);
    } else if (userType == qMetaTypeId<sint32List>()) {
        newValue = deserializeListType<sint32>(it);
    } else if (userType == qMetaTypeId<sint64List>()) {
        newValue = deserializeListType<sint64>(it);
    } else if (userType == qMetaTypeId<uint32List>()) {
        newValue = deserializeListType<uint32>(it);
    } else if (userType == qMetaTypeId<uint64List>()) {
        newValue = deserializeListType<uint64>(it);
    } else if (userType == qMetaTypeId<FloatList>()) {
        newValue = deserializeListType<float>(it);
    } else if (userType == qMetaTypeId<DoubleList>()) {
        newValue = deserializeListType<double>(it);
    }
}
