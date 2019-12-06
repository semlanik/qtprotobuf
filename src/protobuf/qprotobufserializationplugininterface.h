/*
 * MIT License
 *
 * Copyright (c) 2019 Tatyana Borisova <tanusshhka@mail.ru>
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

#pragma once //QProtobufSerializationPluginInterface

#include <QObject>
#include <QString>

#include <unordered_map>
#include <memory>

#include "qtprotobufglobal.h"
#include "qabstractprotobufserializer.h"

namespace QtProtobuf {

class Q_PROTOBUF_EXPORT QProtobufSerializationPluginInterface
{
public:
    explicit QProtobufSerializationPluginInterface() {}
    virtual ~QProtobufSerializationPluginInterface() {}

    virtual QtProtobuf::QAbstractProtobufSerializer *serializer(const QString &serializer_name) = 0;
};

}
#define SerializatorInterface_iid "com.qtprotobuf.QProtobufSerializationPluginInterface"
Q_DECLARE_INTERFACE(QtProtobuf::QProtobufSerializationPluginInterface, SerializatorInterface_iid)
