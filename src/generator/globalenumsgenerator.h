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

#pragma once

#include "classgeneratorbase.h"
#include "utils.h"

namespace QtProtobuf {
namespace generator {

class GlobalEnumsGenerator : public ClassGeneratorBase
{
    PackagesList mPackageList;
public:
    GlobalEnumsGenerator(const PackagesList &packageList, std::unique_ptr<::google::protobuf::io::ZeroCopyOutputStream> out);
    virtual ~GlobalEnumsGenerator() = default;

    void run();

    void startEnum(const std::vector<std::string>& namespaces);
    void run(const ::google::protobuf::FileDescriptor *file);
    void encloseEnum(const std::vector<std::string>& namespaces);
    void printMetatype(const google::protobuf::FileDescriptor *file,
                       const std::vector<std::string>& namespaces);
    void printEnumClass();
    void printConstructor();
};

} //namespace generator
} //namespace QtProtobuf

