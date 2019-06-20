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

#pragma once

#include <QObject>
#include <QVariant>
#include <QMetaObject>

#include <unordered_map>
#include <functional>

#include "qtprotobuftypes.h"
#include "qtprotobuflogging.h"
#include "qprotobufselfcheckiterator.h"

#include "qtprotobufglobal.h"

namespace QtProtobuf {
/*!
*  \addtogroup QtProtobuf
*  \{
*/

/*!
 * \brief The QAbstractProtobufSerializer class is interface that represents basic functions for serialization/deserialization
 *
 * \details This class is used by QProtobufSerializerRegistry to access basic serialization routines.
 *
 */
class Q_PROTOBUF_EXPORT QAbstractProtobufSerializer
{
public:
    /*!
     * \brief Serializer is interface function for serialize method
     */
    using Serializer = std::function<QByteArray(const QVariant &, int &)>;
    /*!
     * \brief Deserializer is interface function for deserialize method
     */
    using Deserializer = std::function<void(QProtobufSelfcheckIterator &, QVariant &)>;

    /*!
     * \brief SerializationHandlers contains set of objects that required for class serializaion/deserialization
     */
    struct SerializationHandlers {
        Serializer serializer; /*!< serializer assigned to class */
        Deserializer deserializer;/*!< deserializer assigned to class */
        WireTypes type;/*!< Serialization WireType */
    };
    /*!
     * \brief SerializerRegistry is container to store mapping between metatype identifier and serialization handlers.
     */
    using SerializerRegistry = std::unordered_map<int/*metatypeid*/, SerializationHandlers>;

    virtual ~QAbstractProtobufSerializer() = default;
    virtual QByteArray serializeProperty(const QVariant &propertyValue, int fieldIndex, bool isEnum) = 0;
    virtual void deserializeProperty(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) = 0;

    virtual QByteArray serializeObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) = 0;
    virtual void deserializeObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) = 0;

    virtual QByteArray serializeListObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject, int fieldIndex) = 0;
    virtual void deserializeListObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) = 0;

    virtual QByteArray serializeMapPair(const QVariant &key, const QVariant &value, int fieldIndex) = 0;
    virtual void deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it) = 0;

    QByteArray serializeObjectCommon(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject);
    void deserializeObjectCommon(QObject *object, const QByteArray &array, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject);

    const SerializerRegistry& handlers() { return m_handlers; }

protected:
    SerializerRegistry m_handlers;//TODO: move to d-prointer
};

/*!
 * \brief The QProtobufSerializer class
 */
class Q_PROTOBUF_EXPORT QProtobufSerializer : public QAbstractProtobufSerializer
{
public:
    QProtobufSerializer();
    ~QProtobufSerializer() = default;

    QByteArray serializeProperty(const QVariant &propertyValue, int fieldIndex, bool isEnum) override;
    void deserializeProperty(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) override;

    QByteArray serializeObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) override;
    void deserializeObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) override;

    QByteArray serializeListObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject, int fieldIndex) override;
    void deserializeListObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) override;

    QByteArray serializeMapPair(const QVariant &key, const QVariant &value, int fieldIndex) override;
    void deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it) override;
};
/*! \} */

//! \private
constexpr int NotUsedFieldIndex = -1;
}
