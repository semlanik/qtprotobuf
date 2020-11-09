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

#include "baseprinter.h"

#include "utils.h"
#include "templates.h"
#include "generatoroptions.h"
#include "generatorcommon.h"

namespace QtProtobuf {
namespace generator {

using PropertyMap = std::map<std::string, std::string>;

/*!
 * \ingroup generator
 * \private
 * \brief The DescriptorPrinterBase class is base of source code generation
 */

template<typename T>
class DescriptorPrinterBase : public BasePrinter
{
public:
    DescriptorPrinterBase(const T* descriptor, const std::shared_ptr<::google::protobuf::io::Printer> &printer) : BasePrinter(printer)
      , mDescriptor(descriptor)
      , mName(utils::upperCaseName(descriptor->name()))
    {}
    virtual ~DescriptorPrinterBase() = default;
public:
    void encloseClass() {
        mPrinter->Print(Templates::SemicolonBlockEnclosureTemplate);
        mPrinter->Print("\n");
    }

    void printNamespaces() {
        auto namespaces = common::getNamespaces(mDescriptor);
        mPrinter->Print("\n");
        for (auto ns : namespaces) {
            mPrinter->Print({{"namespace", ns}}, Templates::NamespaceTemplate);
        }
    }

    void encloseNamespaces() {
        auto namespaces = common::getNamespaces(mDescriptor);
        mPrinter->Print("\n");
        for (size_t i = 0; i < namespaces.size(); ++i) {
            mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
        }
        mPrinter->Print("\n");
    }

protected:
    const T* mDescriptor;
    std::string mName;
    TypeMap mTypeMap;
};

} //namespace generator
} //namespace QtProtobuf
