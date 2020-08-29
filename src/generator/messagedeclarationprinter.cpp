/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>, Tatyana Borisova <tanusshhka@mail.ru>
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

#include "messagedeclarationprinter.h"
#include "utils.h"
#include "generatoroptions.h"
#include "generatorcommon.h"

#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

MessageDeclarationPrinter::MessageDeclarationPrinter(const Descriptor *message, const std::shared_ptr<::google::protobuf::io::Printer> &printer)
    : DescriptorPrinterBase<google::protobuf::Descriptor>(message, printer)
{
    mTypeMap = common::produceMessageTypeMap(message, nullptr);
}

void MessageDeclarationPrinter::printClassForwardDeclarationPrivate()
{
    if (common::hasNestedMessages(mDescriptor)) {
        mPrinter->Print({{"namespace", mName + Templates::QtProtobufNestedNamespace}}, Templates::NamespaceTemplate);
        common::iterateNestedMessages(mDescriptor, [this](const ::google::protobuf::Descriptor *nestedMessage) {
            MessageDeclarationPrinter nesterPrinter(nestedMessage, mPrinter);
            nesterPrinter.printClassForwardDeclarationPrivate();
        });
        mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
    }

    mPrinter->Print({{"classname", mName}}, Templates::ProtoClassForwardDeclarationTemplate);
    mPrinter->Print({{"classname", mName}}, Templates::ComplexListTypeUsingTemplate);
}

void MessageDeclarationPrinter::printClassForwardDeclaration()
{
    printNamespaces();
    printClassForwardDeclarationPrivate();
    encloseNamespaces();
}

void MessageDeclarationPrinter::printClassDeclaration()
{
    printNamespaces();
    printClassDeclarationPrivate();
    encloseNamespaces();
    printMetaTypesDeclaration();//Meta types declaration should be outside of namespaces block
}

void MessageDeclarationPrinter::printClassDeclarationPrivate()
{
    mPrinter->Print({{"namespace", mName + Templates::QtProtobufNestedNamespace}}, Templates::NamespaceTemplate);
    common::iterateNestedMessages(mDescriptor, [this](const ::google::protobuf::Descriptor *nestedMessage) {
        MessageDeclarationPrinter nesterPrinter(nestedMessage, mPrinter);
        nesterPrinter.printClassDeclarationPrivate();
    });
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);

    printComments(mDescriptor);
    printClassDeclarationBegin();
    printClassBody();
    encloseClass();
    printListType();
}

void MessageDeclarationPrinter::printCopyFunctionality()
{
    assert(mDescriptor != nullptr);
    mPrinter->Print({{"classname", mName}},
                    Templates::CopyConstructorDeclarationTemplate);

    mPrinter->Print({{"classname", mName}},
                    Templates::AssignmentOperatorDeclarationTemplate);
}

void MessageDeclarationPrinter::printMoveSemantic()
{
    assert(mDescriptor != nullptr);
    mPrinter->Print({{"classname", mName}},
                    Templates::MoveConstructorDeclarationTemplate);

    mPrinter->Print({{"classname", mName}},
                    Templates::MoveAssignmentOperatorDeclarationTemplate);
}

void MessageDeclarationPrinter::printComparisonOperators()
{
    assert(mDescriptor != nullptr);
    mPrinter->Print({{"classname", mName}}, Templates::EqualOperatorDeclarationTemplate);

    mPrinter->Print({{"classname", mName}}, Templates::NotEqualOperatorDeclarationTemplate);
}

void MessageDeclarationPrinter::printConstructors()
{
    for (int i = 0; i <= mDescriptor->field_count(); i++) {
        printConstructor(i);
    }

    if (mDescriptor->full_name() == std::string("google.protobuf.Timestamp")) {
        mPrinter->Print("Timestamp(const QDateTime &datetime, QObject *parent = nullptr);\n"
                        "operator QDateTime() const;\n");
    }
}

void MessageDeclarationPrinter::printConstructor(int fieldCount)
{
    std::vector<std::string> parameterList;
    mPrinter->Print({{"classname", mName}}, Templates::ProtoConstructorBeginTemplate);
    for (int i = 0; i < fieldCount; i++) {
        const FieldDescriptor *field = mDescriptor->field(i);
        const char *parameterTemplate = Templates::ConstructorParameterTemplate;
        FieldDescriptor::Type fieldType = field->type();
        if (field->is_repeated() && !field->is_map()) {
            parameterTemplate = Templates::ConstructorRepeatedParameterTemplate;
        } else if (fieldType == FieldDescriptor::TYPE_BYTES
                  || fieldType == FieldDescriptor::TYPE_STRING
                  || fieldType == FieldDescriptor::TYPE_MESSAGE
                  || field->is_map()) {
            parameterTemplate = Templates::ConstructorMessageParameterTemplate;
        }
        mPrinter->Print(common::producePropertyMap(field, mDescriptor), parameterTemplate);
    }

    mPrinter->Print({{"classname", mName}}, Templates::ProtoConstructorEndTemplate);
}

void MessageDeclarationPrinter::printMaps()
{    
    Indent();
    for (int i = 0; i < mDescriptor->field_count(); i++) {
        const FieldDescriptor *field = mDescriptor->field(i);
        if (field->is_map()) {
            const Descriptor *type = field->message_type();
            const char *mapTemplate = type->field(1)->type() == FieldDescriptor::TYPE_MESSAGE ? Templates::MessageMapTypeUsingTemplate : Templates::MapTypeUsingTemplate;
            mPrinter->Print(common::producePropertyMap(field, mDescriptor), mapTemplate);
        }
    }
    Outdent();
}

void MessageDeclarationPrinter::printNested()
{
    Indent();
    common::iterateNestedMessages(mDescriptor, [&](const ::google::protobuf::Descriptor *nestedMessage) {
        mPrinter->Print(common::produceMessageTypeMap(nestedMessage, mDescriptor), Templates::NestedMessageUsingTemplate);
    });
    Outdent();
}

void MessageDeclarationPrinter::printClassDeclarationBegin()
{
    mPrinter->Print({{"classname", mName}}, Templates::ProtoClassDeclarationBeginTemplate);
}

void MessageDeclarationPrinter::printMetaTypesDeclaration()
{
    mPrinter->Print(mTypeMap,
                   Templates::DeclareMetaTypeTemplate);
    mPrinter->Print(mTypeMap,
                   Templates::DeclareComplexListTypeTemplate);

    if (GeneratorOptions::instance().hasQml()) {
        mPrinter->Print(mTypeMap,
                        Templates::DeclareComplexQmlListTypeTemplate);
    }
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, PropertyMap &propertyMap) {
        if (field->type() == FieldDescriptor::TYPE_ENUM
                && common::isLocalEnum(field->enum_type(), mDescriptor)) {
            mPrinter->Print(propertyMap, Templates::DeclareMetaTypeTemplate);
            mPrinter->Print(propertyMap, Templates::DeclareMetaTypeListTemplate);
        } else if (field->is_map()) {
            propertyMap["key_type_underscore"] = propertyMap["key_type"];
            utils::replace(propertyMap["key_type_underscore"], "::", "_");

            propertyMap["value_type_underscore"] = propertyMap["key_type"];
            utils::replace(propertyMap["value_type_underscore"], "::", "_");

            mPrinter->Print(propertyMap, Templates::DeclareMetaTypeMapTemplate);
        }
    });

    common::iterateNestedMessages(mDescriptor, [this](const Descriptor *nestedMessage) {
        MessageDeclarationPrinter nesterPrinter(nestedMessage, mPrinter);
        nesterPrinter.printMetaTypesDeclaration();
    });
}

void MessageDeclarationPrinter::printProperties()
{
    assert(mDescriptor != nullptr);
    //private section
    Indent();

    for (int i = 0; i < mDescriptor->field_count(); i++) {
        const FieldDescriptor *field = mDescriptor->field(i);
        const char *propertyTemplate = Templates::PropertyTemplate;
        if (common::isPureMessage(field)) {
            propertyTemplate = Templates::MessagePropertyTemplate;
        } else if (common::hasQmlAlias(field)) {
            propertyTemplate = Templates::NonScriptablePropertyTemplate;
        } else if (field->is_repeated() && !field->is_map()) {
            propertyTemplate = Templates::RepeatedPropertyTemplate;
        }
        mPrinter->Print(common::producePropertyMap(field, mDescriptor), propertyTemplate);
    }

    //Generate extra QmlListProperty that is mapped to list
    for (int i = 0; i < mDescriptor->field_count(); i++) {
        const FieldDescriptor *field = mDescriptor->field(i);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && field->is_repeated() && !field->is_map()
                && GeneratorOptions::instance().hasQml()) {
            mPrinter->Print(common::producePropertyMap(field, mDescriptor), Templates::QmlListPropertyTemplate);
        } else if (common::hasQmlAlias(field)) {
            mPrinter->Print(common::producePropertyMap(field, mDescriptor), Templates::NonScriptableAliasPropertyTemplate);
        }
    }

    Outdent();
}

void MessageDeclarationPrinter::printGetters()
{
    Indent();

    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, const PropertyMap &propertyMap) {
        printComments(field);
        mPrinter->Print("\n");
        if (common::isPureMessage(field)) {
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
    });
    Outdent();
}

void MessageDeclarationPrinter::printSetters()
{
    Indent();
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, const PropertyMap &propertyMap) {
        switch (field->type()) {
        case FieldDescriptor::TYPE_MESSAGE:
            if (!field->is_map() && !field->is_repeated() && !common::isQtType(field)) {
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
    });
    Outdent();
}

void MessageDeclarationPrinter::printSignals()
{
    Indent();
    for (int i = 0; i < mDescriptor->field_count(); i++) {
        mPrinter->Print(common::producePropertyMap(mDescriptor->field(i), mDescriptor), Templates::SignalTemplate);
    }
    Outdent();
}


void MessageDeclarationPrinter::printPrivateMethods()
{
    Indent();
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, const PropertyMap &propertyMap) {
        if (common::hasQmlAlias(field)) {
            mPrinter->Print(propertyMap, Templates::NonScriptableGetterTemplate);
            mPrinter->Print(propertyMap, Templates::NonScriptableSetterTemplate);
        }
    });
    Outdent();
}

void MessageDeclarationPrinter::printQEnums() {
    if (mDescriptor->enum_type_count() <= 0) {
        return;
    }

    printPublicBlock();

    Indent();
    for (int i = 0; i < mDescriptor->enum_type_count(); i++) {
        const auto enumDescr = mDescriptor->enum_type(i);
        auto typeMap = common::produceEnumTypeMap(enumDescr, mDescriptor);
        mPrinter->Print(typeMap, Templates::EnumDefinitionTemplate);

        Indent();
        for (int j = 0; j < enumDescr->value_count(); j++) {
            const auto valueDescr = enumDescr->value(j);
            mPrinter->Print({{"enumvalue", utils::upperCaseName(valueDescr->name())},
                             {"value", std::to_string(valueDescr->number())}}, Templates::EnumFieldTemplate);
        }
        Outdent();
        mPrinter->Print(Templates::SemicolonBlockEnclosureTemplate);
        mPrinter->Print(typeMap, Templates::QEnumTemplate);
    }

    for (int i = 0; i < mDescriptor->enum_type_count(); i++) {
        const auto enumDescr = mDescriptor->enum_type(i);
        auto typeMap = common::produceEnumTypeMap(enumDescr, mDescriptor);
        mPrinter->Print(typeMap, Templates::EnumTypeRepeatedTemplate);
    }
    Outdent();
}

void MessageDeclarationPrinter::printClassBody()
{
    printProperties();
    printQEnums();

    printPublicBlock();
    printNested();
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
    mPrinter->Print({{"classname", mName}}, Templates::ManualRegistrationDeclaration);
    Outdent();

    printSignalsBlock();
    printSignals();

    printPrivateBlock();
    printPrivateMethods();

    printPrivateBlock();
    printClassMembers();
}

void MessageDeclarationPrinter::printListType()
{
    mPrinter->Print({{"classname", mName}}, Templates::ComplexListTypeUsingTemplate);
}

void MessageDeclarationPrinter::printClassMembers()
{
    Indent();
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, const PropertyMap &propertyMap) {
        if (common::isPureMessage(field)) {
            mPrinter->Print(propertyMap, Templates::ComplexMemberTemplate);
        } else if (field->is_repeated() && !field->is_map()) {
             mPrinter->Print(propertyMap, Templates::ListMemberTemplate);
        } else {
            mPrinter->Print(propertyMap, Templates::MemberTemplate);
        }
    });
    Outdent();
}

void MessageDeclarationPrinter::printDestructor()
{
    mPrinter->Print({{"classname", mName}}, "virtual ~$classname$();\n");
}
