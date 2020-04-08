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
