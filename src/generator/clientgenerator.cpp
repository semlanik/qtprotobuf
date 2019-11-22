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

#include "clientgenerator.h"

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

ClientGenerator::ClientGenerator(const ServiceDescriptor *service, const std::shared_ptr<io::ZeroCopyOutputStream> &out) :
    ServiceGeneratorBase(service, out)
{
    mClassName += "Client";
}

void ClientGenerator::printClientClass()
{
    mPrinter.Print({{"classname", mClassName}, {"parent_class", "QtProtobuf::QAbstractGrpcClient"}}, Templates::ClassDefinitionTemplate);
    mPrinter.Print(Templates::QObjectMacro);
}

void ClientGenerator::printConstructor()
{
    Indent();
    mPrinter.Print({{"classname", mClassName}}, Templates::QObjectConstructorTemplate);
    Outdent();
}

void ClientGenerator::printClientIncludes()
{
    printIncludes();

    std::unordered_set<std::string> includeSet;
    includeSet.insert("qabstractgrpcclient");
    includeSet.insert("qgrpcasyncreply");
    for (auto type : includeSet) {
        mPrinter.Print({{"include", type}}, Templates::InternalIncludeTemplate);
    }
}

void ClientGenerator::printClientMethodsDeclaration()
{
    Indent();
    for (int i = 0; i < mService->method_count(); i++) {
        const MethodDescriptor *method = mService->method(i);
        std::map<std::string, std::string> parameters;
        getMethodParameters(method, parameters);

        if (method->server_streaming()) {
            mPrinter.Print(parameters, Templates::ClientMethodSignalDeclarationTemplate);
            mPrinter.Print(parameters, Templates::ClientMethodServerStreamDeclarationTemplate);
            mPrinter.Print(parameters, Templates::ClientMethodServerStream2DeclarationTemplate);
        } else {
            mPrinter.Print(parameters, Templates::ClientMethodDeclarationSyncTemplate);
            mPrinter.Print(parameters, Templates::ClientMethodDeclarationAsyncTemplate);
            mPrinter.Print(parameters, Templates::ClientMethodDeclarationAsync2Template);
        }
        mPrinter.Print("\n");
    }
    Outdent();
}
