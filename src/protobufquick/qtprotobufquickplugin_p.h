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

#include <QQmlExtensionPlugin>
#include <QtProtobufQuick/qtprotobufquick_global.h>

/*!
 * \defgroup QtProtobufQML QML QtProtobuf
 * \brief QML bindings for QtGrpc
 * \details
 * QtProtobuf allows to use generated classes in QML. All types are registred automatically when qRegisterProtobufTypes() called.
 * To make generated classes and QtProtobuf types visible in QML you need to import QtProtobuf QML plugin and your protobuf package
 *
 * sample.proto:
 * \code
 * syntax = "proto3";
 * package qtprotobuf.sample;
 * message SampleMessage {
 *     int32 sampleField = 1;
 * }
 * \endcode
 *
 * QML:
 * \code
 * //Enable QtProtobuf types
 * import QtProtobuf 0.4 //Use recent QtProtobuf version
 * //Import types from sample.proto
 * import qtprotobuf.sample 1.0
 *
 * Item {
 * ...
 *     SampleMessage {
 *         sampleField: 10
 *     }
 * ...
 * }
 * \endcode
 *
 * \section qtprotobufquick_static Static QtProtobuf with QML
 * For statically built QtProtobuf it's neccssary to link `protobufquickplugin` target manually:
 * CMakeLists.txt:
 * \code
 * ...
 * if(QT_PROTOBUF_STATIC)
 *     target_link_libraries(${TARGET} PRIVATE QProtobufprotobufquickplugin)
 * endif()
 * ...
 * \endcode
 *
 * To make QtProtobuf QML plugin visible in QML, call Q_PROTOBUF_IMPORT_QUICK_PLUGIN() macro:
 * \code
 * int main(int argc, char *argv[]) {
 *     ...
 *     qRegisterProtobufTypes();
 *     Q_PROTOBUF_IMPORT_QUICK_PLUGIN()
 *     ...
 * }
 * \endcode
 */

/*!
 * \private
 * \brief The QtProtobufQuickPlugin class
 */

QT_BEGIN_NAMESPACE

class QT_PROTOBUF_QUICK_SHARED_EXPORT QtProtobufQuickPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    ~QtProtobufQuickPlugin() = default;
    void registerTypes(const char *) override;
};

QT_END_NAMESPACE
