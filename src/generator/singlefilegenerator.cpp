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
#include "templates.h"
#include "classgeneratorbase.h"
#include "protobufclassgenerator.h"
#include "protobufsourcegenerator.h"
#include "globalenumsgenerator.h"
#include "globalenumssourcegenerator.h"
#include "servergenerator.h"
#include "clientgenerator.h"
#include "clientsourcegenerator.h"
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
    if (file->message_type_count() <= 0) {
        return true;
    }

    std::string outFileBasename = utils::extractFileName(file->name());
    std::set<std::string> internalIncludes;
    std::set<std::string> externalIncludes;
    std::shared_ptr<io::ZeroCopyOutputStream> outHeader(generatorContext->Open(outFileBasename + Templates::ProtoFileSuffix + ".h"));
    std::shared_ptr<io::ZeroCopyOutputStream> outSource(generatorContext->Open(outFileBasename + Templates::ProtoFileSuffix + ".cpp"));
    std::shared_ptr<::google::protobuf::io::Printer> outHeaderPrinter(new ::google::protobuf::io::Printer(outHeader.get(), '$'));
    std::shared_ptr<::google::protobuf::io::Printer> outSourcePrinter(new ::google::protobuf::io::Printer(outSource.get(), '$'));


    outHeaderPrinter->Print(Templates::DisclaimerTemplate);
    outHeaderPrinter->Print(Templates::PreambleTemplate);
    outHeaderPrinter->Print(Templates::DefaultProtobufIncludesTemplate);
    if (GeneratorOptions::instance().hasQml()) {
        outHeaderPrinter->Print(Templates::QmlProtobufIncludesTemplate);
    }

    outSourcePrinter->Print(Templates::DisclaimerTemplate);
    outSourcePrinter->Print({{"include", outFileBasename + Templates::ProtoFileSuffix}}, Templates::InternalIncludeTemplate);

    externalIncludes.insert("QByteArray");
    externalIncludes.insert("QString");

    for (int i = 0; i < file->dependency_count(); i++) {
        internalIncludes.insert(utils::extractFileName(file->dependency(i)->name()) + Templates::ProtoFileSuffix);
    }

    for(auto include : externalIncludes) {
        outHeaderPrinter->Print({{"include", include}}, Templates::ExternalIncludeTemplate);
    }

    for(auto include : internalIncludes) {
        outHeaderPrinter->Print({{"include", include}}, Templates::InternalIncludeTemplate);
    }

    outSourcePrinter->Print({{"namespace", "QtProtobuf"}}, Templates::UsingNamespaceTemplate);


    PackagesList packageList;
    packageList[file->package()].push_back(file);

    GlobalEnumsGenerator enumGen(packageList,
                                 outHeaderPrinter);
    enumGen.run();

    GlobalEnumsSourceGenerator enumSourceGen(packageList,
                                             outSourcePrinter);
    enumSourceGen.run();


    std::vector<std::string> namespaces;
    std::string namespacesColonDelimited;

    utils::split(file->package(), namespaces, '.');
    assert(namespaces.size() > 0);
    for (size_t i = 0; i < namespaces.size(); i++) {
        if (i > 0) {
            namespacesColonDelimited = namespacesColonDelimited.append("::");
        }
        namespacesColonDelimited = namespacesColonDelimited.append(namespaces[i]);
        outHeaderPrinter->Print({{"namespace", namespaces[i]}}, Templates::NamespaceTemplate);
    }

    iterateNonNestedFileds(file, [&outHeaderPrinter](const ::google::protobuf::Descriptor *message){
        std::string qualifiedClassName = utils::upperCaseName(message->name());
        outHeaderPrinter->Print({{"classname", qualifiedClassName}}, Templates::ProtoClassDeclarationTemplate);
        outHeaderPrinter->Print({{"classname", qualifiedClassName}}, Templates::ComplexListTypeUsingTemplate);
    });

    for (size_t i = 0; i < namespaces.size(); i++) {
        outHeaderPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
    }

    iterateNonNestedFileds(file, [&outHeaderPrinter, &outSourcePrinter](const ::google::protobuf::Descriptor *message){
        ProtobufClassGenerator classGen(message,
                                        outHeaderPrinter);

        classGen.printNamespaces();
        classGen.printClassDeclaration();
        classGen.printProperties();
        classGen.printPrivate();
        classGen.printClassMembers();
        classGen.printPublic();
        classGen.printDestructor();
        classGen.encloseClass();
        classGen.encloseNamespaces();
        classGen.printMetaTypeDeclaration();
        classGen.printMapsMetaTypesDeclaration();
        classGen.printLocalEnumsMetaTypesDeclaration();

        ProtobufSourceGenerator srcGen(message,
                                       outSourcePrinter);
        srcGen.printNamespaces();
        srcGen.printFieldsOrdering();
        srcGen.printRegisterBody();
        srcGen.printDestructor();
        srcGen.printConstructor();
        srcGen.printCopyFunctionality();
        srcGen.printMoveSemantic();
        srcGen.printComparisonOperators();
        srcGen.printGetters();
        srcGen.encloseNamespaces();

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

    std::string outFileBasename = utils::extractFileName(file->name());
    std::set<std::string> internalIncludes;
    std::set<std::string> externalIncludes;
    std::shared_ptr<io::ZeroCopyOutputStream> outHeader(generatorContext->Open(outFileBasename + Templates::GrpcFileSuffix + Templates::ProtoFileSuffix + ".h"));
    std::shared_ptr<io::ZeroCopyOutputStream> outSource(generatorContext->Open(outFileBasename + Templates::GrpcFileSuffix + Templates::ProtoFileSuffix + ".cpp"));
    std::shared_ptr<::google::protobuf::io::Printer> outHeaderPrinter(new ::google::protobuf::io::Printer(outHeader.get(), '$'));
    std::shared_ptr<::google::protobuf::io::Printer> outSourcePrinter(new ::google::protobuf::io::Printer(outSource.get(), '$'));

    outHeaderPrinter->Print(Templates::DisclaimerTemplate);
    outHeaderPrinter->Print(Templates::PreambleTemplate);
    outHeaderPrinter->Print(Templates::DefaultProtobufIncludesTemplate);
    if (GeneratorOptions::instance().hasQml()) {
        outHeaderPrinter->Print(Templates::QmlProtobufIncludesTemplate);
    }

    outSourcePrinter->Print(Templates::DisclaimerTemplate);
    outSourcePrinter->Print({{"include", outFileBasename + Templates::GrpcFileSuffix + Templates::ProtoFileSuffix}}, Templates::InternalIncludeTemplate);

    for (int i = 0; i < file->service_count(); i++) {
        const ServiceDescriptor *service = file->service(i);
        for (int i = 0; i < service->method_count(); i++) {
            const MethodDescriptor *method = service->method(i);
            if (method->input_type()->file() != service->file()) {
                internalIncludes.insert(utils::extractFileName(method->input_type()->file()->name()) + Templates::ProtoFileSuffix);
            }

            if (method->output_type()->file() != service->file()) {
                internalIncludes.insert(utils::extractFileName(method->output_type()->file()->name()) + Templates::ProtoFileSuffix);
            }
        }
    }


    externalIncludes.insert("QAbstractGrpcClient");
    externalIncludes.insert("QGrpcAsyncReply");

    if (file->message_type_count() > 0) {
        internalIncludes.insert(outFileBasename + Templates::ProtoFileSuffix);
    }
    for(auto include : externalIncludes) {
        outHeaderPrinter->Print({{"include", include}}, Templates::ExternalIncludeTemplate);
    }

    for(auto include : internalIncludes) {
        outHeaderPrinter->Print({{"include", include}}, Templates::InternalIncludeTemplate);
    }


    outSourcePrinter->Print({{"namespace", "QtProtobuf"}}, Templates::UsingNamespaceTemplate);

    for (int i = 0; i < file->service_count(); i++) {
        const ServiceDescriptor *service = file->service(i);
        // Serverside descriptor is not in use
        //        std::string baseFilename(service->name());
        //        utils::tolower(baseFilename);

        //        std::string fullFilename = baseFilename + "server.h";
        //        ServerGenerator serverGen(service,
        //                                  std::shared_ptr<io::ZeroCopyOutputStream>(generatorContext->Open(fullFilename)));
        //        serverGen.run();

        ClientGenerator clientGen(service,
                                  outHeaderPrinter);
        clientGen.printNamespaces();
        clientGen.printClientClass();
        clientGen.printPublic();
        clientGen.printConstructor();
        clientGen.printClientMethodsDeclaration();
        clientGen.encloseClass();
        clientGen.encloseNamespaces();

        ClientSourceGenerator clientSrcGen(service,
                                           outSourcePrinter);
        clientSrcGen.printNamespaces();
        clientSrcGen.printConstructor();
        clientSrcGen.printMethods();
        clientSrcGen.encloseNamespaces();
    }

    return true;
}



