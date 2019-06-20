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

#include "qtprotobuftypes.h"
#include "qprotobufobject.h"
#include "qprotobufregistrationhelper.h"

#include <type_traits>

#define registerProtobufType(X) qRegisterMetaType<X>(# X);\
                                qRegisterMetaType<X>("QtProtobuf::"# X);

namespace QtProtobuf {

namespace  {

template<typename T, typename std::enable_if_t<sizeof(T) == sizeof(int32_t)
                                      && std::is_signed<T>::value, int> = 0>
void registerBasicConverters() {
    QMetaType::registerConverter<int32_t, T>(T::fromType);
    QMetaType::registerConverter<T, int32_t>(T::toType);
    QMetaType::registerConverter<double, T>(T::fromType);
    QMetaType::registerConverter<T, double>(T::toType);
    QMetaType::registerConverter<T, QString>(T::toString);
}

template<typename T, typename std::enable_if_t<sizeof(T) == sizeof(int64_t)
                                      && std::is_signed<T>::value, int> = 0>
void registerBasicConverters() {
    QMetaType::registerConverter<int64_t, T>(T::fromType);
    QMetaType::registerConverter<T, int64_t>(T::toType);
    QMetaType::registerConverter<qint64, T>(T::fromType);
    QMetaType::registerConverter<T, qint64>(T::toType);
    QMetaType::registerConverter<int32_t, T>(T::fromType);
    QMetaType::registerConverter<T, QString>(T::toString);
}

template<typename T, typename std::enable_if_t<sizeof(T) == sizeof(int32_t)
                                      && !std::is_signed<T>::value, int> = 0>
void registerBasicConverters() {
    QMetaType::registerConverter<uint32_t, T>(T::fromType);
    QMetaType::registerConverter<T, uint32_t>(T::toType);
    QMetaType::registerConverter<int32_t, T>(T::fromType);
    QMetaType::registerConverter<T, int32_t>(T::toType);
    QMetaType::registerConverter<double, T>(T::fromType);
    QMetaType::registerConverter<T, double>(T::toType);
    QMetaType::registerConverter<T, QString>(T::toString);
}

template<typename T, typename std::enable_if_t<sizeof(T) == sizeof(int64_t)
                                      && !std::is_signed<T>::value, int> = 0>
void registerBasicConverters() {
    QMetaType::registerConverter<uint64_t, T>(T::fromType);
    QMetaType::registerConverter<T, uint64_t>(T::toType);
    QMetaType::registerConverter<quint64, T>(T::fromType);
    QMetaType::registerConverter<T, quint64>(T::toType);
    QMetaType::registerConverter<uint32_t, T>(T::fromType);
    QMetaType::registerConverter<T, QString>(T::toString);
}

void registerTypes() {
    registerProtobufType(int32);
    registerProtobufType(int64);
    registerProtobufType(uint32);
    registerProtobufType(uint64);
    registerProtobufType(sint32);
    registerProtobufType(sint64);
    registerProtobufType(fixed32);
    registerProtobufType(fixed64);
    registerProtobufType(sfixed32);
    registerProtobufType(sfixed64);

    registerProtobufType(int32List);
    registerProtobufType(int64List);
    registerProtobufType(uint32List);
    registerProtobufType(uint64List);
    registerProtobufType(sint32List);
    registerProtobufType(sint64List);
    registerProtobufType(fixed32List);
    registerProtobufType(fixed64List);
    registerProtobufType(sfixed32List);
    registerProtobufType(sfixed64List);

    registerProtobufType(DoubleList);
    registerProtobufType(FloatList);

    registerBasicConverters<int32>();
    registerBasicConverters<int64>();
    registerBasicConverters<sfixed32>();
    registerBasicConverters<sfixed64>();
    registerBasicConverters<fixed32>();
    registerBasicConverters<fixed64>();
}
}

static QProtobufRegistrationHelper helper(registerTypes);
}
