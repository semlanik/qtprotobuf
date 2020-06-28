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

#include "clientdeclarationprinter.h"

#include <google/protobuf/stubs/logging.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.h>

#include <unordered_set>

#include "utils.h"
#include "templates.h"

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::compiler;
ClientDeclarationPrinter::ClientDeclarationPrinter(const ::google::protobuf::ServiceDescriptor *service, const std::shared_ptr<::google::protobuf::io::Printer> &printer) :
    ServiceDeclarationPrinterBase(service, printer)
{
    mName += "Client";
}

void ClientDeclarationPrinter::printClientClass()
{
    mPrinter->Print({{"classname", mName}, {"parent_class", "QtProtobuf::QAbstractGrpcClient"}}, Templates::ClassDefinitionTemplate);
    Indent();
    mPrinter->Print(Templates::QObjectMacro);
    mPrinter->Print("\n");
    Outdent();
}

void ClientDeclarationPrinter::printConstructor()
{
    Indent();
    mPrinter->Print({{"classname", mName}}, Templates::QObjectConstructorTemplate);
    Outdent();
}

void ClientDeclarationPrinter::printClientIncludes()
{
    std::unordered_set<std::string> includeSet;
    includeSet.insert("QAbstractGrpcClient");
    includeSet.insert("QGrpcAsyncReply");
    includeSet.insert("QGrpcSubscription");
    for (auto type : includeSet) {
        mPrinter->Print({{"include", type}}, Templates::ExternalIncludeTemplate);
    }
}

void ClientDeclarationPrinter::printClientMethodsDeclaration()
{
    Indent();
    for (int i = 0; i < mDescriptor->method_count(); i++) {
        const MethodDescriptor *method = mDescriptor->method(i);
        std::map<std::string, std::string> parameters = common::produceMethodMap(method, mName);

        if (method->server_streaming()) {
            mPrinter->Print(parameters, Templates::ClientMethodServerStreamDeclarationTemplate);
            mPrinter->Print(parameters, Templates::ClientMethodServerStream2DeclarationTemplate);
        } else {
            mPrinter->Print(parameters, Templates::ClientMethodDeclarationSyncTemplate);
            mPrinter->Print(parameters, Templates::ClientMethodDeclarationAsyncTemplate);
            mPrinter->Print(parameters, Templates::ClientMethodDeclarationAsync2Template);
            if (GeneratorOptions::instance().hasQml()) {
                mPrinter->Print(parameters, Templates::ClientMethodDeclarationQmlTemplate);
            }
        }
        mPrinter->Print("\n");
    }
    Outdent();
}
