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
#include "servergenerator.h"
#include "utils.h"

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
#include <list>

using namespace ::google::protobuf;
using namespace ::google::protobuf::compiler;

using namespace qtprotobuf;

class QtClassGenerator : public ClassGeneratorBase
{
    std::string mPackage;
    const Descriptor* mMessage;
    std::set<std::string> mExtractedModels;

public:
    QtClassGenerator(const std::string &package, const Descriptor *message, std::unique_ptr<io::ZeroCopyOutputStream> out) :
        ClassGeneratorBase(message->name(), std::move(out))
      , mPackage(std::move(package))
      , mMessage(message){}

    void run() {
        //Post generation collect all generated model types
        for (int i = 0; i < mMessage->field_count(); i++) {
            const FieldDescriptor* field = mMessage->field(i);
            if (field->is_repeated()
                    && field->type() == FieldDescriptor::TYPE_MESSAGE) {
                std::string typeName = field->message_type()->name();
                mExtractedModels.insert(typeName);
            }
        }

        printPreamble();
        printIncludes(mMessage, mExtractedModels);
        printNamespaces(mPackage);
        printClass();
        printProperties(mMessage);
        printPublic();
        printFieldsOrderingDefinition();
        encloseClass();
        enclose();
        printMetaTypeDeclaration(mPackage);
    }

    const std::set<std::string> &extractedModels() const {
        return mExtractedModels;
    }

    void printFieldsOrderingDefinition() {
        Indent();
        mPrinter.Print(FieldsOrderingDefinitionContainerTemplate);
        Outdent();
    }


};

class QtSourcesGenerator : public ClassGeneratorBase
{
    std::string mPackage;
    const Descriptor* mMessage;
    std::set<std::string> mExtractedModels;
public:
    QtSourcesGenerator(const std::string &package, const Descriptor *message, std::unique_ptr<io::ZeroCopyOutputStream> out) :
        ClassGeneratorBase(message->name(), std::move(out))
      , mPackage(std::move(package))
      , mMessage(message){}

    void run() {
        printClassHeaderInclude();
        printNamespaces(mPackage);
        printFieldsOrdering();
        enclose();
    }

    void printClassHeaderInclude() {
        std::string includeFileName = mClassName;
        utils::tolower(includeFileName);
        mPrinter.Print({{"type_lower", includeFileName}}, InternalIncludeTemplate);
    }

    void printFieldsOrdering() {
        mPrinter.Print({{"type", mClassName}}, FieldsOrderingContainerTemplate);
        Indent();
        for (int i = 0; i < mMessage->field_count(); i++) {
            const FieldDescriptor* field = mMessage->field(i);
            if (i != 0) {
                mPrinter.Print("\n,");
            }
            //property_number is incremented by 1 because user properties stating from 1.
            //Property with index 0 is "objectName"
            mPrinter.Print({{"field_number", std::to_string(field->number())},
                            {"property_number", std::to_string(i + 1)}}, FieldOrderTemplate);
        }
        Outdent();
        mPrinter.Print(SemicolonBlockEnclosureTemplate);
    }
};

class GlobalEnumsGenerator : public ClassGeneratorBase
{
    const FileDescriptor *mFile;

public:
    GlobalEnumsGenerator(std::unique_ptr<io::ZeroCopyOutputStream> out) :
        ClassGeneratorBase("GlobalEnums", std::move(out))
    {
        printPreamble();
    }

    void startEnum(std::string package) {
        printNamespaces(package);
        printEnumClass();
    }

    void run(const FileDescriptor *file) {
        printQEnums<FileDescriptor>(file);
    }

    void encloseEnum() {
        encloseClass();
        enclose();
    }

    void printEnumClass() {
        mPrinter.Print({{"classname", mClassName}}, NonProtoClassDefinitionTemplate);
    }
};

bool QtGenerator::Generate(const FileDescriptor *file,
                           const std::string &/*parameter*/,
                           GeneratorContext *generatorContext,
                           std::string *error) const
{
    if (file->syntax() != FileDescriptor::SYNTAX_PROTO3) {
        *error = "Invalid proto used. This plugin only supports 'proto3' syntax";
        return false;
    }

    std::set<std::string> extractedModels;

    for(int i = 0; i < file->message_type_count(); i++) {
        const Descriptor *message = file->message_type(i);
        std::string baseFilename(message->name());
        utils::tolower(baseFilename);

        std::string filename = baseFilename + ".h";
        QtClassGenerator classGen(file->package(), message,
                                  std::move(std::unique_ptr<io::ZeroCopyOutputStream>(generatorContext->Open(filename))));
        classGen.run();
        extractedModels.insert(std::begin(classGen.extractedModels()), std::end(classGen.extractedModels()));

        std::string sourceFileName = baseFilename + ".cpp";
        QtSourcesGenerator classSourceGen(file->package(), message,
                                  std::move(std::unique_ptr<io::ZeroCopyOutputStream>(generatorContext->Open(sourceFileName))));
        classSourceGen.run();
    }

    for(int i = 0; i < file->service_count(); i++) {
        const ServiceDescriptor* service = file->service(i);
        std::string baseFilename(service->name());
        utils::tolower(baseFilename);

        std::string headeFilename = baseFilename + ".h";
        ServerGenerator serverGen(file->package(), service,
                                  std::move(std::unique_ptr<io::ZeroCopyOutputStream>(generatorContext->Open(headeFilename))));
        serverGen.run();


    }
    return true;
}

bool QtGenerator::GenerateAll(const std::vector<const FileDescriptor *> &files, const string &parameter, GeneratorContext *generatorContext, string *error) const
{
    std::string globalEnumsFilename = "globalenums.h";
    GlobalEnumsGenerator enumGen(std::move(std::unique_ptr<io::ZeroCopyOutputStream>(generatorContext->Open(globalEnumsFilename))));
    std::unordered_map<std::string/*package*/, std::list<const FileDescriptor *>> packageList;
    for (auto file : files) {
        packageList[file->package()].push_back(file);
    }

    for (auto package : packageList) {
        enumGen.startEnum(package.first);
        for (auto file : package.second) {
            enumGen.run(file);
        }
        enumGen.encloseEnum();
    }

    return CodeGenerator::GenerateAll(files, parameter, generatorContext, error);
}

