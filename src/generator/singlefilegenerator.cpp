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

#include <iostream>
#include <set>
#include <google/protobuf/stubs/logging.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.h>

static const char *protoFileSuffix = ".pb";
static const char *grpcFileSuffix = "_grpc";
static const std::string globalDeclarationsFilename = std::string("globaldeclarations") + protoFileSuffix;

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::compiler;

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

bool SingleFileGenerator::GenerateAll(const std::vector<const FileDescriptor *> &files, const string &parameter, GeneratorContext *generatorContext, string *error) const
{
    std::shared_ptr<io::ZeroCopyOutputStream> outHeader(generatorContext->Open(globalDeclarationsFilename + ".h"));
    std::shared_ptr<io::ZeroCopyOutputStream> outSource(generatorContext->Open(globalDeclarationsFilename + ".cpp"));
    std::shared_ptr<::google::protobuf::io::Printer> outHeaderPrinter(new ::google::protobuf::io::Printer(outHeader.get(), '$'));
    std::shared_ptr<::google::protobuf::io::Printer> outSourcePrinter(new ::google::protobuf::io::Printer(outSource.get(), '$'));

    PackagesList packageList;
    for (auto file : files) {
        packageList[file->package()].push_back(file);
    }

    GlobalEnumsGenerator enumGen(packageList,
                                 outHeaderPrinter);
    enumGen.run();

    GlobalEnumsSourceGenerator enumSourceGen(packageList,
                                             outSourcePrinter);
    enumSourceGen.run();


    std::shared_ptr<io::ZeroCopyOutputStream> outfHeader(generatorContext->Open(globalDeclarationsFilename + "_.h"));
    std::shared_ptr<::google::protobuf::io::Printer> outfHeaderPrinter(new ::google::protobuf::io::Printer(outfHeader.get(), '$'));

    outfHeaderPrinter->Print(Templates::DisclaimerTemplate);
    outfHeaderPrinter->Print(Templates::PreambleTemplate);
    outfHeaderPrinter->Print(Templates::DefaultProtobufIncludesTemplate);

    for (auto file : files) {
        if (file->message_type_count() > 0) {
            outfHeaderPrinter->Print({{"include", utils::extractFileName(file->name()) + protoFileSuffix}}, Templates::InternalIncludeTemplate);
        }
    }

    for (auto package : packageList) {
        std::vector<std::string> namespaces;
        utils::split(package.first, namespaces, '.');
        for (auto ns : namespaces) {
            outfHeaderPrinter->Print({{"namespace", ns}}, Templates::NamespaceTemplate);
        }
        outfHeaderPrinter->Print("inline void qRegisterProtobufTypes() {\n");
        outfHeaderPrinter->Indent();
        outfHeaderPrinter->Indent();
        for (auto file : package.second) {
            iterateNonNestedFileds(file, [&outfHeaderPrinter](const ::google::protobuf::Descriptor *message){
                outfHeaderPrinter->Print({{"classname", message->name()}}, "qRegisterProtobufType<$classname$>();\n");
            });
        }
        outfHeaderPrinter->Outdent();
        outfHeaderPrinter->Outdent();
        outfHeaderPrinter->Print("}\n");
        for (size_t i = 0; i < namespaces.size(); i++) {
            outfHeaderPrinter->Print("}\n");
        }
    }

    return CodeGenerator::GenerateAll(files, parameter, generatorContext, error);
}

void SingleFileGenerator::iterateNonNestedFileds(const ::google::protobuf::FileDescriptor *file, std::function<void(const ::google::protobuf::Descriptor *)> callback) const
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
    std::shared_ptr<io::ZeroCopyOutputStream> outHeader(generatorContext->Open(outFileBasename + protoFileSuffix + ".h"));
    std::shared_ptr<io::ZeroCopyOutputStream> outSource(generatorContext->Open(outFileBasename + protoFileSuffix + ".cpp"));
    std::shared_ptr<::google::protobuf::io::Printer> outHeaderPrinter(new ::google::protobuf::io::Printer(outHeader.get(), '$'));
    std::shared_ptr<::google::protobuf::io::Printer> outSourcePrinter(new ::google::protobuf::io::Printer(outSource.get(), '$'));


    outHeaderPrinter->Print(Templates::DisclaimerTemplate);
    outHeaderPrinter->Print(Templates::PreambleTemplate);
    outHeaderPrinter->Print(Templates::DefaultProtobufIncludesTemplate);

    outSourcePrinter->Print(Templates::DisclaimerTemplate);
    outSourcePrinter->Print({{"include", outFileBasename + protoFileSuffix}}, Templates::InternalIncludeTemplate);

    for (int i = 0; i < file->message_type_count(); i++) {
        const Descriptor *message = file->message_type(i);

        for(int j = 0; j < message->field_count(); j++) {
            const FieldDescriptor *field = message->field(j);
            switch(field->type()) {
            case FieldDescriptor::TYPE_MESSAGE:
                if ( field->is_map() ) {
                    externalIncludes.insert("QMap");
                    assert(field->message_type() != nullptr);
                    assert(field->message_type()->field_count() == 2);
                } else if (field->message_type()->file() != file) {
                    internalIncludes.insert(utils::extractFileName(field->message_type()->file()->name()) + protoFileSuffix);
                }
                break;
            case FieldDescriptor::TYPE_BYTES:
                externalIncludes.insert("QByteArray");
                break;
            case FieldDescriptor::TYPE_STRING:
                externalIncludes.insert("QString");
                break;
            case FieldDescriptor::TYPE_ENUM:
                if (field->enum_type()->file() != file) {
                    internalIncludes.insert(utils::extractFileName(field->enum_type()->file()->name()) + protoFileSuffix);
                }
                break;
            default:
                break;
            }
        }
    }

    internalIncludes.insert(globalDeclarationsFilename);

    for(auto include : externalIncludes) {
        outHeaderPrinter->Print({{"include", include}}, Templates::ExternalIncludeTemplate);
    }

    for(auto include : internalIncludes) {
        outHeaderPrinter->Print({{"include", include}}, Templates::InternalIncludeTemplate);
    }

    outSourcePrinter->Print({{"namespace", "QtProtobuf"}}, Templates::UsingNamespaceTemplate);

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
        outHeaderPrinter->Print({{"classname", message->name()}}, Templates::ProtoClassDeclarationTemplate);
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
        classGen.encloseClass();
        classGen.printListType();
        classGen.encloseNamespaces();
        classGen.printMetaTypeDeclaration();
        classGen.printMapsMetaTypesDeclaration();
        classGen.printLocalEmumsMetaTypesDeclaration();

        ProtobufSourceGenerator srcGen(message,
                                       outSourcePrinter);
        srcGen.printNamespaces();
        srcGen.printFieldsOrdering();
        srcGen.printRegisterBody();
        srcGen.printConstructor();
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
    std::shared_ptr<io::ZeroCopyOutputStream> outHeader(generatorContext->Open(outFileBasename + grpcFileSuffix + protoFileSuffix + ".h"));
    std::shared_ptr<io::ZeroCopyOutputStream> outSource(generatorContext->Open(outFileBasename + grpcFileSuffix + protoFileSuffix + ".cpp"));
    std::shared_ptr<::google::protobuf::io::Printer> outHeaderPrinter(new ::google::protobuf::io::Printer(outHeader.get(), '$'));
    std::shared_ptr<::google::protobuf::io::Printer> outSourcePrinter(new ::google::protobuf::io::Printer(outSource.get(), '$'));

    outHeaderPrinter->Print(Templates::DisclaimerTemplate);
    outHeaderPrinter->Print(Templates::PreambleTemplate);
    outHeaderPrinter->Print(Templates::DefaultProtobufIncludesTemplate);

    outSourcePrinter->Print(Templates::DisclaimerTemplate);
    outSourcePrinter->Print({{"include", outFileBasename + grpcFileSuffix + protoFileSuffix}}, Templates::InternalIncludeTemplate);

    for (int i = 0; i < file->service_count(); i++) {
        const ServiceDescriptor *service = file->service(i);
        for (int i = 0; i < service->method_count(); i++) {
            const MethodDescriptor *method = service->method(i);
            if (method->input_type()->file() != service->file()) {
                internalIncludes.insert(utils::extractFileName(method->input_type()->file()->name()) + protoFileSuffix);
            }

            if (method->output_type()->file() != service->file()) {
                internalIncludes.insert(utils::extractFileName(method->output_type()->file()->name()) + protoFileSuffix);
            }
        }
    }


    externalIncludes.insert("QAbstractGrpcClient");
    externalIncludes.insert("QGrpcAsyncReply");

    internalIncludes.insert(globalDeclarationsFilename);
    if (file->message_type_count() > 0) {
        internalIncludes.insert(outFileBasename + protoFileSuffix);
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



