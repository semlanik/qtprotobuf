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
#include "generatorcommon.h"

#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.pb.h>

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

ProtobufClassGenerator::ProtobufClassGenerator(const Descriptor *message, const std::shared_ptr<::google::protobuf::io::ZeroCopyOutputStream> &out)
    : ClassGeneratorBase(message->full_name(), out)
    , mMessage(message)
    , mTypeMap(common::produceMessageTypeMap(message, nullptr))
{
}

ProtobufClassGenerator::ProtobufClassGenerator(const ::google::protobuf::Descriptor *message, const std::shared_ptr<::google::protobuf::io::Printer> &printer)
    : ClassGeneratorBase(message->full_name(), printer)
    , mMessage(message)
    , mTypeMap(common::produceMessageTypeMap(message, nullptr))
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

void ProtobufClassGenerator::printConstructors()
{
    for (int i = 0; i <= mMessage->field_count(); i++) {
        printConstructor(i);
    }
}

void ProtobufClassGenerator::printConstructor(int fieldCount)
{
    std::vector<std::string> parameterList;
    mPrinter->Print({{"classname", mClassName}}, Templates::ProtoConstructorBeginTemplate);
    for (int i = 0; i < fieldCount; i++) {
        const FieldDescriptor *field = mMessage->field(i);
        const char *parameterTemplate = Templates::ConstructorParameterTemplate;
        FieldDescriptor::Type fieldType = field->type();
        if (field->is_repeated() && !field->is_map()) {
            parameterTemplate = Templates::ConstructorRepeatedParameterTemplate;
        } else if(fieldType == FieldDescriptor::TYPE_BYTES
                  || fieldType == FieldDescriptor::TYPE_STRING
                  || fieldType == FieldDescriptor::TYPE_MESSAGE
                  || field->is_map()) {
            parameterTemplate = Templates::ConstructorMessageParameterTemplate;
        }
        mPrinter->Print(common::producePropertyMap(field, mMessage), parameterTemplate);
    }

    mPrinter->Print({{"classname", mClassName}}, Templates::ProtoConstructorEndTemplate);
    mPrinter->Print("\n");
}

void ProtobufClassGenerator::printMaps()
{
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);        
        if (field->is_map()) {
            const Descriptor *type = field->message_type();
            const char *mapTemplate = type->field(1)->type() == FieldDescriptor::TYPE_MESSAGE ? Templates::MessageMapTypeUsingTemplate : Templates::MapTypeUsingTemplate;
            mPrinter->Print(common::producePropertyMap(field, mMessage), mapTemplate);
        }
    }
    Outdent();
}

void ProtobufClassGenerator::printMetaTypesDeclaration()
{
    mPrinter->Print(mTypeMap,
                   Templates::DeclareMetaTypeTemplate);
    mPrinter->Print(mTypeMap,
                   Templates::DeclareComplexListTypeTemplate);

    if (GeneratorOptions::instance().hasQml()) {
        mPrinter->Print(mTypeMap,
                        Templates::DeclareComplexQmlListTypeTemplate);
    }

    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        auto fieldMap = common::producePropertyMap(field, nullptr);
        if (field->type() == FieldDescriptor::TYPE_ENUM
                && common::isLocalEnum(field->enum_type(), mMessage)) {
            mPrinter->Print(fieldMap, Templates::DeclareMetaTypeTemplate);
        } else if (field->is_map()) {
            fieldMap["key_type_underscore"] = fieldMap["key_type"];
            utils::replace(fieldMap["key_type_underscore"], "::", "_");

            fieldMap["value_type_underscore"] = fieldMap["key_type"];
            utils::replace(fieldMap["value_type_underscore"], "::", "_");

            mPrinter->Print(fieldMap, Templates::DeclareMetaTypeMapTemplate);
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
        } else if (common::hasQmlAlias(field)) {
            propertyTemplate = Templates::NonScriptablePropertyTemplate;
        } else if (field->is_repeated() && !field->is_map()) {
            propertyTemplate = Templates::RepeatedPropertyTemplate;
        }
        mPrinter->Print(common::producePropertyMap(field, mMessage), propertyTemplate);
    }

    //Generate extra QmlListProperty that is mapped to list
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && field->is_repeated() && !field->is_map()
                && GeneratorOptions::instance().hasQml()) {
            mPrinter->Print(common::producePropertyMap(field, mMessage), Templates::QmlListPropertyTemplate);
        } else if (common::hasQmlAlias(field)) {
            mPrinter->Print(common::producePropertyMap(field, mMessage), Templates::NonScriptableAliasPropertyTemplate);
        }
    }

    Outdent();
}

void ProtobufClassGenerator::printGetters()
{
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor *field = mMessage->field(i);
        printComments(field);
        mPrinter->Print("\n");

        PropertyMap propertyMap = common::producePropertyMap(field, mMessage);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !field->is_repeated()) {
            mPrinter->Print(propertyMap, Templates::GetterPrivateMessageDeclarationTemplate);
            mPrinter->Print(propertyMap, Templates::GetterMessageDeclarationTemplate);
        } else {
            mPrinter->Print(propertyMap, Templates::GetterTemplate);
        }

        if (field->is_repeated()) {
            mPrinter->Print(propertyMap, Templates::GetterContainerExtraTemplate);
            if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map()
                    && GeneratorOptions::instance().hasQml()) {
                mPrinter->Print(propertyMap, Templates::GetterQmlListDeclarationTemplate);
            }
        }
    }
    Outdent();
}

void ProtobufClassGenerator::printSetters()
{
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        auto field = mMessage->field(i);
        PropertyMap propertyMap = common::producePropertyMap(field, mMessage);
        switch (field->type()) {
        case FieldDescriptor::TYPE_MESSAGE:
            if (!field->is_map() && !field->is_repeated()) {
                mPrinter->Print(propertyMap, Templates::SetterPrivateTemplateDeclarationMessageType);
                mPrinter->Print(propertyMap, Templates::SetterTemplateDeclarationMessageType);
            } else {
                mPrinter->Print(propertyMap, Templates::SetterTemplateDeclarationComplexType);
            }
            break;
        case FieldDescriptor::FieldDescriptor::TYPE_STRING:
        case FieldDescriptor::FieldDescriptor::TYPE_BYTES:
            mPrinter->Print(propertyMap, Templates::SetterTemplateDeclarationComplexType);
            break;
        default:
            mPrinter->Print(propertyMap, Templates::SetterTemplate);
            break;
        }
    }
    Outdent();
}

void ProtobufClassGenerator::printSignals()
{
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        mPrinter->Print(common::producePropertyMap(mMessage->field(i), mMessage), Templates::SignalTemplate);
    }
    Outdent();
}


void ProtobufClassGenerator::printPrivateMethods()
{
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        auto field = mMessage->field(i);
        auto propertyMap = common::producePropertyMap(field, mMessage);
        if (common::hasQmlAlias(field)) {
            mPrinter->Print(propertyMap, Templates::NonScriptableGetterTemplate);
            mPrinter->Print(propertyMap, Templates::NonScriptableSetterTemplate);
        }
    }
    Outdent();
}

void ProtobufClassGenerator::printClassBody()
{
    printProperties();
    printQEnums(mMessage);

    printPublicBlock();
    printMaps();

    Indent();
    printConstructors();
    printDestructor();

    printCopyFunctionality();
    printMoveSemantic();

    printComparisonOperators();
    Outdent();

    printGetters();
    printSetters();

    Indent();
    mPrinter->Print({{"classname", mClassName}}, Templates::ManualRegistrationDeclaration);
    Outdent();

    printSignalsBlock();
    printSignals();

    printPrivateBlock();
    printPrivateMethods();

    printPrivateBlock();
    printClassMembers();
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
        auto propertyMap = common::producePropertyMap(field, mMessage);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !field->is_repeated()) {
            mPrinter->Print(propertyMap, Templates::ComplexMemberTemplate);
        } else if(field->is_repeated() && !field->is_map()){
             mPrinter->Print(propertyMap, Templates::ListMemberTemplate);
        } else {
            mPrinter->Print(propertyMap, Templates::MemberTemplate);
        }
    }
    Outdent();
}

void ProtobufClassGenerator::run()
{
    printDisclaimer();
    printPreamble();
    printIncludes();
    printNamespaces();
    printFieldClassDeclaration();
    printComments(mMessage);
    printClassDeclaration();
    printClassBody();
    encloseClass();
    printListType();
    encloseNamespaces();
    printMetaTypesDeclaration();
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
            mPrinter->Print({{"classname", utils::upperCaseName(field->message_type()->name())}}, Templates::ProtoClassDeclarationTemplate);
        }
    }
}
