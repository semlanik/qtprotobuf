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

#pragma once //QProtobufJsonSerializerImpl

#include "qabstractprotobufserializer.h"
#include "qprotobufmetaobject.h"
#include "qtprotobuftypes.h"

#include <memory>

/*!
*  \ingroup QtProtobuf
 * \brief The QProtobufJsonSerializerPlugin class
 */
class QProtobufJsonSerializerImpl : public QtProtobuf::QAbstractProtobufSerializer
{
public:
    QProtobufJsonSerializerImpl();
    ~QProtobufJsonSerializerImpl() = default;

protected:
    QByteArray serializeMessage(const QObject *object, const QtProtobuf::QProtobufMetaObject &metaObject) const  override;
    void deserializeMessage(QObject *object, const QtProtobuf::QProtobufMetaObject &metaObject, const QByteArray &data) const override;

    QByteArray serializeObject(const QObject *object, const QtProtobuf::QProtobufMetaObject &metaObject, const QtProtobuf::QProtobufMetaProperty &metaProperty) const override;
    void deserializeObject(QObject *object, const QtProtobuf::QProtobufMetaObject &metaObject, QtProtobuf::QProtobufSelfcheckIterator &it) const override;

    QByteArray serializeListObject(const QObject *object, const QtProtobuf::QProtobufMetaObject &metaObject, const QtProtobuf::QProtobufMetaProperty &metaProperty) const override;
    bool deserializeListObject(QObject *object, const QtProtobuf::QProtobufMetaObject &metaObject, QtProtobuf::QProtobufSelfcheckIterator &it) const override;

    QByteArray serializeMapPair(const QVariant &key, const QVariant &value, const QtProtobuf::QProtobufMetaProperty &metaProperty) const override;
    bool deserializeMapPair(QVariant &key, QVariant &value, QtProtobuf::QProtobufSelfcheckIterator &it) const override;

    QByteArray serializeEnum(QtProtobuf::int64 value, const QMetaEnum &metaEnum, const QtProtobuf::QProtobufMetaProperty &metaProperty) const override;
    QByteArray serializeEnumList(const QList<QtProtobuf::int64> &value, const QMetaEnum &metaEnum, const QtProtobuf::QProtobufMetaProperty &metaProperty) const override;

    void deserializeEnum(QtProtobuf::int64 &value, const QMetaEnum &metaEnum, QtProtobuf::QProtobufSelfcheckIterator &it) const override;
    void deserializeEnumList(QList<QtProtobuf::int64> &value, const QMetaEnum &metaEnum, QtProtobuf::QProtobufSelfcheckIterator &it) const override;
};
