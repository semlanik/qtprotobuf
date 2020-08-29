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

#include "singlefilegenerator.h"
#include "messagedeclarationprinter.h"
#include "messagedefinitionprinter.h"
#include "enumdeclarationprinter.h"
#include "enumdefinitionprinter.h"
#include "serverdeclarationprinter.h"
#include "clientdeclarationprinter.h"
#include "clientdefinitionprinter.h"

#include "templates.h"
#include "utils.h"
#include "generatoroptions.h"

#include <iostream>
#include <set>
#include <google/protobuf/stubs/logging.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.h>

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::compiler;

SingleFileGenerator::SingleFileGenerator() : GeneratorBase(GeneratorBase::SingleMode)
{}

bool SingleFileGenerator::Generate(const FileDescriptor *file,
                                   const std::string &parameter,
                                   GeneratorContext *generatorContext,
                                   std::string *error) const
{
    if (file->syntax() != FileDescriptor::SYNTAX_PROTO3) {
        *error = "Invalid proto used. This plugin only supports 'proto3' syntax";
        return false;
    }

    return GenerateMessages(file, parameter, generatorContext, error)
            && GenerateServices(file, parameter, generatorContext, error);
}

bool SingleFileGenerator::GenerateMessages(const ::google::protobuf::FileDescriptor *file,
                                           const std::string &,
                                           ::google::protobuf::compiler::GeneratorContext *generatorContext,
                                           std::string *) const {
    if (file->message_type_count() <= 0 && file->enum_type_count() <= 0) {
        return true;
    }

    std::set<std::string> internalIncludes;
    std::set<std::string> externalIncludes;

    std::string basename = generateBaseName(file, utils::extractFileBasename(file->name()));
    std::unique_ptr<io::ZeroCopyOutputStream> headerStream(generatorContext->Open(basename + Templates::ProtoFileSuffix + ".h"));
    std::unique_ptr<io::ZeroCopyOutputStream> sourceStream(generatorContext->Open(basename + Templates::ProtoFileSuffix + ".cpp"));

    std::shared_ptr<::google::protobuf::io::Printer> headerPrinter(new ::google::protobuf::io::Printer(headerStream.get(), '$'));
    std::shared_ptr<::google::protobuf::io::Printer> sourcePrinter(new ::google::protobuf::io::Printer(sourceStream.get(), '$'));

    printDisclaimer(headerPrinter);
    printPreamble(headerPrinter);

    headerPrinter->Print(Templates::DefaultProtobufIncludesTemplate);
    if (GeneratorOptions::instance().hasQml()) {
        headerPrinter->Print(Templates::QmlProtobufIncludesTemplate);
    }

    printDisclaimer(sourcePrinter);
    sourcePrinter->Print({{"include", basename + Templates::ProtoFileSuffix}}, Templates::InternalIncludeTemplate);

    externalIncludes.insert("QByteArray");
    externalIncludes.insert("QString");

    bool hasQtTypes = false;
    common::iterateMessages(file, [&externalIncludes, &hasQtTypes](const ::google::protobuf::Descriptor *message) {
        for (int i = 0; i < message->field_count(); i++) {
            auto field = message->field(i);
            if (field->type() == ::google::protobuf::FieldDescriptor::TYPE_MESSAGE
                    && !field->is_map() && !field->is_repeated()
                    && common::isQtType(field)) {                
                externalIncludes.insert(field->message_type()->name());
                hasQtTypes = true;
            }
        }
        if (message->full_name() == "google.protobuf.Timestamp") {
            externalIncludes.insert("QDateTime");
        }
    });

    if (hasQtTypes) {
        externalIncludes.insert("QtProtobufQtTypes");
    }

    for (int i = 0; i < file->dependency_count(); i++) {
        if (file->dependency(i)->name() == "QtProtobuf/QtCore.proto"
                || file->dependency(i)->name() == "QtProtobuf/QtGui.proto") {
            continue;
        }
        internalIncludes.insert(utils::removeFileSuffix(file->dependency(i)->name()) + Templates::ProtoFileSuffix);
    }

    for (auto include : externalIncludes) {
        headerPrinter->Print({{"include", include}}, Templates::ExternalIncludeTemplate);
    }

    for (auto include : internalIncludes) {
        headerPrinter->Print({{"include", include}}, Templates::InternalIncludeTemplate);
    }

    if (GeneratorOptions::instance().hasQml()) {
        sourcePrinter->Print({{"include", "QQmlEngine"}}, Templates::ExternalIncludeTemplate);
    }

    printQtProtobufUsingNamespace(sourcePrinter);

    PackagesList packageList;
    packageList[file->package()].push_back(file);

    for (int i = 0; i < file->enum_type_count(); i++) {
        EnumDeclarationPrinter enumDecl(file->enum_type(i), headerPrinter);
        enumDecl.run();
        EnumDefinitionPrinter enumSourceDef(file->enum_type(i), sourcePrinter);
        enumSourceDef.run();
    }

    common::iterateMessages(file, [&headerPrinter](const ::google::protobuf::Descriptor *message) {
        MessageDeclarationPrinter messageDecl(message, headerPrinter);
        messageDecl.printClassForwardDeclaration();
    });

    common::iterateMessages(file, [&headerPrinter, &sourcePrinter](const ::google::protobuf::Descriptor *message) {
        MessageDeclarationPrinter messageDecl(message, headerPrinter);
        messageDecl.printClassDeclaration();


        MessageDefinitionPrinter messageDef(message, sourcePrinter);
        messageDef.printClassDefinition();
    });

    return true;
}


bool SingleFileGenerator::GenerateServices(const ::google::protobuf::FileDescriptor *file,
                                           const std::string &,
                                           ::google::protobuf::compiler::GeneratorContext *generatorContext,
                                           std::string *) const
{
    if (file->service_count() <= 0) {
        return true;
    }

    std::set<std::string> internalIncludes;
    std::set<std::string> externalIncludes;

    std::string basename = generateBaseName(file, utils::extractFileBasename(file->name()));
    std::unique_ptr<io::ZeroCopyOutputStream> headerStream(generatorContext->Open(basename + Templates::GrpcFileSuffix + Templates::ProtoFileSuffix + ".h"));
    std::unique_ptr<io::ZeroCopyOutputStream> sourceStream(generatorContext->Open(basename + Templates::GrpcFileSuffix + Templates::ProtoFileSuffix + ".cpp"));
    std::shared_ptr<::google::protobuf::io::Printer> headerPrinter(new ::google::protobuf::io::Printer(headerStream.get(), '$'));
    std::shared_ptr<::google::protobuf::io::Printer> sourcePrinter(new ::google::protobuf::io::Printer(sourceStream.get(), '$'));

    printDisclaimer(headerPrinter);
    printPreamble(headerPrinter);
    headerPrinter->Print(Templates::DefaultProtobufIncludesTemplate);
    if (GeneratorOptions::instance().hasQml()) {
        headerPrinter->Print(Templates::QmlProtobufIncludesTemplate);
    }

    printDisclaimer(sourcePrinter);
    sourcePrinter->Print({{"include", basename + Templates::GrpcFileSuffix + Templates::ProtoFileSuffix}}, Templates::InternalIncludeTemplate);

    for (int i = 0; i < file->service_count(); i++) {
        const ServiceDescriptor *service = file->service(i);
        for (int i = 0; i < service->method_count(); i++) {
            const MethodDescriptor *method = service->method(i);
            if (method->input_type()->file() != service->file()) {
                internalIncludes.insert(utils::removeFileSuffix(method->input_type()->file()->name()) + Templates::ProtoFileSuffix);
            }

            if (method->output_type()->file() != service->file()) {
                internalIncludes.insert(utils::removeFileSuffix(method->output_type()->file()->name()) + Templates::ProtoFileSuffix);
            }
        }
    }

    externalIncludes.insert("QAbstractGrpcClient");
    externalIncludes.insert("QGrpcAsyncReply");
    externalIncludes.insert("QGrpcSubscription");

    if (file->message_type_count() > 0) {
        internalIncludes.insert(basename + Templates::ProtoFileSuffix);
    }
    for (auto include : externalIncludes) {
        headerPrinter->Print({{"include", include}}, Templates::ExternalIncludeTemplate);
    }

    for (auto include : internalIncludes) {
        headerPrinter->Print({{"include", include}}, Templates::InternalIncludeTemplate);
    }

    if (GeneratorOptions::instance().hasQml()) {
        sourcePrinter->Print({{"include", "QQmlEngine"}}, Templates::ExternalIncludeTemplate);
        sourcePrinter->Print({{"include", "QJSEngine"}}, Templates::ExternalIncludeTemplate);
        sourcePrinter->Print({{"include", "QJSValue"}}, Templates::ExternalIncludeTemplate);
    }


    printQtProtobufUsingNamespace(sourcePrinter);

    for (int i = 0; i < file->service_count(); i++) {
        const ServiceDescriptor *service = file->service(i);
        // Serverside descriptor is not in use
        //        std::string baseFilename(service->name());
        //        utils::tolower(baseFilename);

        //        std::string fullFilename = baseFilename + "server.h";
        //        ServerDeclarationprinter serverGen(service,
        //                                  std::shared_ptr<io::ZeroCopyOutputStream>(generatorContext->Open(fullFilename)));
        //        serverGen.run();

        ClientDeclarationPrinter clientDecl(service, headerPrinter);
        clientDecl.run();

        ClientDefinitionPrinter clientDef(service, sourcePrinter);
        clientDef.run();
    }

    return true;
}



