/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Tatyana Borisova <tanusshhka@mail.ru>
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

#include "classgeneratorbase.h"

#include "templates.h"
#include "utils.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <set>

using namespace ::qtprotobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

ClassGeneratorBase::ClassGeneratorBase(std::string fullClassName, std::unique_ptr<::google::protobuf::io::ZeroCopyOutputStream> out) : mOutput(std::move(out))
  , mPrinter(mOutput.get(), '$')
{
    utils::split(fullClassName, mNamespaces, '.');
    assert(mNamespaces.size() > 0);
    mClassName = mNamespaces.back();
    mNamespaces.erase(mNamespaces.end() - 1);
    for (size_t i = 0; i < mNamespaces.size(); i++) {
        if (i > 0) {
            mNamespacesColonDelimited = mNamespacesColonDelimited.append("::");
        }
        mNamespacesColonDelimited = mNamespacesColonDelimited.append(mNamespaces[i]);
    }
}

void ClassGeneratorBase::printPreamble()
{
    mPrinter.Print(Templates::PreambleTemplate);
}

void ClassGeneratorBase::printNamespaces()
{
    printNamespaces(mNamespaces);
}

void ClassGeneratorBase::printNamespaces(const std::vector<std::string> &namespaces)
{
    for (auto ns: namespaces) {
        mPrinter.Print({{"namespace", ns}}, Templates::NamespaceTemplate);
    }
}

void ClassGeneratorBase::printClassDeclaration()
{
    mPrinter.Print({{"classname", mClassName}}, Templates::ProtoClassDefinitionTemplate);
}

void ClassGeneratorBase::encloseClass()
{
    mPrinter.Print(Templates::SemicolonBlockEnclosureTemplate);
}

void ClassGeneratorBase::encloseNamespaces(int count)
{
    for (int i = 0; i < count; i++) {
        mPrinter.Print(Templates::SimpleBlockEnclosureTemplate);
    }
}

void ClassGeneratorBase::encloseNamespaces()
{
    encloseNamespaces(mNamespaces.size());
}

void ClassGeneratorBase::printPublic()
{
    mPrinter.Print(Templates::PublicBlockTemplate);
}

void ClassGeneratorBase::printPrivate()
{
    mPrinter.Print(Templates::PrivateBlockTemplate);
}

void ClassGeneratorBase::printMetaTypeDeclaration()
{
    mPrinter.Print({{"classname", mClassName}, {"namespaces", mNamespacesColonDelimited}},
                   Templates::DeclareMetaTypeTemplate);
    mPrinter.Print({{"classname", mClassName}, {"namespaces", mNamespacesColonDelimited}},
                   Templates::DeclareComplexListTypeTemplate);
}

bool ClassGeneratorBase::isLocalMessageEnum(const google::protobuf::Descriptor *message,
                                            const ::google::protobuf::FieldDescriptor *field)
{
    assert(field->enum_type() != nullptr);
    for (int i = 0; i < message->enum_type_count(); i++) {
        const auto enumDescr = message->enum_type(i);
        if (enumDescr && enumDescr->full_name() == field->enum_type()->full_name()) {
            return true;
        }
    }
    return false;
}

std::string ClassGeneratorBase::getTypeName(const FieldDescriptor *field, const Descriptor *messageFor)
{
    assert(field != nullptr);
    std::string typeName;
    std::string namespaceQtProtoDefinition("qtprotobuf::");

    std::string namespaceTypeName;
    std::vector<std::string> typeNamespace;

    if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
        const Descriptor *msg = field->message_type();
        namespaceTypeName = getNamespacesList(msg, typeNamespace, mNamespacesColonDelimited);
        typeName = namespaceTypeName.append(msg->name());

        if (field->is_map()) {
            return mClassName + "::" + field->message_type()->name();
        }
        if (field->is_repeated()) {
            return namespaceTypeName.append("List");
        }
    } else if (field->type() == FieldDescriptor::TYPE_ENUM) {
        const EnumDescriptor *enumType = field->enum_type();
        namespaceTypeName = getNamespacesList(enumType, typeNamespace, mNamespacesColonDelimited);
        EnumVisibility visibility = getEnumVisibility(field, messageFor);
        if (visibility == LOCAL_ENUM) {
            if(field->is_repeated()) {
                typeName = typeName.append(mClassName + "::" + enumType->name());
            } else {
                typeName = typeName.append(enumType->name());
            }
        } else if (visibility == GLOBAL_ENUM) {
            namespaceTypeName = getNamespacesList(enumType, typeNamespace, "");
            typeName = namespaceTypeName.append(Templates::GlobalEnumClassNameTemplate)
                    .append("::").append(enumType->name());
        } else {
            typeName = namespaceTypeName.append(enumType->name());
        }
        if (field->is_repeated()) {
            return typeName.append("List");
        }
    } else {
        auto it = Templates::TypeReflection.find(field->type());
        if (it != std::end(Templates::TypeReflection)) {
            if (field->type() != FieldDescriptor::TYPE_STRING
                    && field->type() != FieldDescriptor::TYPE_BYTES
                    && field->type() != FieldDescriptor::TYPE_BOOL
                    && field->type() != FieldDescriptor::TYPE_FLOAT
                    && field->type() != FieldDescriptor::TYPE_DOUBLE) {
                typeName = typeName.append(namespaceQtProtoDefinition.append(it->second));
            } else {
                typeName = typeName.append(it->second);
            }
            if (field->is_repeated()) {
                if (field->type() == FieldDescriptor::TYPE_FLOAT
                        || field->type() == FieldDescriptor::TYPE_DOUBLE) {
                    typeName[0] = ::toupper(typeName[0]);
                    typeName = namespaceQtProtoDefinition.append(typeName);
                }
                typeName.append("List");
            }
        }
    }

    return typeName;
}

template<typename T>
std::string ClassGeneratorBase::getNamespacesList(const T *message, std::vector<std::string> &container, const std::string &localNamespace)
{
    std::string result;
    utils::split(std::string(message->full_name()), container, '.');

    if (container.size() > 1) {
        //delete type name -> only namespace stays
        container.pop_back();

        for (size_t i = 0; i < container.size(); i++) {
            if (i > 0) {
                result = result.append("::");
            }
            result = result.append(container[i]);
        }
    }

    if (container.size() > 0
            && localNamespace != result) {
        result = result.append("::");
    } else {
        result.clear();
    }

    return result;
}

ClassGeneratorBase::EnumVisibility ClassGeneratorBase::getEnumVisibility(const FieldDescriptor *field, const Descriptor *messageFor)
{
    assert(field->enum_type() != nullptr);

    if (isLocalMessageEnum(messageFor, field)) {
        return LOCAL_ENUM;
    }

    const EnumDescriptor *enumType = field->enum_type();
    const FileDescriptor *enumFile = field->enum_type()->file();

    for (int i = 0; i < enumFile->message_type_count(); i++) {
        const Descriptor* msg = enumFile->message_type(i);
        for (int j = 0; j < msg->enum_type_count(); j++) {
            if (enumType->full_name() == msg->enum_type(j)->full_name()) {
                return NEIGHBOUR_ENUM;
            }
        }
    }

    return GLOBAL_ENUM;
}
