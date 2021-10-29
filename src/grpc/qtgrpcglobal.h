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
    #ifdef QT_BUILD_GRPC_LIB
        #define Q_GRPC_EXPORT Q_DECL_EXPORT
    #else
        #define Q_GRPC_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define Q_GRPC_EXPORT
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

#ifndef Q_GRPC_IMPORT_QUICK_PLUGIN
    #if defined(QT_PROTOBUF_STATIC) && defined(QT_QML_LIB) // TODO: Check how detect this in Qt6
        #include <QtPlugin>
        #include <QQmlExtensionPlugin>
        #define Q_GRPC_IMPORT_QUICK_PLUGIN() \
            Q_IMPORT_PLUGIN(QtGrpcQuickPlugin) \
            qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtGrpcQuickPlugin().instance())->registerTypes("QtGrpc");
    #else
        #define Q_GRPC_IMPORT_QUICK_PLUGIN()
    #endif //QT_PROTOBUF_STATIC
#endif //Q_PROTOBUF_IMPORT_QUICK_PLUGIN

#include <memory>

namespace QtProtobuf {
class QGrpcCallReply;
class QGrpcStream;
using QGrpcCallReplyShared = std::shared_ptr<QGrpcCallReply>;
using QGrpcStreamShared = std::shared_ptr<QGrpcStream>;
}
