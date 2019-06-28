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

using namespace QtProtobuf;

QByteArray QProtobufJsonSerializer::serializeProperty(const QVariant &propertyValue, int fieldIndex, const QMetaProperty &metaProperty)
{
    Q_UNUSED(propertyValue)
    Q_UNUSED(fieldIndex)
    Q_UNUSED(metaProperty)
    return QByteArray();
}

void QProtobufJsonSerializer::deserializeProperty(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject)
{
    Q_UNUSED(object)
    Q_UNUSED(it)
    Q_UNUSED(propertyOrdering)
    Q_UNUSED(metaObject)
}

QByteArray QProtobufJsonSerializer::serializeObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject)
{
    Q_UNUSED(object)
    Q_UNUSED(propertyOrdering)
    Q_UNUSED(metaObject)
    return QByteArray();
}

void QProtobufJsonSerializer::deserializeObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject)
{
    Q_UNUSED(object)
    Q_UNUSED(it)
    Q_UNUSED(propertyOrdering)
    Q_UNUSED(metaObject)
}

QByteArray QProtobufJsonSerializer::serializeListObject(const QObject *object, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject, int fieldIndex)
{
    Q_UNUSED(object)
    Q_UNUSED(propertyOrdering)
    Q_UNUSED(metaObject)
    Q_UNUSED(fieldIndex)
    return QByteArray();
}

void QProtobufJsonSerializer::deserializeListObject(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject)
{
    Q_UNUSED(object)
    Q_UNUSED(it)
    Q_UNUSED(propertyOrdering)
    Q_UNUSED(metaObject)
}

QByteArray QProtobufJsonSerializer::serializeMapPair(const QVariant &key, const QVariant &value, int fieldIndex)
{
    Q_UNUSED(key)
    Q_UNUSED(value)
    Q_UNUSED(fieldIndex)
    return QByteArray();
}

void QProtobufJsonSerializer::deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it)
{
    Q_UNUSED(key)
    Q_UNUSED(value)
    Q_UNUSED(it)
}
