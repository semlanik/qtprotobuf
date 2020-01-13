/*
 * MIT License
 *
 * Copyright (c) Tatyana Borisova <tanusshhka@mail.ru>
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

#include "enumssourcegenerator.h"

#include "generatoroptions.h"

#include <google/protobuf/io/zero_copy_stream.h>

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

EnumsSourceGenerator::EnumsSourceGenerator(const google::protobuf::EnumDescriptor *enumDesctiptor, const std::shared_ptr<google::protobuf::io::ZeroCopyOutputStream> &out) :
    ClassGeneratorBase(enumDesctiptor->full_name() + Templates::EnumClassSuffix, out)
  , mEnumDescriptor(enumDesctiptor) {}


EnumsSourceGenerator::EnumsSourceGenerator(const google::protobuf::EnumDescriptor *enumDesctiptor, const std::shared_ptr<::google::protobuf::io::Printer> &printer) :
    ClassGeneratorBase(enumDesctiptor->full_name() + Templates::EnumClassSuffix, printer)
  , mEnumDescriptor(enumDesctiptor) {}

void EnumsSourceGenerator::run() {
    printNamespaces(mNamespaces);
    printRegisterBody();
    encloseNamespaces();
}

void EnumsSourceGenerator::printRegisterBody()
{
    std::string packageName;
    for (auto name : mNamespaces) {
        if (packageName.empty()) {
            packageName.append(name);
            continue;
        }
        packageName.append(".").append(name);
    }

    const std::map<std::string, std::string> registrationProperties = {{"classname", mClassName},
                                                                       {"type", mClassName},
                                                                       {"namespaces", mNamespacesColonDelimited},
                                                                       {"package", packageName}};

    mPrinter->Print(registrationProperties, Templates::ManualRegistrationGlobalEnumDefinition);
    Indent();
    if (GeneratorOptions::instance().hasQml()) {
        mPrinter->Print(registrationProperties, Templates::QmlRegisterTypeUncreatableTemplate);
    }

    const std::map<std::string, std::string> properties = {{"classname", mClassName},
                                                           {"type", mClassName + "::" + mEnumDescriptor->name()},
                                                           {"enum", mEnumDescriptor->name() + Templates::ListSuffix},
                                                           {"namespaces", mNamespacesColonDelimited}};
    mPrinter->Print(properties, Templates::ComplexGlobalEnumFieldRegistrationTemplate);
    mPrinter->Print(properties, Templates::RegisterMetaTypeTemplate);
    mPrinter->Print(properties, Templates::RegisterEnumSerializersTemplate);
    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
}
