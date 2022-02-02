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

#define registerProtobufType(X) qRegisterMetaType<X>(# X)

namespace QtProtobuf {

template<typename T>
void registerBasicConverters() {
    QMetaType::registerConverter<int32_t, T>(T::fromType);
    QMetaType::registerConverter<T, int32_t>(T::toType);
    QMetaType::registerConverter<int64_t, T>(T::fromType);
    QMetaType::registerConverter<T, int64_t>(T::toType);
    QMetaType::registerConverter<uint32_t, T>(T::fromType);
    QMetaType::registerConverter<T, uint32_t>(T::toType);
    QMetaType::registerConverter<uint64_t, T>(T::fromType);
    QMetaType::registerConverter<T, uint64_t>(T::toType);
    QMetaType::registerConverter<qulonglong, T>(T::fromType);
    QMetaType::registerConverter<T, qulonglong>(T::toType);
    QMetaType::registerConverter<qlonglong, T>(T::fromType);
    QMetaType::registerConverter<T, qlonglong>(T::toType);
    QMetaType::registerConverter<double, T>(T::fromType);
    QMetaType::registerConverter<T, double>(T::toType);
    QMetaType::registerConverter<T, QString>(T::toString);
}

std::list<QtProtobuf::RegisterFunction>& registerFunctions() {
    static std::list<std::function<void(void)>> registrationList;
    return registrationList;
}

} //namespace QtProtobuf

void qRegisterProtobufTypes() {
    static bool registred = false;
    if (registred) {
        return;
    }
    registred = true;
    registerProtobufType(QtProtobuf::int32);
    registerProtobufType(QtProtobuf::int64);
    registerProtobufType(QtProtobuf::uint32);
    registerProtobufType(QtProtobuf::uint64);
    registerProtobufType(QtProtobuf::sint32);
    registerProtobufType(QtProtobuf::sint64);
    registerProtobufType(QtProtobuf::fixed32);
    registerProtobufType(QtProtobuf::fixed64);
    registerProtobufType(QtProtobuf::sfixed32);
    registerProtobufType(QtProtobuf::sfixed64);

    registerProtobufType(QtProtobuf::int32List);
    registerProtobufType(QtProtobuf::int64List);
    registerProtobufType(QtProtobuf::uint32List);
    registerProtobufType(QtProtobuf::uint64List);
    registerProtobufType(QtProtobuf::sint32List);
    registerProtobufType(QtProtobuf::sint64List);
    registerProtobufType(QtProtobuf::fixed32List);
    registerProtobufType(QtProtobuf::fixed64List);
    registerProtobufType(QtProtobuf::sfixed32List);
    registerProtobufType(QtProtobuf::sfixed64List);

    registerProtobufType(QtProtobuf::DoubleList);
    registerProtobufType(QtProtobuf::FloatList);

    QtProtobuf::registerBasicConverters<QtProtobuf::int32>();
    QtProtobuf::registerBasicConverters<QtProtobuf::int64>();
    QtProtobuf::registerBasicConverters<QtProtobuf::sfixed32>();
    QtProtobuf::registerBasicConverters<QtProtobuf::sfixed64>();
    QtProtobuf::registerBasicConverters<QtProtobuf::fixed32>();
    QtProtobuf::registerBasicConverters<QtProtobuf::fixed64>();

    for (auto registerFunc : QtProtobuf::registerFunctions()) {
        registerFunc();
    }
}

QT_END_NAMESPACE
