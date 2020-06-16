/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>, Tatyana Borisova <tanusshhka@mail.ru>
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

#include "enumdefinitionprinter.h"

#include "generatoroptions.h"

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

EnumDefinitionPrinter::EnumDefinitionPrinter(const google::protobuf::EnumDescriptor *descriptor, const std::shared_ptr<::google::protobuf::io::Printer> &printer) :
    DescriptorPrinterBase<google::protobuf::EnumDescriptor>(descriptor, printer)
{
    mTypeMap = common::produceEnumTypeMap(descriptor, nullptr);
    mName += Templates::EnumClassSuffix;
}

void EnumDefinitionPrinter::run() {
    printNamespaces();
    printRegisterBody();
    encloseNamespaces();
}

void EnumDefinitionPrinter::printRegisterBody()
{
    auto typeMap = mTypeMap;
    typeMap["classname"] = mName;
    mPrinter->Print(typeMap, Templates::EnumRegistrarTemplate);
    mPrinter->Print(typeMap, Templates::ManualRegistrationGlobalEnumDefinition);
    Indent();
    if (GeneratorOptions::instance().hasQml()) {
        mPrinter->Print(typeMap, Templates::QmlRegisterEnumTypeTemplate);
    }

    mPrinter->Print(typeMap, Templates::ComplexGlobalEnumFieldRegistrationTemplate);
    mPrinter->Print(typeMap, Templates::RegisterGlobalEnumMetaTypeTemplate);
    mPrinter->Print(typeMap, Templates::RegisterEnumSerializersTemplate);
    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
}
