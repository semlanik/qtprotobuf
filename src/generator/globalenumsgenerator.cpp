/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
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

#include "globalenumsgenerator.h"

#include <google/protobuf/io/zero_copy_stream.h>

using namespace ::qtprotobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

GlobalEnumsGenerator::GlobalEnumsGenerator(const PackagesList &packageList, std::unique_ptr<io::ZeroCopyOutputStream> out) :
    ClassGeneratorBase(Templates::GlobalEnumClassNameTemplate, std::move(out))
  , mPackageList(packageList) {}

void GlobalEnumsGenerator::startEnum(const std::vector<std::string>& namespaces) {
    printNamespaces(namespaces);
    printEnumClass();
    printPublic();
    Indent();
    mPrinter.Print(Templates::ComplexTypeRegistrationMethodTemplate);
    Outdent();
    printPrivate();
    printConstructor();
}

void GlobalEnumsGenerator::printConstructor()
{
    Indent();
    mPrinter.Print({{"classname", mClassName}}, Templates::ConstructorHeaderTemplate);
    mPrinter.Print({{"classname", mClassName}}, Templates::CopyConstructorTemplate);
    encloseNamespaces(1);
    mPrinter.Print({{"classname", mClassName}}, Templates::MoveConstructorTemplate);
    encloseNamespaces(1);
    Outdent();
}

void GlobalEnumsGenerator::run() {
    printPreamble();
    std::vector<std::string> namespaces;
    for (auto package : mPackageList) {
        utils::split(package.first, namespaces, '.');
        startEnum(namespaces);
        for (auto file : package.second) {
            run(file);
        }
        encloseEnum(namespaces);
        for (auto file : package.second) {
            printMetatype(file, namespaces);
        }
    }
}

void GlobalEnumsGenerator::run(const FileDescriptor *file) {
    printQEnums(file);
}

void GlobalEnumsGenerator::printMetatype(const google::protobuf::FileDescriptor *file,
                                         const std::vector<std::string>& namespaces) {
    std::string fullNamespace;
    for (auto name : namespaces) {
        if (fullNamespace.empty()) {
            fullNamespace.append(name);
            continue;
        }
        fullNamespace.append("::").append(name);
    }
    std::string fullClassname;
    for (int i = 0; i < file->enum_type_count(); i++) {
        const auto enumDescr = file->enum_type(i);
        if (!enumDescr->name().empty()) {
            fullClassname.append(Templates::GlobalEnumClassNameTemplate).append("::").append(enumDescr->name());
        }
    }
    if (!fullClassname.empty()) {
        mPrinter.Print({{"classname", fullClassname}, {"namespaces", fullNamespace}},
                       Templates::DeclareMetaTypeListTemplate);
    }
}

void GlobalEnumsGenerator::encloseEnum(const std::vector<std::string>& namespaces) {
    encloseClass();
    encloseNamespaces(namespaces.size());
}

void GlobalEnumsGenerator::printEnumClass() {
    mPrinter.Print({{"classname", mClassName}}, Templates::NonProtoClassDefinitionTemplate);
}
