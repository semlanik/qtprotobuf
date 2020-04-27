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

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <string>
#include <memory>
#include <functional>
#include "generatorbase.h"

namespace google { namespace protobuf {
class FileDescriptor;
class Descriptor;
namespace compiler {
class GeneratorContext;
}}}

namespace QtProtobuf {
namespace generator {

/*!
 * \ingroup generator
 * \private
 * \brief The SingleFileGenerator class
 */
class SingleFileGenerator : public GeneratorBase
{
public:
    SingleFileGenerator();
    bool Generate(const ::google::protobuf::FileDescriptor *file,
                          const std::string &parameter,
                          ::google::protobuf::compiler::GeneratorContext *generatorContext,
                          std::string *error) const override;
private:
    bool GenerateServices(const ::google::protobuf::FileDescriptor *file,
                          const std::string &parameter,
                          ::google::protobuf::compiler::GeneratorContext *generatorContext,
                          std::string *error) const;

    bool GenerateMessages(const ::google::protobuf::FileDescriptor *file,
                          const std::string &parameter,
                          ::google::protobuf::compiler::GeneratorContext *generatorContext,
                          std::string *error) const;
};

} //namespace generator
} // namespace QtProtobuf
