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

#include "protobufsourcegenerator.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>

using namespace qtprotobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

ProtobufSourceGenerator::ProtobufSourceGenerator(const google::protobuf::Descriptor *message,
                                                 std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> out) :
    ClassSourceGeneratorBase(message->full_name(), std::move(out))
  , mMessage(message)
{

}

void ProtobufSourceGenerator::printRegisterBody()
{
    mPrinter.Print({{"classname", mClassName}, {"namespaces", mNamespacesColonDelimited}}, Templates::ComplexTypeRegistrationTemplate);
}

void ProtobufSourceGenerator::printFieldsOrdering() {
    mPrinter.Print({{"type", mClassName}}, Templates::FieldsOrderingContainerTemplate);
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        if (i != 0) {
            mPrinter.Print("\n,");
        }
        //property_number is incremented by 1 because user properties stating from 1.
        //Property with index 0 is "objectName"
        mPrinter.Print({{"field_number", std::to_string(field->number())},
                        {"property_number", std::to_string(i + 1)}}, Templates::FieldOrderTemplate);
    }
    Outdent();
    mPrinter.Print(Templates::SemicolonBlockEnclosureTemplate);
}
