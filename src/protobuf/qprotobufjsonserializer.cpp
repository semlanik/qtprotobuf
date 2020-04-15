/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
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
#include <QMetaProperty>

using namespace QtProtobuf;

namespace QtProtobuf {

//! \private
class QProtobufJsonSerializerPrivate final
{
    Q_DISABLE_COPY_MOVE(QProtobufJsonSerializerPrivate)
public:
    using Serializer = std::function<QByteArray(const QVariant&)>;
    using Deserializer = std::function<void(void)>;

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
        return QString::number(value, 'g').toUtf8();
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

    QProtobufJsonSerializerPrivate(QProtobufJsonSerializer *q) : q_ptr(q) {
        if (handlers.empty()) {
            handlers[QMetaType::Float] = {QProtobufJsonSerializerPrivate::serializeFloat, []{}};
            handlers[QMetaType::QString] = {QProtobufJsonSerializerPrivate::serializeString, []{}};
            handlers[QMetaType::QByteArray] = {QProtobufJsonSerializerPrivate::serializeBytes, []{}};
            handlers[qMetaTypeId<QtProtobuf::int32List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::int32List>, []{}};
            handlers[qMetaTypeId<QtProtobuf::int64List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::int64List>, []{}};
            handlers[qMetaTypeId<QtProtobuf::sint32List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::sint32List>, []{}};
            handlers[qMetaTypeId<QtProtobuf::sint64List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::sint64List>, []{}};
            handlers[qMetaTypeId<QtProtobuf::uint32List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::uint32List>, []{}};
            handlers[qMetaTypeId<QtProtobuf::uint64List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::uint64List>, []{}};
            handlers[qMetaTypeId<QtProtobuf::fixed32List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::fixed32List>, []{}};
            handlers[qMetaTypeId<QtProtobuf::fixed64List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::fixed64List>, []{}};
            handlers[qMetaTypeId<QtProtobuf::sfixed32List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::sfixed32List>, []{}};
            handlers[qMetaTypeId<QtProtobuf::sfixed64List>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::sfixed64List>, []{}};
            handlers[qMetaTypeId<QtProtobuf::FloatList>()] = {QProtobufJsonSerializerPrivate::serializeList<QtProtobuf::FloatList>, []{}};
            handlers[qMetaTypeId<QtProtobuf::DoubleList>()] = {QProtobufJsonSerializerPrivate::serializeDoubleList, []{}};
            handlers[qMetaTypeId<QStringList>()] = {QProtobufJsonSerializerPrivate::serializeStringList,[]{}};
            handlers[qMetaTypeId<QByteArrayList>()] = {QProtobufJsonSerializerPrivate::serializeBytesList,[]{}};
        }
    }
    ~QProtobufJsonSerializerPrivate() = default;

    QByteArray serializeProperty(const QVariant &propertyValue, const QProtobufMetaProperty &metaProperty) {
        QByteArray buffer;
        buffer.append("\"");
        buffer.append(metaProperty.protoPropertyName().toUtf8());
        buffer.append("\":");
        auto userType = metaProperty.userType();
        auto &value = QtProtobufPrivate::findHandler(metaProperty.userType());
        if (value.serializer) {
            value.serializer(q_ptr, propertyValue, metaProperty, buffer);
        } else {
            auto handler = handlers.find(userType);
            if (handler != handlers.end()) {
                buffer += handler->second.serializer(propertyValue);
            } else {
                buffer += propertyValue.toString().toUtf8();
            }
        }
        return buffer;
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

private:
    static SerializerRegistry handlers;
    QProtobufJsonSerializer *q_ptr;
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
    Q_UNUSED(object)
    Q_UNUSED(data)
    Q_UNUSED(metaObject)
}

QByteArray QProtobufJsonSerializer::serializeObject(const QObject *object, const QProtobufMetaObject &metaObject, const QProtobufMetaProperty &/*metaProperty*/) const
{
    return dPtr->serializeObject(object, metaObject);
}

void QProtobufJsonSerializer::deserializeObject(QObject *object, const QProtobufMetaObject &metaObject, QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(object)
    Q_UNUSED(it)
    Q_UNUSED(metaObject)
}

QByteArray QProtobufJsonSerializer::serializeListObject(const QObject *object, const QProtobufMetaObject &metaObject, const QProtobufMetaProperty &/*metaProperty*/) const
{
    return dPtr->serializeObject(object, metaObject);
}

void QProtobufJsonSerializer::deserializeListObject(QObject *object, const QProtobufMetaObject &metaObject, QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(object)
    Q_UNUSED(it)
    Q_UNUSED(metaObject)
}

QByteArray QProtobufJsonSerializer::serializeMapPair(const QVariant &key, const QVariant &value, const QProtobufMetaProperty &metaProperty) const
{
    Q_UNUSED(key)
    Q_UNUSED(value)
    Q_UNUSED(metaProperty)
    return QByteArray();
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
    Q_UNUSED(value)
    Q_UNUSED(metaEnum)
    Q_UNUSED(it)
}

void QProtobufJsonSerializer::deserializeEnumList(QList<int64> &value, const QMetaEnum &metaEnum, QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(value)
    Q_UNUSED(metaEnum)
    Q_UNUSED(it)
}
