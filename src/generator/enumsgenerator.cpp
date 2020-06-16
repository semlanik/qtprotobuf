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

#include "enumsgenerator.h"

#include <google/protobuf/io/zero_copy_stream.h>

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

EnumsGenerator::EnumsGenerator(const EnumDescriptor *enumDesctiptor, const std::shared_ptr<io::ZeroCopyOutputStream> &out) :
    ClassGeneratorBase(enumDesctiptor->full_name() + Templates::EnumClassSuffix, out)
  , mEnumDescriptor(enumDesctiptor)
  , mTypeMap(common::produceEnumTypeMap(enumDesctiptor, nullptr))
{}


EnumsGenerator::EnumsGenerator(const EnumDescriptor *enumDesctiptor, const std::shared_ptr<::google::protobuf::io::Printer> &printer) :
    ClassGeneratorBase(enumDesctiptor->full_name() + Templates::EnumClassSuffix, printer)
  , mEnumDescriptor(enumDesctiptor)
  , mTypeMap(common::produceEnumTypeMap(enumDesctiptor, nullptr))
{}

void EnumsGenerator::startEnum()
{
    printNamespaces(mNamespaces);
    printEnumClass();
    printPublicBlock();
    Indent();
    mPrinter->Print({{"classname", mClassName}}, Templates::ManualRegistrationDeclaration);
    Outdent();
    printPrivateBlock();
    printConstructor();
}

void EnumsGenerator::printEnum()
{
    printPublicBlock();

    Indent();
    mPrinter->Print({{"enum", mEnumDescriptor->name()}}, Templates::EnumDefinitionTemplate);

    Indent();
    for (int j = 0; j < mEnumDescriptor->value_count(); j++) {
        const auto valueDescr = mEnumDescriptor->value(j);
        mPrinter->Print({{"enumvalue", utils::upperCaseName(valueDescr->name())},
                         {"value", std::to_string(valueDescr->number())}}, Templates::EnumFieldTemplate);
    }
    Outdent();
    mPrinter->Print(Templates::SemicolonBlockEnclosureTemplate);
    mPrinter->Print({{"type", mEnumDescriptor->name()}}, Templates::QEnumTemplate);
    mPrinter->Print({{"enum", mEnumDescriptor->name()}}, Templates::EnumTypeRepeatedTemplate);
    Outdent();
}

void EnumsGenerator::printConstructor()
{
    Indent();
    mPrinter->Print({{"classname", mClassName}}, Templates::ConstructorHeaderTemplate);
    mPrinter->Print({{"classname", mClassName}}, Templates::DeletedCopyConstructorTemplate);
    mPrinter->Print({{"classname", mClassName}}, Templates::DeletedMoveConstructorTemplate);
    Outdent();
}

void EnumsGenerator::run()
{
    startEnum();
    printEnum();
    encloseEnum();
    printMetatype();
}

void EnumsGenerator::printMetatype()
{
    mPrinter->Print(mTypeMap,
                   Templates::DeclareMetaTypeListTemplate);
}

void EnumsGenerator::encloseEnum()
{
    encloseClass();
    encloseNamespaces();
}

void EnumsGenerator::printEnumClass()
{
    mPrinter->Print({{"classname", mClassName}}, Templates::NonProtoClassDefinitionTemplate);
}
