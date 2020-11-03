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

#include "clientdefinitionprinter.h"

#include <google/protobuf/io/zero_copy_stream.h>

#include "utils.h"
#include "templates.h"
#include "generatorcommon.h"

using namespace QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::compiler;

ClientDefinitionPrinter::ClientDefinitionPrinter(const google::protobuf::ServiceDescriptor *service,
                      const std::shared_ptr<::google::protobuf::io::Printer> &printer) :
    DescriptorPrinterBase<google::protobuf::ServiceDescriptor>(service, printer)
{
    mName += "Client";
}

void ClientDefinitionPrinter::printMethods()
{
    for (int i = 0; i < mDescriptor->method_count(); i++) {
        const MethodDescriptor *method = mDescriptor->method(i);
        MethodMap parameters = common::produceMethodMap(method, mName);
        if (method->server_streaming()) {
            mPrinter->Print(parameters, Templates::ClientMethodServerStreamDefinitionTemplate);
            mPrinter->Print(parameters, Templates::ClientMethodServerStream2DefinitionTemplate);
            if (GeneratorOptions::instance().hasQml()) {
                mPrinter->Print(parameters, Templates::ClientMethodServerStreamQmlDefinitionTemplate);
            }
        } else {
            mPrinter->Print(parameters, Templates::ClientMethodDefinitionSyncTemplate);
            mPrinter->Print(parameters, Templates::ClientMethodDefinitionAsyncTemplate);
            mPrinter->Print(parameters, Templates::ClientMethodDefinitionAsync2Template);
            if (GeneratorOptions::instance().hasQml()) {
                mPrinter->Print(parameters, Templates::ClientMethodDefinitionQmlTemplate);
                mPrinter->Print(parameters, Templates::ClientMethodDefinitionQml2Template);
            }
        }
    }
}

void ClientDefinitionPrinter::printConstructor()
{
    mPrinter->Print({ {"classname", mName},
                     {"parent_class", "QAbstractGrpcClient"},
                     {"service_name", mDescriptor->full_name()} }, Templates::ClientConstructorDefinitionTemplate);
}
