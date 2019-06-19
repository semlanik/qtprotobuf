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

#include "clientsourcegenerator.h"

#include <google/protobuf/io/zero_copy_stream.h>

#include "utils.h"
#include "templates.h"

using namespace qtprotobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::compiler;

ClientSourceGenerator::ClientSourceGenerator(const google::protobuf::ServiceDescriptor *service,
                                             std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> out) :
    ClassSourceGeneratorBase(service->full_name(), std::move(out))
  , mService(service)
{
    mClassName += "Client";
}

void ClientSourceGenerator::printMethods()
{
    for (int i = 0; i < mService->method_count(); i++) {
        const MethodDescriptor *method = mService->method(i);
        std::map<std::string, std::string> parameters;
        getMethodParameters(method, parameters);
        if (method->server_streaming()) {
            mPrinter.Print(parameters, Templates::ClientMethodServerStreamDefinitionTemplate);
            mPrinter.Print(parameters, Templates::ClientMethodServerStream2DefinitionTemplate);
        } else {
            mPrinter.Print(parameters, Templates::ClientMethodDefinitionSyncTemplate);
            mPrinter.Print(parameters, Templates::ClientMethodDefinitionAsyncTemplate);
            mPrinter.Print(parameters, Templates::ClientMethodDefinitionAsync2Template);
        }
    }
}

void ClientSourceGenerator::printConstructor()
{
    mPrinter.Print({ {"classname", mClassName},
                     {"parent_class", "QAbstractGrpcClient"},
                     {"service_name", mService->full_name()} }, Templates::ConstructorDefinitionSyncTemplate);
}
