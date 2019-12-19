/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Tatyana Borisova <tanusshhka@mail.ru>
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

#include <google/protobuf/io/printer.h>
#include <memory>
#include <list>

#include "utils.h"
#include "templates.h"

namespace google { namespace protobuf {
class FieldDescriptor;
class Descriptor;
class MethodDescriptor;
namespace io {
class ZeroCopyOutputStream;
}}}

namespace QtProtobuf {
namespace generator {

using PropertyMap = std::map<std::string, std::string>;

/*!
 * \ingroup generator
 * \private
 * \brief The ClassGeneratorBase class is base of source code generation
 */
class ClassGeneratorBase
{
public:
    ClassGeneratorBase(const std::string &fullClassName, const std::shared_ptr<google::protobuf::io::ZeroCopyOutputStream> &out);
    ClassGeneratorBase(const std::string &fullClassName, const std::shared_ptr<::google::protobuf::io::Printer> &printer);
    virtual ~ClassGeneratorBase() = default;
    virtual void run() = 0;
protected:
    enum EnumVisibility {
        GLOBAL_ENUM,
        LOCAL_ENUM,
        NEIGHBOUR_ENUM
    };

    std::shared_ptr<::google::protobuf::io::ZeroCopyOutputStream> mOutput;
    std::shared_ptr<::google::protobuf::io::Printer> mPrinter;
    std::string mClassName;
    std::vector<std::string> mNamespaces;
    std::string mNamespacesColonDelimited;

public:
    void printDisclaimer();
    void printPreamble();
    void printNamespaces();
    void printNamespaces(const std::vector<std::string> &namespaces);
    void printClassDeclaration();
    void printPublic();
    void printPrivate();
    void encloseClass();
    void printMetaTypeDeclaration();
    void encloseNamespaces();
    void encloseNamespaces(int count);
    void printInclude(const google::protobuf::Descriptor *message, const google::protobuf::FieldDescriptor *field, std::set<std::string> &existingIncludes);
    bool hasGlobalEnum(const std::list<const ::google::protobuf::FileDescriptor *> &list);
    void printField(const google::protobuf::Descriptor *message, const ::google::protobuf::FieldDescriptor *field, const char *fieldTemplate);
    bool producePropertyMap(const google::protobuf::Descriptor *message, const ::google::protobuf::FieldDescriptor *field, PropertyMap &propertyMap);
    bool isComplexType(const google::protobuf::FieldDescriptor *field);
    std::string qualifiedName(const std::string &name);

    template<typename T>
    void printQEnums(const T *message) {
        if (message->enum_type_count() <= 0) {
            return;
        }

        printPublic();

        Indent();
        for (int i = 0; i < message->enum_type_count(); i++) {
            const auto enumDescr = message->enum_type(i);
            mPrinter->Print({{"enum", enumDescr->name()}}, Templates::EnumDefinitionTemplate);

            Indent();
            for (int j = 0; j < enumDescr->value_count(); j++) {
                const auto valueDescr = enumDescr->value(j);
                mPrinter->Print({{"enumvalue", utils::upperCaseName(valueDescr->name())},
                                 {"value", std::to_string(valueDescr->number())}}, Templates::EnumFieldTemplate);
            }
            Outdent();
            mPrinter->Print(Templates::SemicolonBlockEnclosureTemplate);
            mPrinter->Print({{"type", enumDescr->name().c_str()}}, Templates::QEnumTemplate);
        }

        for (int i = 0; i < message->enum_type_count(); i++) {
            const auto enumDescr = message->enum_type(i);
            mPrinter->Print({{"enum", enumDescr->name()}}, Templates::EnumTypeRepeatedTemplate);
        }
        Outdent();
    }


    void Indent() {
        mPrinter->Indent();
        mPrinter->Indent();
    }

    void Outdent() {
        mPrinter->Outdent();
        mPrinter->Outdent();
    }

    std::string getTypeName(const ::google::protobuf::FieldDescriptor *field, const ::google::protobuf::Descriptor *messageFor);
    static bool isLocalMessageEnum(const google::protobuf::Descriptor *message,
                            const ::google::protobuf::FieldDescriptor *field);
    template<typename T>
    static std::string getNamespacesList(const T *message, std::vector<std::string> &container, const std::string &localNamespace);
    static EnumVisibility getEnumVisibility(const ::google::protobuf::FieldDescriptor *field, const ::google::protobuf::Descriptor *messageFor);
    void getMethodParameters(const ::google::protobuf::MethodDescriptor *method, std::map<std::string, std::string> &parameters);
};

} //namespace generator
} //namespace QtProtobuf
