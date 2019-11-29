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

#include "protobufsourcegenerator.h"
#include "utils.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include "generatoroptions.h"

using namespace QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

ProtobufSourceGenerator::ProtobufSourceGenerator(const google::protobuf::Descriptor *message,
                                                 const std::shared_ptr<google::protobuf::io::ZeroCopyOutputStream> &out) :
    ClassSourceGeneratorBase(message->full_name(), out)
  , mMessage(message)
{
}

ProtobufSourceGenerator::ProtobufSourceGenerator(const google::protobuf::Descriptor *message, const std::shared_ptr<::google::protobuf::io::Printer> &printer) :
    ClassSourceGeneratorBase(message->full_name(), printer)
  , mMessage(message)
{
}

void ProtobufSourceGenerator::printRegisterBody()
{
    const std::map<std::string, std::string> registrationProperties = {{"classname", mClassName},
                                                                       {"namespaces", mNamespacesColonDelimited},
                                                                       {"package", mMessage->file()->package()}
                                                                      };
    mPrinter->Print(registrationProperties,
                    Templates::ManualRegistrationComplexTypeDefinition);
    Indent();
    if (GeneratorOptions::instance().hasQml()) {
        mPrinter->Print(registrationProperties, Templates::RegisterQmlListPropertyMetaTypeTemplate);
        mPrinter->Print(registrationProperties, Templates::QmlRegisterTypeTemplate);
    }

    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_ENUM
                && isLocalMessageEnum(mMessage, field)) {
            mPrinter->Print({{"type", mClassName + "::" + field->enum_type()->name() + Templates::ListSuffix},
                             {"namespaces", mNamespacesColonDelimited}},
                            Templates::RegisterMetaTypeTemplateNoNamespace);
            mPrinter->Print({{"type", mClassName + "::" + field->enum_type()->name() + Templates::ListSuffix},
                             {"namespaces", mNamespacesColonDelimited}},
                            Templates::RegisterMetaTypeTemplate);
            mPrinter->Print({{"type", mClassName + "::" + field->enum_type()->name()},
                             {"namespaces", mNamespacesColonDelimited}},
                            Templates::RegisterMetaTypeTemplateNoNamespace);
            mPrinter->Print({{"type", mClassName + "::" + field->enum_type()->name()},
                             {"namespaces", mNamespacesColonDelimited}},
                            Templates::RegisterMetaTypeTemplate);
            mPrinter->Print({{"type", mClassName + "::" + field->enum_type()->name()}},
                            Templates::RegisterEnumSerializersTemplate);
        } else if (field->is_map()) {
            mPrinter->Print({{"type", field->message_type()->name()},
                             {"namespaces", mClassName}},
                            Templates::RegisterMetaTypeTemplate);
            mPrinter->Print({{"type", field->message_type()->name()},
                             {"namespaces", mNamespacesColonDelimited + "::" + mClassName}},
                            Templates::RegisterMetaTypeTemplate);

            mPrinter->Print({{"classname", mClassName},
                             {"type", field->message_type()->name()},
                             {"key_type", getTypeName(field->message_type()->field(0), mMessage)},
                             {"value_type", getTypeName(field->message_type()->field(1), mMessage)}},
                            Templates::MapSerializationRegisterTemplate);
        }
    }

    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
}

void ProtobufSourceGenerator::printFieldsOrdering() {
    mPrinter->Print({{"type", mClassName}}, Templates::FieldsOrderingContainerTemplate);
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (i != 0) {
            mPrinter->Print("\n,");
        }
        //property_number is incremented by 1 because user properties stating from 1.
        //Property with index 0 is "objectName"
        mPrinter->Print({{"field_number", std::to_string(field->number())},
                         {"property_number", std::to_string(i + 1)}}, Templates::FieldOrderTemplate);
    }
    Outdent();
    mPrinter->Print(Templates::SemicolonBlockEnclosureTemplate);
    mPrinter->Print("\n");
}

void ProtobufSourceGenerator::printConstructor()
{
    std::vector<std::string> parameterList;
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        std::string fieldTypeName = getTypeName(field, mMessage);
        std::string fieldName = field->name();
        fieldName[0] = static_cast<char>(::tolower(fieldName[0]));
        fieldName = qualifiedName(fieldName);

        if (field->is_repeated() || field->is_map()) {
            parameterList.push_back("const " + fieldTypeName + " &" + fieldName);
        } else {
            switch (field->type()) {
            case FieldDescriptor::TYPE_DOUBLE:
            case FieldDescriptor::TYPE_FLOAT:
                parameterList.push_back(fieldTypeName + " " + fieldName);
                break;
            case FieldDescriptor::TYPE_FIXED32:
            case FieldDescriptor::TYPE_FIXED64:
            case FieldDescriptor::TYPE_INT32:
            case FieldDescriptor::TYPE_INT64:
            case FieldDescriptor::TYPE_SINT32:
            case FieldDescriptor::TYPE_SINT64:
            case FieldDescriptor::TYPE_UINT32:
            case FieldDescriptor::TYPE_UINT64:
                parameterList.push_back(fieldTypeName + " " + fieldName);
                break;
            case FieldDescriptor::TYPE_BOOL:
                parameterList.push_back(fieldTypeName + " " + fieldName);
                break;
            case FieldDescriptor::TYPE_BYTES:
            case FieldDescriptor::TYPE_STRING:
            case FieldDescriptor::TYPE_MESSAGE:
                parameterList.push_back("const " + fieldTypeName + " &" + fieldName);
                break;
            default:
                parameterList.push_back(fieldTypeName + " " + fieldName);
                break;
            }
        }
    }

    for (size_t i = 0; i <= parameterList.size(); i++) {
        std::string parameters = "";
        for (size_t j = 0; j < i; j++) {
            parameters += parameterList[j] + ", ";
        }
        mPrinter->Print({{"classname", mClassName},
                         {"parameter_list", parameters}}, Templates::ProtoConstructorDefinitionTemplate);
        for (size_t j = 0; j < parameterList.size(); j++) {
            const FieldDescriptor *field = mMessage->field(j);
            std::string fieldName = field->name();
            auto fieldTypeName = getTypeName(field, mMessage);
            fieldName[0] = static_cast<char>(::tolower(fieldName[0]));
            fieldName = qualifiedName(fieldName);
            if (field->type() == FieldDescriptor::TYPE_MESSAGE
                    && !field->is_map() && !field->is_repeated()) {
                if (j < i) {
                    mPrinter->Print({{"property_name", fieldName}, {"type", fieldTypeName}}, Templates::MessagePropertyInitializerTemplate);
                } else {
                    mPrinter->Print({{"property_name", fieldName}, {"type", fieldTypeName}}, Templates::MessagePropertyEmptyInitializerTemplate);
                }
            } else {
                if (j < i) {
                    mPrinter->Print({{"property_name", fieldName}}, Templates::PropertyInitializerTemplate);
                } else {
                    std::string initializer = "";
                    if (!field->is_repeated() && !field->is_map()) {
                        switch (field->type()) {
                        case FieldDescriptor::TYPE_DOUBLE:
                        case FieldDescriptor::TYPE_FLOAT:
                            initializer = "0.0";
                            break;
                        case FieldDescriptor::TYPE_FIXED32:
                        case FieldDescriptor::TYPE_FIXED64:
                        case FieldDescriptor::TYPE_INT32:
                        case FieldDescriptor::TYPE_INT64:
                        case FieldDescriptor::TYPE_SINT32:
                        case FieldDescriptor::TYPE_SINT64:
                        case FieldDescriptor::TYPE_UINT32:
                        case FieldDescriptor::TYPE_UINT64:
                            initializer = "0";
                            break;
                        case FieldDescriptor::TYPE_BOOL:
                            initializer = "false";
                            break;
                        case FieldDescriptor::TYPE_ENUM:
                            initializer = fieldTypeName + "::" + field->enum_type()->value(0)->name();
                            break;
                        default:
                            break;
                        }
                    }
                    if (initializer != "") {
                        mPrinter->Print({{"property_name", fieldName}, {"initializer", initializer}}, Templates::PropertyEmptyInitializerTemplate);
                    }
                }
            }
        }
        mPrinter->Print(Templates::ConstructorContentTemplate);
        mPrinter->Print("\n");
    }
}

void ProtobufSourceGenerator::printCopyFunctionality()
{
    mPrinter->Print({{"classname", mClassName}},
                    Templates::CopyConstructorDefinitionTemplate);
    for (int j = 0; j < mMessage->field_count(); j++) {
        const FieldDescriptor *field = mMessage->field(j);
        std::string fieldName = field->name();
        auto fieldTypeName = getTypeName(field, mMessage);
        fieldName[0] = static_cast<char>(::tolower(fieldName[0]));
        fieldName = qualifiedName(fieldName);

        if (field->type() == FieldDescriptor::TYPE_MESSAGE
                && !field->is_map() && !field->is_repeated()) {
            mPrinter->Print({{"property_name", fieldName}, {"type", fieldTypeName}}, Templates::MessagePropertyEmptyInitializerTemplate);
        }
    }
    mPrinter->Print("\n{\n");

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        auto field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !field->is_repeated()) {
            printField(mMessage, field, Templates::CopyComplexFieldTemplate);
        } else {
            printField(mMessage, field, Templates::CopyFieldTemplate);
        }
    }
    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);

    mPrinter->Print({{"classname", mClassName}},
                    Templates::AssignmentOperatorDefinitionTemplate);
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        auto field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !field->is_repeated()) {
            printField(mMessage, field, Templates::CopyComplexFieldTemplate);
        } else {
            printField(mMessage, field, Templates::CopyFieldTemplate);
        }
    }
    mPrinter->Print(Templates::AssignmentOperatorReturnTemplate);
    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
}

void ProtobufSourceGenerator::printMoveSemantic()
{
    assert(mMessage != nullptr);
    mPrinter->Print({{"classname", mClassName}},
                    Templates::MoveConstructorDefinitionTemplate);
    for (int j = 0; j < mMessage->field_count(); j++) {
        const FieldDescriptor *field = mMessage->field(j);
        std::string fieldName = field->name();
        auto fieldTypeName = getTypeName(field, mMessage);
        fieldName[0] = static_cast<char>(::tolower(fieldName[0]));
        fieldName = qualifiedName(fieldName);

        if (field->type() == FieldDescriptor::TYPE_MESSAGE
                && !field->is_map() && !field->is_repeated()) {
            mPrinter->Print({{"property_name", fieldName}, {"type", fieldTypeName}}, Templates::MessagePropertyEmptyInitializerTemplate);
        }
    }
    mPrinter->Print("\n{\n");

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (isComplexType(field) || field->is_repeated()) {
            if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !field->is_repeated()) {
                printField(mMessage, field, Templates::MoveMessageFieldTemplate);
            } else {
                printField(mMessage, field, Templates::MoveComplexFieldTemplate);
            }
        } else {
            if (field->type() != FieldDescriptor::TYPE_ENUM) {
                printField(mMessage, field, Templates::MoveFieldTemplate);
            }
            else {
                printField(mMessage, field, Templates::EnumMoveFieldTemplate);
            }
        }
    }
    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);

    mPrinter->Print({{"classname", mClassName}},
                    Templates::MoveAssignmentOperatorDefinitionTemplate);
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (isComplexType(field) || field->is_repeated()) {
            if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !field->is_repeated()) {
                printField(mMessage, field, Templates::MoveMessageFieldTemplate);
            } else {
                printField(mMessage, field, Templates::MoveComplexFieldTemplate);
            }
        } else {
            if (field->type() != FieldDescriptor::TYPE_ENUM) {
                printField(mMessage, field, Templates::MoveFieldTemplate);
            }
            else {
                printField(mMessage, field, Templates::EnumMoveFieldTemplate);
            }
        }
    }
    mPrinter->Print(Templates::AssignmentOperatorReturnTemplate);
    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
}

void ProtobufSourceGenerator::printComparisonOperators()
{
    assert(mMessage != nullptr);
    PropertyMap properties;
    mPrinter->Print({{"classname", mClassName}}, Templates::EqualOperatorDefinitionTemplate);

    bool isFirst = true;
    if (mMessage->field_count() <= 0) {
        mPrinter->Print("true");
    }
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (producePropertyMap(mMessage, field, properties)) {
            if (!isFirst) {
                mPrinter->Print("\n&& ");
            } else {
                Indent();
                Indent();
                isFirst = false;
            }
            if (field->type() == FieldDescriptor::TYPE_MESSAGE
                    && !field->is_map() && !field->is_repeated()) {
                mPrinter->Print(properties, Templates::EqualOperatorMessagePropertyTemplate);
            } else {
                mPrinter->Print(properties, Templates::EqualOperatorPropertyTemplate);
            }
        }
    }

    //Only if at least one field "copied"
    if (!isFirst) {
        Outdent();
        Outdent();
    }

    mPrinter->Print(";\n");
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);

    mPrinter->Print({{"classname", mClassName}}, Templates::NotEqualOperatorDefinitionTemplate);
}

void ProtobufSourceGenerator::printGetters()
{
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !field->is_repeated()) {
            printField(mMessage, field, Templates::GetterPrivateMessageDefinitionTemplate);
            printField(mMessage, field, Templates::GetterMessageDefinitionTemplate);
        }
        if (field->is_repeated()) {
            printField(mMessage, field, Templates::GetterContainerExtraDefinitionTemplate);
            if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map()
                    && GeneratorOptions::instance().hasQml()) {
                printField(mMessage, field, Templates::GetterQmlListDefinitionTemplate);
            }
        }
    }
    for (int i = 0; i < mMessage->field_count(); i++) {
        auto field = mMessage->field(i);
        switch (field->type()) {
        case FieldDescriptor::TYPE_MESSAGE:
            if (!field->is_map() && !field->is_repeated()) {
                printField(mMessage, field, Templates::SetterPrivateTemplateDefinitionMessageType);
                printField(mMessage, field, Templates::SetterTemplateDefinitionMessageType);
            } else {
                printField(mMessage, field, Templates::SetterTemplateDefinitionComplexType);
            }
            break;
        case FieldDescriptor::FieldDescriptor::TYPE_STRING:
        case FieldDescriptor::FieldDescriptor::TYPE_BYTES:
            printField(mMessage, field, Templates::SetterTemplateDefinitionComplexType);
            break;
        default:
            break;
        }
    }
}

void ProtobufSourceGenerator::printDestructor()
{
    mPrinter->Print({{"classname", mClassName}}, "$classname$::~$classname$()\n"
                                                 "{}\n\n");
}

void ProtobufSourceGenerator::printIncludes()
{
    std::set<std::string> existingIncludes;
    for (int i = 0; i < mMessage->field_count(); i++) {
        printInclude(mMessage, mMessage->field(i), existingIncludes);
    }
}
