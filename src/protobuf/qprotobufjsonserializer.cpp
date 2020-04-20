/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>
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

#include "qprotobufjsonserializer.h"
#include "qprotobufmetaobject.h"
#include "qprotobufmetaproperty.h"
#include "qtprotobuflogging.h"

#include <microjson.h>

#include <QMetaProperty>

using namespace QtProtobuf;

namespace QtProtobuf {

//! \private
class QProtobufJsonSerializerPrivate final
{
    Q_DISABLE_COPY_MOVE(QProtobufJsonSerializerPrivate)
public:
    using Serializer = std::function<QByteArray(const QVariant&)>;
    using Deserializer = std::function<QVariant(QByteArray)>;

    struct SerializationHandlers {
        Serializer serializer; /*!< serializer assigned to class */
        Deserializer deserializer;/*!< deserializer assigned to class */
    };

    using SerializerRegistry = std::unordered_map<int/*metatypeid*/, SerializationHandlers>;

    static QByteArray serializeFloat(const QVariant &propertyValue) {
        bool ok = false;
        float value = propertyValue.toFloat(&ok);
        if (!ok) {
            return QByteArray("NaN");
        }
        return QString::number(static_cast<double>(value), 'g').toUtf8();
    }

    static QByteArray serializeString(const QVariant &propertyValue) {
        return QString("\"%1\"").arg(propertyValue.toString()).toUtf8() ;
    }

    static QByteArray serializeBytes(const QVariant &propertyValue) {
        return QByteArray("\"") + propertyValue.toByteArray().toBase64() + "\"";
    }

    template<typename L>
    static QByteArray serializeList(const QVariant &propertyValue) {
        L listValue = propertyValue.value<L>();
        QByteArray result("[");
        for (auto value : listValue) {
            result += QString::number(value) + ",";
        }
        if (listValue.size() > 0) {
            result.resize(result.size() - 1);//Remove trailing `,`
        }
        result += "]";
        return result;
    }

    static QByteArray serializeDoubleList(const QVariant &propertyValue) {
        DoubleList listValue = propertyValue.value<DoubleList>();
        QByteArray result("[");
        for (auto value : listValue) {
            result += QString::number(value, 'g').toUtf8() + ",";
        }
        if (listValue.size() > 0) {
            result.resize(result.size() - 1);//Remove trailing `,`
        }
        result += "]";
        return result;
    }

    static QByteArray serializeStringList(const QVariant &propertyValue) {
        QStringList listValue = propertyValue.value<QStringList>();
        QByteArray result("[");
        for (auto value : listValue) {
            result += QByteArray("\"") + value.toUtf8() + "\",";
        }
        if (listValue.size() > 0) {
            result.resize(result.size() - 1);//Remove trailing `,`
        }
        result += "]";
        return result;
    }

    static QByteArray serializeBytesList(const QVariant &propertyValue) {
        QByteArrayList listValue = propertyValue.value<QByteArrayList>();
        QByteArray result("[");
        for (auto value : listValue) {
            result += QByteArray("\"") + value.toBase64() + "\",";
        }
        if (listValue.size() > 0) {
            result.resize(result.size() - 1);//Remove trailing `,`
        }
        result += "]";
        return result;
    }

    QProtobufJsonSerializerPrivate(QProtobufJsonSerializer *q) : qPtr(q) {
        if (handlers.empty()) {
            handlers[qMetaTypeId<QtProtobuf::int32>()] = {{}, QProtobufJsonSerializerPrivate::deserializeInt32};
            handlers[qMetaTypeId<QtProtobuf::sfixed32>()] = {{}, QProtobufJsonSerializerPrivate::deserializeInt32};
            handlers[qMetaTypeId<QtProtobuf::sint32>()] = {{}, QProtobufJsonSerializerPrivate::deserializeInt32};
            handlers[qMetaTypeId<QtProtobuf::sint64>()] = {{}, QProtobufJsonSerializerPrivate::deserializeInt64};
            handlers[qMetaTypeId<QtProtobuf::int64>()] = {{}, QProtobufJsonSerializerPrivate::deserializeInt64};
            handlers[qMetaTypeId<QtProtobuf::sfixed64>()] = {{}, QProtobufJsonSerializerPrivate::deserializeInt64};
            handlers[qMetaTypeId<QtProtobuf::uint32>()] = {{}, QProtobufJsonSerializerPrivate::deserializeUInt32};
            handlers[qMetaTypeId<QtProtobuf::fixed32>()] = {{}, QProtobufJsonSerializerPrivate::deserializeUInt32};
            handlers[qMetaTypeId<QtProtobuf::uint64>()] = {{}, QProtobufJsonSerializerPrivate::deserializeUInt64};
            handlers[qMetaTypeId<QtProtobuf::fixed64>()] = {{}, QProtobufJsonSerializerPrivate::deserializeUInt64};
            handlers[qMetaTypeId<bool>()] = {{}, QProtobufJsonSerializerPrivate::deserializeBool};
            handlers[QMetaType::Float] = {QProtobufJsonSerializerPrivate::serializeFloat, QProtobufJsonSerializerPrivate::deserializeFloat};
            handlers[QMetaType::Double] = {{}, QProtobufJsonSerializerPrivate::deserializeDouble};
            handlers[QMetaType::QString] = {QProtobufJsonSerializerPrivate::serializeString, QProtobufJsonSerializerPrivate::deserializeString};
            handlers[QMetaType::QByteArray] = {QProtobufJsonSerializerPrivate::serializeBytes, QProtobufJsonSerializerPrivate::deserializeByteArray};
            handlers[qMetaTypeId<QtProtobuf::int32List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::int32List>, QProtobufJsonSerializerPrivate::deserializeList<QtProtobuf::int32>};
            handlers[qMetaTypeId<QtProtobuf::int64List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::int64List>, QProtobufJsonSerializerPrivate::deserializeList<QtProtobuf::int64>};
            handlers[qMetaTypeId<QtProtobuf::sint32List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::sint32List>, QProtobufJsonSerializerPrivate::deserializeList<QtProtobuf::sint32>};
            handlers[qMetaTypeId<QtProtobuf::sint64List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::sint64List>, QProtobufJsonSerializerPrivate::deserializeList<QtProtobuf::sint64>};
            handlers[qMetaTypeId<QtProtobuf::uint32List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::uint32List>, QProtobufJsonSerializerPrivate::deserializeList<QtProtobuf::uint32>};
            handlers[qMetaTypeId<QtProtobuf::uint64List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::uint64List>, QProtobufJsonSerializerPrivate::deserializeList<QtProtobuf::uint64>};
            handlers[qMetaTypeId<QtProtobuf::fixed32List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::fixed32List>, QProtobufJsonSerializerPrivate::deserializeList<QtProtobuf::fixed32>};
            handlers[qMetaTypeId<QtProtobuf::fixed64List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::fixed64List>, QProtobufJsonSerializerPrivate::deserializeList<QtProtobuf::fixed64>};
            handlers[qMetaTypeId<QtProtobuf::sfixed32List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::sfixed32List>, QProtobufJsonSerializerPrivate::deserializeList<QtProtobuf::sfixed32>};
            handlers[qMetaTypeId<QtProtobuf::sfixed64List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::sfixed64List>, QProtobufJsonSerializerPrivate::deserializeList<QtProtobuf::sfixed64>};
            handlers[qMetaTypeId<QtProtobuf::FloatList>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::FloatList>, QProtobufJsonSerializerPrivate::deserializeList<float>};
            handlers[qMetaTypeId<QtProtobuf::DoubleList>()] = {QProtobufJsonSerializerPrivate::serializeDoubleList, QProtobufJsonSerializerPrivate::deserializeList<double>};
            handlers[qMetaTypeId<QStringList>()] = {QProtobufJsonSerializerPrivate::serializeStringList, QProtobufJsonSerializerPrivate::deserializeStringList};
            handlers[qMetaTypeId<QByteArrayList>()] = {QProtobufJsonSerializerPrivate::serializeBytesList, QProtobufJsonSerializerPrivate::deserializeList<QByteArray>};
        }
    }
    ~QProtobufJsonSerializerPrivate() = default;

    QByteArray serializeValue(const QVariant &propertyValue, const QProtobufMetaProperty &metaProperty) {
        QByteArray buffer;
        auto userType = propertyValue.userType();
        auto &value = QtProtobufPrivate::findHandler(userType);
        if (value.serializer) {
            value.serializer(qPtr, propertyValue, metaProperty, buffer);
        } else {
            auto handler = handlers.find(userType);
            if (handler != handlers.end() && handler->second.serializer) {
                buffer += handler->second.serializer(propertyValue);
            } else {
                buffer += propertyValue.toString().toUtf8();
            }
        }
        return buffer;
    }

    QByteArray serializeProperty(const QVariant &propertyValue, const QProtobufMetaProperty &metaProperty) {
        return QByteArray("\"") + metaProperty.protoPropertyName().toUtf8() + "\":" + serializeValue(propertyValue, metaProperty);
    }

    QByteArray serializeObject(const QObject *object, const QProtobufMetaObject &metaObject) {
        QByteArray result = "{";
        for (const auto &field : metaObject.propertyOrdering) {
            int propertyIndex = field.second;
            int fieldIndex = field.first;
            Q_ASSERT_X(fieldIndex < 536870912 && fieldIndex > 0, "", "fieldIndex is out of range");
            QMetaProperty metaProperty = metaObject.staticMetaObject.property(propertyIndex);
            const char *propertyName = metaProperty.name();
            const QVariant &propertyValue = object->property(propertyName);
            result.append(serializeProperty(propertyValue, QProtobufMetaProperty(metaProperty, fieldIndex)));
            result.append(",");
        }
        result.resize(result.size() - 1);//Remove trailing `,`
        result.append("}");
        return result;
    }

    static QVariant deserializeInt32(const QByteArray &data) {
        return QVariant::fromValue(data.toInt());
    }

    static QVariant deserializeUInt32(const QByteArray &data) {
        return QVariant::fromValue(data.toUInt());
    }

    static QVariant deserializeInt64(const QByteArray &data) {
        return QVariant::fromValue(data.toLongLong());
    }

    static QVariant deserializeUInt64(const QByteArray &data) {
        return QVariant::fromValue(data.toULongLong());
    }

    static QVariant deserializeFloat(const QByteArray &data) {
        return QVariant::fromValue(data.toFloat());
    }

    static QVariant deserializeDouble(const QByteArray &data) {
        return QVariant::fromValue(data.toDouble());
    }

    static QVariant deserializeBool(const QByteArray &data) {
        return QVariant::fromValue(QString::fromUtf8(data));
    }

    static QVariant deserializeString(const QByteArray &data) {
        return QVariant::fromValue(QString::fromUtf8(data).replace("\\\"", "\""));
    }

    static QVariant deserializeByteArray(const QByteArray &data) {
        return QVariant::fromValue(QByteArray::fromBase64(data));
    }

    template<typename T>
    static QVariant deserializeList(const QByteArray &data) {
        QList<T> list;
        auto arrayValues = microjson::parseJsonArray(data.data(), static_cast<size_t>(data.size()));
        auto handler = handlers.find(qMetaTypeId<T>());
        if (handler == handlers.end() || !handler->second.deserializer) {
            qProtoWarning() << "Unable to deserialize simple type list. Could not find desrializer for type" << qMetaTypeId<T>();
            return QVariant::fromValue(list);
        }

        for (auto &arrayValue : arrayValues) {
            QVariant newValue = handler->second.deserializer(QByteArray::fromStdString(arrayValue.value));
            list.append(newValue.value<T>());
        }
        return QVariant::fromValue(list);
    }

    static QVariant deserializeStringList(const QByteArray &data) {
        QStringList list;
        auto arrayValues = microjson::parseJsonArray(data.data(), static_cast<size_t>(data.size()));
        for (auto &arrayValue : arrayValues) {
            QVariant newValue = deserializeString(QByteArray::fromStdString(arrayValue.value));
            list.append(newValue.value<QString>());
        }
        return QVariant::fromValue(list);
    }

    void deserializeObject(QObject *object, const QProtobufMetaObject &metaObject, const char *data, int size) {
        microjson::JsonObject obj = microjson::parseJsonObject(data, static_cast<size_t>(size));

        for (auto &property : obj) {
            auto name = property.first;
            int propertyIndex = metaObject.staticMetaObject.indexOfProperty(name.data());
            if (propertyIndex >= 0) {
                QMetaProperty metaProperty = metaObject.staticMetaObject.property(propertyIndex);
                auto userType = metaProperty.userType();
                auto &handler = QtProtobufPrivate::findHandler(userType);
                QByteArray value = QByteArray::fromStdString(property.second.value);
                if (handler.deserializer) {
                    QVariant newValue;
                    if (property.second.type == microjson::JsonArrayType) {
                        microjson::JsonArray arrayValues = microjson::parseJsonArray(property.second.value.data(), property.second.value.size());
                        if (arrayValues.size() > 0) {
                            if (arrayValues[0].type == microjson::JsonObjectType) {
                                for (auto &arrayValue : arrayValues) {
                                    QByteArray arrayBuffer = QByteArray::fromStdString(arrayValue.value);
                                    QtProtobuf::QProtobufSelfcheckIterator it(arrayBuffer);
                                    handler.deserializer(qPtr, it, newValue);
                                }
                            } else {
                                QtProtobuf::QProtobufSelfcheckIterator it(value);
                                handler.deserializer(qPtr, it, newValue);
                            }
                        }
                    } else {
                        QtProtobuf::QProtobufSelfcheckIterator it(value);
                        handler.deserializer(qPtr, it, newValue);
                    }
                    object->setProperty(name.c_str(), newValue);
                } else {
                    auto handler = handlers.find(metaProperty.userType());
                    if (handler != handlers.end() && handler->second.deserializer) {
                        object->setProperty(name.c_str(), handler->second.deserializer(value));
                    }
                }
            }
        }
    }
private:
    static SerializerRegistry handlers;
    QProtobufJsonSerializer *qPtr;
};

QProtobufJsonSerializerPrivate::SerializerRegistry QProtobufJsonSerializerPrivate::handlers = {};

}

QProtobufJsonSerializer::QProtobufJsonSerializer() : dPtr(new QProtobufJsonSerializerPrivate(this))
{
}

QProtobufJsonSerializer::~QProtobufJsonSerializer() = default;


QByteArray QProtobufJsonSerializer::serializeMessage(const QObject *object, const QProtobufMetaObject &metaObject) const
{
    return dPtr->serializeObject(object, metaObject);
}

void QProtobufJsonSerializer::deserializeMessage(QObject *object, const QProtobufMetaObject &metaObject, const QByteArray &data) const
{
    dPtr->deserializeObject(object, metaObject, data.data(), data.size());
}

QByteArray QProtobufJsonSerializer::serializeObject(const QObject *object, const QProtobufMetaObject &metaObject, const QProtobufMetaProperty &/*metaProperty*/) const
{
    return dPtr->serializeObject(object, metaObject);
}

void QProtobufJsonSerializer::deserializeObject(QObject *object, const QProtobufMetaObject &metaObject, QProtobufSelfcheckIterator &it) const
{
    dPtr->deserializeObject(object, metaObject, it.data(), it.size());
}

QByteArray QProtobufJsonSerializer::serializeListBegin(const QProtobufMetaProperty &/*metaProperty*/) const
{
    return {"["};
}

QByteArray QProtobufJsonSerializer::serializeListObject(const QObject *object, const QProtobufMetaObject &metaObject, const QProtobufMetaProperty &/*metaProperty*/) const
{
    return dPtr->serializeObject(object, metaObject) + ",";
}

QByteArray QProtobufJsonSerializer::serializeListEnd(QByteArray &buffer, const QProtobufMetaProperty &/*metaProperty*/) const
{
    if (buffer[buffer.size() - 1] == ',') {
        buffer.resize(buffer.size() - 1);
    }
    return {"]"};
}

void QProtobufJsonSerializer::deserializeListObject(QObject *object, const QProtobufMetaObject &metaObject, QProtobufSelfcheckIterator &it) const
{
    dPtr->deserializeObject(object, metaObject, it.data(), it.size());
}

QByteArray QProtobufJsonSerializer::serializeMapBegin(const QProtobufMetaProperty &/*metaProperty*/) const
{
    return {"{"};
}
QByteArray QProtobufJsonSerializer::serializeMapPair(const QVariant &key, const QVariant &value, const QProtobufMetaProperty &metaProperty) const
{
    return QByteArray("\"") + key.toString().toUtf8() + "\":" + dPtr->serializeValue(value, metaProperty) + ",";
}

QByteArray QProtobufJsonSerializer::serializeMapEnd(QByteArray &buffer, const QProtobufMetaProperty &/*metaProperty*/) const
{
    if (buffer[buffer.size() - 1] == ',') {
        buffer.resize(buffer.size() - 1);
    }
    return {"}"};
}

void QProtobufJsonSerializer::deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(key)
    Q_UNUSED(value)
    Q_UNUSED(it)
}

QByteArray QProtobufJsonSerializer::serializeEnum(int64 value, const QMetaEnum &metaEnum, const QtProtobuf::QProtobufMetaProperty &/*metaProperty*/) const
{
    return QByteArray("\"") + metaEnum.key(static_cast<int>(value)) + "\"";
}

QByteArray QProtobufJsonSerializer::serializeEnumList(const QList<int64> &values, const QMetaEnum &metaEnum, const QtProtobuf::QProtobufMetaProperty &/*metaProperty*/) const
{
    QByteArray result = "[";
    for (auto value : values) {
        result.append("\"");
        result.append(metaEnum.key(static_cast<int>(value)));
        result.append("\",");
    }
    if (values.size() > 0) {
        result.resize(result.size() - 1);
    }
    result.append("]");
    return result;
}

void QProtobufJsonSerializer::deserializeEnum(int64 &value, const QMetaEnum &metaEnum, QProtobufSelfcheckIterator &it) const
{
    value = metaEnum.keyToValue(it.data());
}

void QProtobufJsonSerializer::deserializeEnumList(QList<int64> &value, const QMetaEnum &metaEnum, QProtobufSelfcheckIterator &it) const
{
    auto arrayValues = microjson::parseJsonArray(it.data(), static_cast<size_t>(it.size()));

    for (auto &arrayValue : arrayValues) {
        value.append(metaEnum.keyToValue(arrayValue.value.c_str()));
    }
}
