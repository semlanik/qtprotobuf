/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
 *
 * This file is part of qtprotobuf project https://git.semlanik.org/semlanik/qtprotobuf
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

#include "generator.h"
#include "templates.h"
#include "classgeneratorbase.h"

#include <google/protobuf/stubs/logging.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.h>

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <string>

using namespace ::google::protobuf;
using namespace ::google::protobuf::compiler;

using namespace qtprotobuf;

class QtClassGenerator : public ClassGeneratorBase
{
    std::string mPackage;
    const Descriptor* mMessage;

public:
    QtClassGenerator(const std::string& package, const Descriptor* message, std::unique_ptr<io::ZeroCopyOutputStream> out) :
        ClassGeneratorBase(message->name(), std::move(out))
      , mPackage(std::move(package))
      ,mMessage(message){}

    void run() {
        printPreamble();
        printIncludes(mMessage);
        printNamespaces(mPackage);
        printClass();
        printProperties(mMessage);
        enclose();
    }
};

class GlobalEnumsGenerator : public ClassGeneratorBase
{
    const FileDescriptor* mFile;

public:
    GlobalEnumsGenerator(const FileDescriptor* file, std::unique_ptr<io::ZeroCopyOutputStream> out) :
        ClassGeneratorBase("GlobalEnums", std::move(out))
    , mFile(file)
    {}

    void run() {
        printPreamble();
        printNamespaces(mFile->package());
        printClass();
        printQEnums<FileDescriptor>(mFile);
        enclose();
    }
};

bool QtGenerator::Generate(const FileDescriptor* file,
                           const std::string& /*parameter*/,
                           GeneratorContext* generator_context,
                           std::string* error) const
{
    if (file->syntax() != FileDescriptor::SYNTAX_PROTO3) {
        *error = "Invalid proto used. This plugin only supports 'proto3' syntax";
        return false;
    }

    for(int i = 0; i < file->message_type_count(); i++) {
        const Descriptor* message = file->message_type(i);
        std::string filename = message->name() + ".h";
        std::transform(std::begin(filename), std::end(filename), std::begin(filename), ::tolower);
        QtClassGenerator classGen(file->package(), message,
                                  std::move(std::unique_ptr<io::ZeroCopyOutputStream>(generator_context->Open(filename))));
        classGen.run();
    }

    std::string globalEnumsFilename = "globalenums.h";
    GlobalEnumsGenerator enumGen(file, std::move(std::unique_ptr<io::ZeroCopyOutputStream>(generator_context->Open(globalEnumsFilename))));
    enumGen.run();
    return true;
}
