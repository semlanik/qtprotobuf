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

#include "google/protobuf/any.qpb.h"
#include "google/protobuf/api.qpb.h"
#include "google/protobuf/duration.qpb.h"
#include "google/protobuf/empty.qpb.h"
#include "google/protobuf/field_mask.qpb.h"
#include "google/protobuf/source_context.qpb.h"
#include "google/protobuf/struct.qpb.h"
#include "google/protobuf/timestamp.qpb.h"
#include "google/protobuf/type.qpb.h"
#include "google/protobuf/wrappers.qpb.h"

namespace google {

namespace protobuf {

inline void qRegisterProtobufTypes() {
    NullValueGadget::registerTypes();
    SyntaxGadget::registerTypes();
    qRegisterProtobufType<Any>();
    qRegisterProtobufType<Api>();
    qRegisterProtobufType<Method>();
    qRegisterProtobufType<Mixin>();
    qRegisterProtobufType<Duration>();
    qRegisterProtobufType<Empty>();
    qRegisterProtobufType<FieldMask>();
    qRegisterProtobufType<SourceContext>();
    qRegisterProtobufType<Struct>();
    qRegisterProtobufType<Value>();
    qRegisterProtobufType<ListValue>();
    qRegisterProtobufType<Timestamp>();
    qRegisterProtobufType<Type>();
    qRegisterProtobufType<Field>();
    qRegisterProtobufType<Enum>();
    qRegisterProtobufType<EnumValue>();
    qRegisterProtobufType<Option>();
    qRegisterProtobufType<DoubleValue>();
    qRegisterProtobufType<FloatValue>();
    qRegisterProtobufType<Int64Value>();
    qRegisterProtobufType<UInt64Value>();
    qRegisterProtobufType<Int32Value>();
    qRegisterProtobufType<UInt32Value>();
    qRegisterProtobufType<StringValue>();
    qRegisterProtobufType<BytesValue>();
}

}

}
