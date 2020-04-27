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

#include "classgeneratorbase.h"
#include <google/protobuf/io/printer.h>
#include <memory>

#include "templates.h"

namespace google { namespace protobuf {
class FieldDescriptor;
class Descriptor;
namespace io {
class ZeroCopyOutputStream;
}}}

namespace QtProtobuf {
namespace generator {

using PropertyMap = std::map<std::string, std::string>;

/*!
 * \ingroup generator
 * \private
 * \brief The ProtobufClassGenerator class
 */
class ProtobufClassGenerator : public ClassGeneratorBase
{
    const ::google::protobuf::Descriptor *mMessage;
public:
    ProtobufClassGenerator(const ::google::protobuf::Descriptor *message, const std::shared_ptr<google::protobuf::io::ZeroCopyOutputStream> &out);
    ProtobufClassGenerator(const ::google::protobuf::Descriptor *message, const std::shared_ptr<::google::protobuf::io::Printer> &printer);
    virtual ~ProtobufClassGenerator() = default;

    void run() override;

    void printIncludes();
    void printCopyFunctionality();
    void printMoveSemantic();
    void printComparisonOperators();
    void printProperties();
    void printClassMembers();
    void printConstructor();
    void printDestructor();
    void printListType();
    void printMaps();
    void printMapsMetaTypesDeclaration();
    void printLocalEnumsMetaTypesDeclaration();
    void printFieldClassDeclaration();

    std::set<std::string> extractModels() const;

private:
    static bool isListType(const ::google::protobuf::FieldDescriptor *field);
};

}
}
