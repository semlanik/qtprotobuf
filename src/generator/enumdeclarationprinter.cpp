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

#include "enumdeclarationprinter.h"
#include "generatorcommon.h"

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;

EnumDeclarationPrinter::EnumDeclarationPrinter(const EnumDescriptor *descriptor, const std::shared_ptr<::google::protobuf::io::Printer> &printer) :
    DescriptorPrinterBase<google::protobuf::EnumDescriptor>(descriptor, printer)
{
    mTypeMap = common::produceEnumTypeMap(descriptor, nullptr);
    mName += Templates::EnumClassSuffix;
}

void EnumDeclarationPrinter::startEnum()
{
    printNamespaces();
    printEnumClass();
    printPublicBlock();
    Indent();
    mPrinter->Print({{"classname", mName}}, Templates::ManualRegistrationDeclaration);
    Outdent();
    printPrivateBlock();
    printConstructor();
}

void EnumDeclarationPrinter::printEnum()
{
    auto typeMap = common::produceEnumTypeMap(mDescriptor, nullptr);
    printPublicBlock();

    Indent();
    mPrinter->Print(typeMap, Templates::EnumDefinitionTemplate);

    Indent();
    for (int j = 0; j < mDescriptor->value_count(); j++) {
        const auto valueDescr = mDescriptor->value(j);
        mPrinter->Print({{"enumvalue", utils::upperCaseName(valueDescr->name())},
                         {"value", std::to_string(valueDescr->number())}}, Templates::EnumFieldTemplate);
    }
    Outdent();
    mPrinter->Print(Templates::SemicolonBlockEnclosureTemplate);
    mPrinter->Print(typeMap, Templates::QEnumTemplate);
    mPrinter->Print(typeMap, Templates::EnumTypeRepeatedTemplate);
    Outdent();
}

void EnumDeclarationPrinter::printConstructor()
{
    Indent();
    mPrinter->Print({{"classname", mName}}, Templates::ConstructorHeaderTemplate);
    mPrinter->Print({{"classname", mName}}, Templates::DeletedCopyConstructorTemplate);
    mPrinter->Print({{"classname", mName}}, Templates::DeletedMoveConstructorTemplate);
    Outdent();
}

void EnumDeclarationPrinter::printMetatype()
{
    mPrinter->Print(mTypeMap,
                   Templates::DeclareMetaTypeListTemplate);
}

void EnumDeclarationPrinter::encloseEnum()
{
    encloseClass();
    encloseNamespaces();
}

void EnumDeclarationPrinter::printEnumClass()
{
    mPrinter->Print({{"classname", mName}}, Templates::ClassDeclarationTemplate);
}
