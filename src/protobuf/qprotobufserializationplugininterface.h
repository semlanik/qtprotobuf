/*
 * MIT License
 *
 * Copyright (c) 2019 Tatyana Borisova <tanusshhka@mail.ru>, Alexey Edelev <semlanik@gmail.com>
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

/*!
 * \ingroup QtProtobuf
 * \brief The QProtobufSerializationPluginInterface class is interface that provides possibility
 *        to load serialization/deserialization implementations to project via Qt plugin mechanism.
 *
 * \details This class should be used as parent of main plugin class. Inherited class should reimplement
 *          scope of virtual methods that used by serialization/deserialization functionality. Plugin interface name
 *          should be declared via Q_INTERFACES macros. Plugin must provide metadata information about itself.
 *          Information is stored in json format and attached to plugin using Q_PLUGIN_METADATA macros.  \n
 *          An example of class source code:
 *
 *          \code{.cpp}
 *          class SERIALIZATIONSHARED_EXPORT QtSerializationPlugin : public QObject, QtProtobuf::QProtobufSerializationPluginInterface
 *          {
 *              Q_OBJECT
 *              Q_PLUGIN_METADATA(IID SerializatorInterface_iid FILE "serializeinfo.json")
 *              Q_INTERFACES(QtProtobuf::QProtobufSerializationPluginInterface)
 *
 *          public:
 *              QtSerializationPlugin();
 *              ~QtSerializationPlugin() = default;
 *
 *              virtual std::shared_ptr<QtProtobuf::QAbstractProtobufSerializer> serializer(const QString &serializerName);
 *
 *          }
 *          \endcode
 *
 *          An example of json file:
 *
 *          \code{.json}
 *          {
 *             "name":"Test Plugin",
 *             "author":"Jhon Smith",
 *             "version":1.0,
 *             "protobufVersion":1.0,
 *             "types":["protobuf", "json"],
 *             "rating":0
 *          }
 *          \endcode
 *
 *         Plugins are read by path that is defined by QT_PROTOBUF_PLUGIN_PATH variable.
 *         QtProtobuf reads plugins metadata list from protobuf plugin folder and provides to user possibility to choose plugin and serializer for serialization/deserialization.
 *
 */
class Q_PROTOBUF_EXPORT QProtobufSerializationPluginInterface
{
public:
    explicit QProtobufSerializationPluginInterface() = default;
    virtual ~QProtobufSerializationPluginInterface() = default;

    /*!
     * \brief Method finds and returns pointer to specific serialization implementation by serializer name, otherwise returns nullptr.
     * \param[in] name of specific serializer that should be supplied by plugin.
     * \return An object to serializer realization.
     */
    virtual std::shared_ptr<QtProtobuf::QAbstractProtobufSerializer> serializer(const QString &serializerName) = 0;
};

}
#define SerializatorInterface_iid "com.qtprotobuf.QProtobufSerializationPluginInterface"
Q_DECLARE_INTERFACE(QtProtobuf::QProtobufSerializationPluginInterface, SerializatorInterface_iid)
