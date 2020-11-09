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

#include <google/protobuf/io/printer.h>
#include <google/protobuf/descriptor.h>
#include <memory>

#include "utils.h"
#include "generatoroptions.h"
#include "generatorcommon.h"

namespace google { namespace protobuf {
class FieldDescriptor;
class Descriptor;
class MethodDescriptor;
}}

namespace QtProtobuf {
namespace generator {

using PropertyMap = std::map<std::string, std::string>;

/*!
 * \ingroup generator
 * \private
 * \brief The BasePrinter class is base of source code generation
 */
class BasePrinter
{
public:
    BasePrinter(const std::shared_ptr<::google::protobuf::io::Printer> &printer);
    virtual ~BasePrinter() = default;

    void printPublicBlock();
    void printPrivateBlock();
    void printSignalsBlock();

    template<typename T>
    void printComments(T *descriptor) {
        if (!GeneratorOptions::instance().generateComments()) {
            return;
        }

        ::google::protobuf::SourceLocation loc;
        descriptor->GetSourceLocation(&loc);

        utils::trim(loc.leading_comments);
        if (loc.leading_comments.size() > 0) {
            auto firstEntry = loc.leading_comments.find('\n');
            bool isSingleLine = firstEntry == std::string::npos;

            if (loc.leading_comments[0] != '!' && loc.leading_comments[0] != '*' && loc.leading_comments[0] != ' ') {
                loc.leading_comments = " " + loc.leading_comments;
                if (!isSingleLine) {
                    loc.leading_comments = "\n" + loc.leading_comments;
                }
            }
            mPrinter->Print("\n/*");
            if (isSingleLine) {
                mPrinter->Print(loc.leading_comments.c_str());
            } else {
                utils::replace(loc.leading_comments, "\n", "\n *");
                mPrinter->Print(loc.leading_comments.c_str());
                if (loc.leading_comments[loc.leading_comments.size() - 1] != '\n') {
                    mPrinter->Print("\n");
                }
            }
            mPrinter->Print(" */");
        }
    }

    void Indent() {
        mPrinter->Indent();
        mPrinter->Indent();
    }

    void Outdent() {
        mPrinter->Outdent();
        mPrinter->Outdent();
    }

protected:
    std::shared_ptr<::google::protobuf::io::Printer> mPrinter;
    std::vector<std::string> mNamespaces;
};

} //namespace generator
} //namespace QtProtobuf
