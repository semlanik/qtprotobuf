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

#include "protobufclassgenerator.h"
#include "utils.h"
#include "generatoroptions.h"

#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

ProtobufClassGenerator::ProtobufClassGenerator(const Descriptor *message, const std::shared_ptr<::google::protobuf::io::ZeroCopyOutputStream> &out)
    : ClassGeneratorBase(message->full_name(), out)
    , mMessage(message)
{
}

ProtobufClassGenerator::ProtobufClassGenerator(const ::google::protobuf::Descriptor *message, const std::shared_ptr<::google::protobuf::io::Printer> &printer)
    : ClassGeneratorBase(message->full_name(), printer)
    , mMessage(message)
{

}

void ProtobufClassGenerator::printCopyFunctionality()
{
    assert(mMessage != nullptr);
    mPrinter->Print({{"classname", mClassName}},
                   Templates::CopyConstructorDeclarationTemplate);

    mPrinter->Print({{"classname", mClassName}},
                   Templates::AssignmentOperatorDeclarationTemplate);
}

void ProtobufClassGenerator::printMoveSemantic()
{
    assert(mMessage != nullptr);
    mPrinter->Print({{"classname", mClassName}},
                   Templates::MoveConstructorDeclarationTemplate);

    mPrinter->Print({{"classname", mClassName}},
                   Templates::MoveAssignmentOperatorDeclarationTemplate);
}

void ProtobufClassGenerator::printComparisonOperators()
{
    assert(mMessage != nullptr);
    PropertyMap properties;
    mPrinter->Print({{"classname", mClassName}}, Templates::EqualOperatorDeclarationTemplate);

    mPrinter->Print({{"classname", mClassName}}, Templates::NotEqualOperatorDeclarationTemplate);
}

void ProtobufClassGenerator::printIncludes()
{
    assert(mMessage != nullptr);

    mPrinter->Print(Templates::DefaultProtobufIncludesTemplate);
    if (GeneratorOptions::instance().hasQml()) {
        mPrinter->Print(Templates::QmlProtobufIncludesTemplate);
    }

    std::set<std::string> existingIncludes;
    for (int i = 0; i < mMessage->field_count(); i++) {
        printInclude(mMessage, mMessage->field(i), existingIncludes);
    }
}

bool ProtobufClassGenerator::isListType(const FieldDescriptor *field)
{
    assert(field != nullptr);
    return field && field->is_repeated()
            && field->type() == FieldDescriptor::TYPE_MESSAGE;
}

void ProtobufClassGenerator::printConstructor()
{
    std::vector<std::string> parameterList;
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        std::string fieldTypeName = getTypeName(field, mMessage);
        std::string fieldName = field->name();
        fieldName[0] = static_cast<char>(::tolower(fieldName[0]));
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
                        {"parameter_list", parameters}}, Templates::ProtoConstructorTemplate);
    }
    mPrinter->Print("\n");
}

void ProtobufClassGenerator::printMaps()
{
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);

        if (field->is_map()) {
            std::string keyType = getTypeName(field->message_type()->field(0), mMessage);
            std::string valueType = getTypeName(field->message_type()->field(1), mMessage);
            const char *mapTemplate = Templates::MapTypeUsingTemplate;

            if (field->message_type()->field(1)->type() == FieldDescriptor::TYPE_MESSAGE) {
                mapTemplate = Templates::MessageMapTypeUsingTemplate;
            }

            mPrinter->Print({{"classname",field->message_type()->name()},
                            {"key", keyType},
                            {"value", valueType}}, mapTemplate);
        }
    }
    Outdent();
}

void ProtobufClassGenerator::printLocalEnumsMetaTypesDeclaration()
{
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (field == nullptr || field->enum_type() == nullptr)
            continue;

        if (field->type() == FieldDescriptor::TYPE_ENUM
                && isLocalMessageEnum(mMessage, field)) {
             mPrinter->Print({{"classname", mClassName + "::" + field->enum_type()->name() + Templates::ListSuffix},
                             {"namespaces", mNamespacesColonDelimited}}, Templates::DeclareMetaTypeTemplate);
        }
    }
}

void ProtobufClassGenerator::printMapsMetaTypesDeclaration()
{
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (field->is_map()) {
             mPrinter->Print({{"classname", field->message_type()->name()},
                             {"namespaces", mNamespacesColonDelimited + "::" + mClassName}}, Templates::DeclareMetaTypeTemplate);
        }
    }
}

void ProtobufClassGenerator::printProperties()
{
    assert(mMessage != nullptr);
    //private section
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        const char *propertyTemplate = Templates::PropertyTemplate;
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !field->is_repeated()) {
            propertyTemplate = Templates::MessagePropertyTemplate;
        }
        printField(mMessage, field, propertyTemplate);
    }

    //Generate extra QmlListProperty that is mapped to list
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && field->is_repeated() && !field->is_map()
                && GeneratorOptions::instance().hasQml()) {
            printField(mMessage, field, Templates::QmlListPropertyTemplate);
        }
    }

    Outdent();

    printQEnums(mMessage);

    //public section
    printPublic();
    printMaps();

    //Body
    Indent();
    printConstructor();
    printCopyFunctionality();
    printMoveSemantic();
    printComparisonOperators();

    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !field->is_repeated()) {
            printField(mMessage, field, Templates::GetterPrivateMessageDeclarationTemplate);
            printField(mMessage, field, Templates::GetterMessageDeclarationTemplate);
        } else {
            printField(mMessage, field, Templates::GetterTemplate);
        }
        if (field->is_repeated()) {
            printField(mMessage, field, Templates::GetterContainerExtraDeclarationTemplate);
            if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map()
                    && GeneratorOptions::instance().hasQml()) {
                printField(mMessage, field, Templates::GetterQmlListDeclarationTemplate);
            }
        }
    }

    for (int i = 0; i < mMessage->field_count(); i++) {
        auto field = mMessage->field(i);
        switch (field->type()) {
        case FieldDescriptor::TYPE_MESSAGE:
            if (!field->is_map() && !field->is_repeated()) {
                printField(mMessage, field, Templates::SetterPrivateTemplateDeclarationMessageType);
                printField(mMessage, field, Templates::SetterTemplateDeclarationMessageType);
            } else {
                printField(mMessage, field, Templates::SetterTemplateDeclarationComplexType);
            }
            break;
        case FieldDescriptor::FieldDescriptor::TYPE_STRING:
        case FieldDescriptor::FieldDescriptor::TYPE_BYTES:
            printField(mMessage, field, Templates::SetterTemplateDeclarationComplexType);
            break;
        default:
            printField(mMessage, field, Templates::SetterTemplateSimpleType);
            break;
        }
    }

    Outdent();

    Indent();
    mPrinter->Print({{"classname", mClassName}}, Templates::ManualRegistrationDeclaration);
    Outdent();

    mPrinter->Print(Templates::SignalsBlockTemplate);

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        printField(mMessage, mMessage->field(i), Templates::SignalTemplate);
    }
    Outdent();
}

void ProtobufClassGenerator::printListType()
{
    mPrinter->Print({{"classname", mClassName}}, Templates::ComplexListTypeUsingTemplate);

}

void ProtobufClassGenerator::printClassMembers()
{
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        auto field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !field->is_repeated()) {
            printField(mMessage, field, Templates::ComplexMemberTemplate);
        } else {
            printField(mMessage, field, Templates::MemberTemplate);
        }
    }
    Outdent();
}

std::set<std::string> ProtobufClassGenerator::extractModels() const
{
    std::set<std::string> extractedModels;
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (field->is_repeated()
                && field->type() == FieldDescriptor::TYPE_MESSAGE) {
            std::string typeName = field->message_type()->name();
            extractedModels.insert(typeName);
        }
    }
    return extractedModels;
}

void ProtobufClassGenerator::run()
{
    printDisclaimer();
    printPreamble();
    printIncludes();
    printNamespaces();
    printFieldClassDeclaration();
    printClassDeclaration();
    printProperties();
    printPrivate();
    printClassMembers();
    printPublic();
    printDestructor();
    encloseClass();
    printListType();
    encloseNamespaces();
    printMetaTypeDeclaration();
    printMapsMetaTypesDeclaration();
    printLocalEnumsMetaTypesDeclaration();
}

void ProtobufClassGenerator::printDestructor()
{
    mPrinter->Print({{"classname", mClassName}}, "virtual ~$classname$();\n");
}

void ProtobufClassGenerator::printFieldClassDeclaration()
{
    for(int i = 0; i < mMessage->field_count(); i++) {
        auto field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE
                && !field->is_map() && !field->is_repeated()) {
            mPrinter->Print({{"classname", field->message_type()->name()}}, Templates::ProtoClassDeclarationTemplate);
        }
    }
}
