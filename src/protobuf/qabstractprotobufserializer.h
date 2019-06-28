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

    /*!
     * \brief serializeProperty Method called on property serialization cycle.
     * \param[in] propertyValue Value of property and metainformation stored in QVariant
     * \param[in] fieldIndex index of property in message
     * \param[in] metaProperty meta information about property to be serialized
     * \return Raw serialized data represented as byte array
     */
    virtual QByteArray serializeProperty(const QVariant &propertyValue, int fieldIndex, const QMetaProperty &metaProperty) = 0;

    /*!
     * \brief deserializeProperty Method called on property deserialization cycle
     * \param[out] object Property value will be written to the given QObject.
     * \param[in] it Pointer to next chunk of raw data received from channel
     * \param[in] propertyOrdering Ordering of properties for given \a object
     * \param[in] metaObject Static meta object of given \a object. Static meta object usualy is used to get actual
     *            property value and write new property to \a object
     */
    virtual void deserializeProperty(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) = 0;

    /*!
     * \brief serializeObject Serializes complete \a object according given \a propertyOrdering and \a metaObject
     *        information
     * \param[in] object Pointer to object to be serialized
     * \param[in] propertyOrdering Protobuf order of QObject properties
     * \param[in] metaObject Meta object information for given \a object
     * \return Raw serialized data represented as byte array
     */
    virtual QByteArray serializeObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) = 0;

    /*!
     * \brief deserializeObject Deserializes buffer to an \a object
     * \param[out] object Pointer to allocated object
     * \param[in] it Pointer to beging of buffer where object serialized data is located
     * \param[in] propertyOrdering Ordering of properties for given \a object
     * \param[in] metaObject Static meta object of given \a object. Static meta object usualy is used to get actual
     *        property value and write new property to \a object
     */
    virtual void deserializeObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) = 0;

    /*!
     * \brief serializeListObject Method called to serialize \a object as a part of list property
     * \param[in] object Pointer to object that will be serialized
     * \param[in] propertyOrdering Ordering of properties for given \a object
     * \param[in] metaObject Static meta object of given \a object
     * \param[in] fieldIndex Index of list property in target message
     * \return Raw serialized data represented as byte array
     */
    virtual QByteArray serializeListObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject, int fieldIndex) = 0;

    /*!
     * \brief deserializeListObject Deserializes an \a object from byte stream as part of list property
     * \param[out] object Pointer to allocated object, that will be appended to list property
     * \param[in] it Pointer to beging of buffer where object serialized data is located
     * \param[in] propertyOrdering Ordering of properties for given \a object
     * \param[in] metaObject Static meta object of given \a object. Static meta object usualy is used to get actual
     *        property value and write new property to \a object
     */
    virtual void deserializeListObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject) = 0;

    /*!
     * \brief serializeMapPair Serializes QMap pair of \a key and \a value to raw data buffer
     * \param[in] key Map key
     * \param[in] value Map value for given \a key
     * \param[in] fieldIndex Index of map property in message
     * \return Raw serialized data represented as byte array
     *
     * \see https://developers.google.com/protocol-buffers/docs/proto3#maps for details
     */
    virtual QByteArray serializeMapPair(const QVariant &key, const QVariant &value, int fieldIndex) = 0;

    /*!
     * \brief deserializeMapPair Deserializes QMap pair of \a key and \a value from raw data
     * \param[out] key Buffer that will be used to store deserialized key. When passed to function, QVariant
     *        already stores default constructed value. So it's possible to receive meta information about type from it.
     * \param[out] value Buffer that will be used to store deserialized value. When passed to function, QVariant
     *        already stores default constructed value. So it's possible to receive meta information about type from it.
     * \param[in] it Points to serialized raw key/value data
     *
     * \see https://developers.google.com/protocol-buffers/docs/proto3#maps for details
     */
    virtual void deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it) = 0;

    /*!
     * \brief serializeObjectCommon Common routine to iterate object properties. Could be used in serializer
     *        implementations. But in practice is private interface for QProtobufSerializerRegistry
     * \param[in] object Pointer to object to be serialized
     * \param[in] propertyOrdering Ordering of properties for given \a object
     * \param[in] metaObject Static meta object of given \a object.
     * \return Raw serialized data represented as byte array
     */
    QByteArray serializeObjectCommon(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject);

    /*!
     * \brief deserializeObjectCommon Common routine to iterate object properties. Could be used in serializer
     *        implementations. But in practice is private interface for QProtobufSerializerRegistry
     * \param[out] object Pointer to allocated object
     * \param[in] array Complete message buffer received from transport stream
     * \param[in] propertyOrdering Ordering of properties for given \a object
     * \param[in] metaObject Static meta object of given \a object. Static meta object usualy is used to get actual
     *        property value and write new property to \a object
     */
    void deserializeObjectCommon(QObject *object, const QByteArray &array, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject);

    /*!
     * \brief handlers Method to access serializers set provided by serializer
     * \return Serializers set provided by serializer
     *
     * \note In practice this method is interface for QProtobufSerializerRegistry
     */
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

    QByteArray serializeProperty(const QVariant &propertyValue, int fieldIndex, const QMetaProperty &metaProperty) override;
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
