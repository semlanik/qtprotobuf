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

namespace google { namespace protobuf {
class FileDescriptor;
namespace compiler {
class GeneratorContext;
}}}

namespace QtProtobuf {
namespace generator {

class QtGenerator : public ::google::protobuf::compiler::CodeGenerator
{
    bool Generate(const ::google::protobuf::FileDescriptor *file,
                          const std::string &parameter,
                          ::google::protobuf::compiler::GeneratorContext *generatorContext,
                          std::string *error) const override;

    bool GenerateAll(const std::vector<const ::google::protobuf::FileDescriptor *> &files,
                             const std::string &parameter,
                             ::google::protobuf::compiler::GeneratorContext *generatorContext,
                             std::string *error) const override;
    bool HasGenerateAll() const override { return true; }
};

} //namespace generator
} // namespace QtProtobuf
