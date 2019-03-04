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
        printIncludes(mMessage);

        if (mExtractedModels.size() > 0) {
            mPrinter.Print(ListModelsIncludeTemplate);
        }

        printNamespaces(mPackage);
        printClass();
        printProperties(mMessage);
        printPublic();
        printFieldsOrderingDefinition();
        encloseClass();
        enclose();
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
        std::transform(std::begin(includeFileName), std::end(includeFileName), std::begin(includeFileName), ::tolower);
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
    GlobalEnumsGenerator(const FileDescriptor *file, std::unique_ptr<io::ZeroCopyOutputStream> out) :
        ClassGeneratorBase("GlobalEnums", std::move(out))
    , mFile(file)
    {}

    void run() {
        printPreamble();
        printNamespaces(mFile->package());
        printClass();
        printQEnums<FileDescriptor>(mFile);
        encloseClass();
        enclose();
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
        std::transform(std::begin(baseFilename), std::end(baseFilename), std::begin(baseFilename), ::tolower);

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

    std::string globalEnumsFilename = "globalenums.h";
    GlobalEnumsGenerator enumGen(file, std::move(std::unique_ptr<io::ZeroCopyOutputStream>(generatorContext->Open(globalEnumsFilename))));
    enumGen.run();

    //TODO: move to separate class also slipt definitions
    //Print list models
    std::unique_ptr<io::ZeroCopyOutputStream> out(generatorContext->Open("listmodels.h"));
    io::Printer printer(out.get(), '$');
    printer.Print(PreambleTemplate);

    for(auto modelTypeName : extractedModels) {
        std::string modelTypeNameLower(modelTypeName);
        std::transform(std::begin(modelTypeNameLower), std::end(modelTypeNameLower), std::begin(modelTypeNameLower), ::tolower);
        printer.Print({{"type_lower", modelTypeNameLower}}, InternalIncludeTemplate);
    }

    printer.Print(UniversalListModelIncludeTemplate);
    for(auto modelTypeName : extractedModels) {
        printer.Print({{"type", modelTypeName}}, ModelClassTemplate);
    }

    return true;
}
