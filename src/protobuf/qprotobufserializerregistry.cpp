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

#include "qprotobufserializerregistry_p.h"
#include "qprotobufserializer.h"
#include "qprotobufjsonserializer.h"

#include <QString>
#include <QHash>

namespace std {
  template<> struct hash<QString> {
    std::size_t operator()(const QString &s) const {
      return std::hash<std::string>()(s.toStdString());
    }
  };
}

namespace QtProtobuf {
class QProtobufSerializerRegistryPrivate {
public:
    QProtobufSerializerRegistryPrivate() {
        serializers["protobuf"] = std::shared_ptr<QAbstractProtobufSerializer>(new QProtobufSerializer);
        serializers["json"] = std::shared_ptr<QAbstractProtobufSerializer>(new QProtobufJsonSerializer);
    }
    std::unordered_map<QString/*id*/,  std::shared_ptr<QAbstractProtobufSerializer>> serializers;
};
}

using namespace QtProtobuf;

QProtobufSerializerRegistry::QProtobufSerializerRegistry() : d(new QProtobufSerializerRegistryPrivate)
{

}

QProtobufSerializerRegistry::~QProtobufSerializerRegistry() = default;


std::shared_ptr<QAbstractProtobufSerializer> QProtobufSerializerRegistry::getSerializer(const QString &id)
{
    return d->serializers.at(id); //throws
}

std::unique_ptr<QAbstractProtobufSerializer> QProtobufSerializerRegistry::acquireSerializer(const QString &/*id*/)
{
    Q_ASSERT_X(false, "QProtobufSerializerRegistry", "acquireSerializer is unimplemented");
    return std::unique_ptr<QAbstractProtobufSerializer>();
}
