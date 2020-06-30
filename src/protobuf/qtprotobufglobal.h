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

#include <QtCore/QtGlobal>

#ifndef QT_PROTOBUF_STATIC
    #if defined(QT_BUILD_PROTOBUF_LIB)
        #define Q_PROTOBUF_EXPORT Q_DECL_EXPORT
    #else
        #define Q_PROTOBUF_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define Q_PROTOBUF_EXPORT
#endif

//Support macro for Qt versions < 5.13
//TODO: remove once migration to 5.14 as minimum required version is completed
#ifndef Q_DISABLE_MOVE
    #define Q_DISABLE_MOVE(Class) \
        Class(Class &&) = delete; \
        Class &operator=(Class &&) = delete;
#endif

#ifndef Q_DISABLE_COPY_MOVE
    #define Q_DISABLE_COPY_MOVE(Class) \
        Q_DISABLE_COPY(Class) \
        Q_DISABLE_MOVE(Class)
#endif
/*!
 * \ingroup QtProtobuf
 * \def Q_PROTOBUF_IMPORT_QUICK_PLUGIN()
 *      Imports statically linked QtProtobufQuickPlugin.<br/>
 *      If you use statically linked QtProtobuf, you need to call
 *      Q_PROTOBUF_IMPORT_QUICK_PLUGIN macro before use QtProtobuf in QML.<br/>
 * C++:
 * \code
 * int main(int argc, char *argv[]) {
 *     ...
 *     QtProtobuf::qRegisterProtobufTypes();
 *     Q_PROTOBUF_IMPORT_QUICK_PLUGIN()
 *     ...
 * }
 * \endcode
 *
 * QML:
 * \code
 * //Make QtProtobuf types visible in QML
 * import QtProtobuf 0.4 //Use recent QtProtobuf version
 * ...
 * Item {
 *     //QtProtobuf usage
 * }
 * ...
 * \endcode
 */
#ifndef Q_PROTOBUF_IMPORT_QUICK_PLUGIN
    #ifdef QT_PROTOBUF_STATIC
        #include <QtPlugin>
        #include <QQmlExtensionPlugin>
        #define Q_PROTOBUF_IMPORT_QUICK_PLUGIN() \
            Q_IMPORT_PLUGIN(QtProtobufQuickPlugin) \
            qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtProtobufQuickPlugin().instance())->registerTypes("QtProtobuf");
    #else
        #define Q_PROTOBUF_IMPORT_QUICK_PLUGIN()
    #endif //QT_PROTOBUF_STATIC
#endif //Q_PROTOBUF_IMPORT_QUICK_PLUGIN
