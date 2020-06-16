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
#include "generatorcommon.h"

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

std::string GeneratorBase::generateBaseName(const ::google::protobuf::FileDescriptor *file, std::string name)
{
    std::vector<std::string> packages = utils::split(file->package(), '.');
    std::string outFileBasename = "";
    if (GeneratorOptions::instance().isFolder()) {
        for (auto package : packages) {
            outFileBasename += package + "/";
        }
        outFileBasename += name;
    } else {
        outFileBasename = name;
    }

    return outFileBasename;
}


void GeneratorBase::printDisclaimer(const std::shared_ptr<::google::protobuf::io::Printer> printer)
{
    printer->Print(Templates::DisclaimerTemplate);
}

void GeneratorBase::printPreamble(const std::shared_ptr<::google::protobuf::io::Printer> printer)
{
    printer->Print(Templates::PreambleTemplate);
}

void GeneratorBase::printInclude(const std::shared_ptr<::google::protobuf::io::Printer> printer, const google::protobuf::Descriptor *message, const FieldDescriptor *field, std::set<std::string> &existingIncludes)
{
    assert(field != nullptr);
    std::string newInclude;
    const char *includeTemplate;
    switch (field->type()) {
    case FieldDescriptor::TYPE_MESSAGE:
        if (field->is_map()) {
            newInclude = "QMap";
            assert(field->message_type() != nullptr);
            assert(field->message_type()->field_count() == 2);
            printInclude(printer, message, field->message_type()->field(0), existingIncludes);
            printInclude(printer, message, field->message_type()->field(1), existingIncludes);
            includeTemplate = Templates::ExternalIncludeTemplate;
        } else {
            std::string outFileBasename = "";
            std::string fieldPackage = field->message_type()->file()->package();
            if (fieldPackage != message->file()->package()) {
                std::vector<std::string> packages = utils::split(fieldPackage, '.');
                for (auto package : packages) {
                    outFileBasename += package + "/";
                }
            }

            std::string typeName = field->message_type()->name();
            utils::tolower(typeName);
            newInclude = outFileBasename + typeName;
            includeTemplate = Templates::InternalIncludeTemplate;
        }
        break;
    case FieldDescriptor::TYPE_BYTES:
        newInclude = "QByteArray";
        includeTemplate = Templates::ExternalIncludeTemplate;
        break;
    case FieldDescriptor::TYPE_STRING:
        newInclude = "QString";
        includeTemplate = Templates::ExternalIncludeTemplate;
        break;
    case FieldDescriptor::TYPE_ENUM: {
        common::EnumVisibility enumVisibily = common::enumVisibility(field->enum_type(), message);
        if (enumVisibily == common::GLOBAL_ENUM) {
            includeTemplate = Templates::GlobalEnumIncludeTemplate;
        } else if (enumVisibily == common::NEIGHBOR_ENUM) {
            includeTemplate = Templates::InternalIncludeTemplate;
            std::string fullEnumName = field->enum_type()->full_name();
            std::vector<std::string> fullEnumNameParts = utils::split(fullEnumName, '.');
            std::string enumTypeOwner = fullEnumNameParts.at(fullEnumNameParts.size() - 2);
            utils::tolower(enumTypeOwner);
            newInclude = enumTypeOwner;
        } else {
            return;
        }
    }
        break;
    default:
        return;
    }

    if (existingIncludes.find(newInclude) == std::end(existingIncludes)) {
        printer->Print({{"include", newInclude}}, includeTemplate);
        existingIncludes.insert(newInclude);
    }
}

void GeneratorBase::printQtProtobufUsingNamespace(const std::shared_ptr<::google::protobuf::io::Printer> printer)
{
    printer->Print({{"namespace", "QtProtobuf"}}, Templates::UsingNamespaceTemplate);
}

void GeneratorBase::printNamespaces(const std::shared_ptr<::google::protobuf::io::Printer> printer, const std::vector<std::string> namespaces) {
    printer->Print("\n");
    for (auto ns : namespaces) {
        printer->Print({{"namespace", ns}}, Templates::NamespaceTemplate);
    }
}

