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

#pragma once

#include <google/protobuf/io/printer.h>

#include "templates.h"

namespace google { namespace protobuf {
class FieldDescriptor;
class Descriptor;
namespace io {
class ZeroCopyOutputStream;
}}}

namespace qtprotobuf {

using PropertyMap = std::map<std::string, std::string>;

class ClassGeneratorBase
{
public:
    ClassGeneratorBase(std::string mClassName, std::unique_ptr<::google::protobuf::io::ZeroCopyOutputStream> out);

protected:
    std::unique_ptr<::google::protobuf::io::ZeroCopyOutputStream> mOutput;
    ::google::protobuf::io::Printer mPrinter;
    std::string mClassName;
    int mNamespaceCount;

    bool producePropertyMap(const ::google::protobuf::FieldDescriptor *field, PropertyMap &propertyMap);
    void printPreamble();
    void printIncludes(const ::google::protobuf::Descriptor *message);
    void printNamespaces(const std::string &package);
    void printClass();
    void printField(const ::google::protobuf::FieldDescriptor *field, const char *fieldTemplate);
    void enclose();
    std::string getTypeName(const ::google::protobuf::FieldDescriptor *field);

    template<typename T>
    void printQEnums(const T *message) {
        if (message->enum_type_count() <= 0) {
            return;
        }

        Indent();
        mPrinter.Print("Q_ENUMS(");
        for (int i = 0; i < message->enum_type_count(); i++) {
            const auto enumDescr = message->enum_type(i);
            mPrinter.Print(enumDescr->name().c_str());
        }
        mPrinter.Print(")\n");
        Outdent();

        printPublic();

        Indent();
        for (int i = 0; i < message->enum_type_count(); i++) {
            const auto enumDescr = message->enum_type(i);
            mPrinter.Print({{"enum", enumDescr->name()}}, EnumDefinitionTemplate);
            Indent();
            for (int j = 0; j < enumDescr->value_count(); j++) {
                const auto valueDescr = enumDescr->value(j);
                mPrinter.Print({{"enumvalue", valueDescr->name()},
                                {"value", std::to_string(valueDescr->number())}}, EnumFieldTemplate);
            }
            Outdent();
            mPrinter.Print(SemicolonBlockEnclosureTemplate);
        }
        Outdent();
    }

    void printCopyFunctionality(const ::google::protobuf::Descriptor *message);
    void printEqualOperator(const ::google::protobuf::Descriptor *message);
    void printProperties(const ::google::protobuf::Descriptor *message);
    void printConstructor();
    void printPublic();
    void Indent() {
        mPrinter.Indent();
        mPrinter.Indent();
    }

    void Outdent() {
        mPrinter.Outdent();
        mPrinter.Outdent();
    }
};

}
