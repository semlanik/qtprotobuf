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
    const std::map<std::string, std::string> registrationProperties = {{"classname", mClassName},
                                                                       {"namespaces", mNamespacesColonDelimited},
                                                                       {"package", mMessage->file()->package()}
                                                                      };
    mPrinter.Print(registrationProperties,
                   Templates::ComplexTypeRegistrationTemplate);
    Indent();
    mPrinter.Print(registrationProperties, Templates::RegisterQmlListPropertyMetaTypeTemplate);
    mPrinter.Print(registrationProperties, Templates::QmlRegisterTypeTemplate);

    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_ENUM
                && isLocalMessageEnum(mMessage, field)) {
            mPrinter.Print({{"type", mClassName + "::" + field->enum_type()->name() + "List"},
                            {"namespaces", mNamespacesColonDelimited}},
                           Templates::RegisterMetaTypeTemplateNoNamespace);
            mPrinter.Print({{"type", mClassName+ "::" + field->enum_type()->name() + "List"},
                            {"namespaces", mNamespacesColonDelimited}},
                           Templates::RegisterMetaTypeTemplate);
        } else if (field->is_map()) {
            mPrinter.Print({{"type", field->message_type()->name()},
                            {"namespaces", mClassName}},
                           Templates::RegisterMetaTypeTemplate);
            mPrinter.Print({{"type", field->message_type()->name()},
                            {"namespaces", mNamespacesColonDelimited + "::" + mClassName}},
                           Templates::RegisterMetaTypeTemplate);

            mPrinter.Print({{"classname", mClassName},
                            {"type", field->message_type()->name()},
                            {"key_type", getTypeName(field->message_type()->field(0), mMessage)},
                            {"value_type", getTypeName(field->message_type()->field(1), mMessage)}},
                             Templates::MapSerializationRegisterTemplate);
        }
    }

    mPrinter.Print({{"classname", mClassName}}, Templates::RegisterSerializersTemplate);
    Outdent();
    mPrinter.Print(Templates::SimpleBlockEnclosureTemplate);
    printRegistrationHelperInvokation();
    mPrinter.Print(Templates::SimpleBlockEnclosureTemplate);
}

void ProtobufSourceGenerator::printFieldsOrdering() {
    mPrinter.Print({{"type", mClassName}}, Templates::FieldsOrderingContainerTemplate);
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
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
    mPrinter.Print("\n");
}

void ProtobufSourceGenerator::printRegistrationHelperInvokation()
{
    mPrinter.Print(Templates::RegistratorTemplate);
}

void ProtobufSourceGenerator::printConstructor()
{
    std::string parameterList;
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        std::string fieldTypeName = getTypeName(field, mMessage);
        std::string fieldName = field->name();
        fieldName[0] = static_cast<char>(::tolower(fieldName[0]));
        if (field->is_repeated() || field->is_map()) {
            parameterList += "const " + fieldTypeName + " &" + fieldName;
        } else {
            switch (field->type()) {
            case FieldDescriptor::TYPE_DOUBLE:
            case FieldDescriptor::TYPE_FLOAT:
                parameterList += fieldTypeName + " " + fieldName;
                break;
            case FieldDescriptor::TYPE_FIXED32:
            case FieldDescriptor::TYPE_FIXED64:
            case FieldDescriptor::TYPE_INT32:
            case FieldDescriptor::TYPE_INT64:
            case FieldDescriptor::TYPE_SINT32:
            case FieldDescriptor::TYPE_SINT64:
            case FieldDescriptor::TYPE_UINT32:
            case FieldDescriptor::TYPE_UINT64:
                parameterList += fieldTypeName + " " + fieldName;
                break;
            case FieldDescriptor::TYPE_BOOL:
                parameterList += fieldTypeName + " " + fieldName;
                break;
            case FieldDescriptor::TYPE_BYTES:
            case FieldDescriptor::TYPE_STRING:
            case FieldDescriptor::TYPE_MESSAGE:
                parameterList += "const " + fieldTypeName + " &" + fieldName;
                break;
            default:
                parameterList += fieldTypeName + " " + fieldName;
                break;
            }
        }
        parameterList += ", ";
    }
    mPrinter.Print({{"classname", mClassName},
                    {"parameter_list", parameterList}}, Templates::ProtoConstructorDefinitionTemplate);

    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        std::string fieldName = field->name();
        fieldName[0] =  static_cast<char>(::tolower(fieldName[0]));
        mPrinter.Print({{"property_name", fieldName}}, Templates::PropertyInitializerTemplate);
    }
    mPrinter.Print(Templates::ConstructorContentTemplate);
}
