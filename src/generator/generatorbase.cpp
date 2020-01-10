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

#include "generatorbase.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/logging.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include "utils.h"
#include "templates.h"
#include "generatoroptions.h"

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::compiler;

GeneratorBase::GeneratorBase(Mode mode) : m_mode(mode)
{

}
void GeneratorBase::iterateNonNestedFileds(const ::google::protobuf::FileDescriptor *file, std::function<void(const ::google::protobuf::Descriptor *)> callback) const
{
    for (int i = 0; i < file->message_type_count(); i++) {
        const Descriptor *message = file->message_type(i);

        //Detect nested fields and filter maps fields
        int mapsFieldsCount = 0;
        for (int j = 0; j < message->nested_type_count(); j++) {
            for (int k = 0; k < message->field_count(); k++) {
                if (message->field(k)->is_map() && message->field(k)->message_type() == message->nested_type(j)) {
                    ++mapsFieldsCount;
                }
            }
        }

        if (message->nested_type_count() > 0 && message->nested_type_count() > mapsFieldsCount) {
            std::cerr << file->name() << ":" << (message->index() + 1) << ": " << " Error: Meta object features not supported for nested classes in " << message->full_name() << std::endl;
            continue;
        }
        callback(message);
    }
}

bool GeneratorBase::GenerateAll(const std::vector<const FileDescriptor *> &files, const string &parameter, GeneratorContext *generatorContext, string *error) const
{
    return CodeGenerator::GenerateAll(files, parameter, generatorContext, error);
}
