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

#include "messagedefinitionprinter.h"

#include <google/protobuf/descriptor.h>
#include "generatoroptions.h"

using namespace QtProtobuf::generator;
using namespace ::google::protobuf;

MessageDefinitionPrinter::MessageDefinitionPrinter(const Descriptor *message, const std::shared_ptr<::google::protobuf::io::Printer> &printer) :
    DescriptorPrinterBase<Descriptor>(message, printer)
{
    mTypeMap = common::produceMessageTypeMap(message, nullptr);
}

void MessageDefinitionPrinter::printClassDefinitionPrivate()
{
    if (common::hasNestedMessages(mDescriptor)) {
        mPrinter->Print({{"namespace", mTypeMap["classname"] + Templates::QtProtobufNestedNamespace}}, Templates::NamespaceTemplate);
        common::iterateNestedMessages(mDescriptor, [this](const Descriptor *nestedMessage) {
            MessageDefinitionPrinter nestedPrinter(nestedMessage, mPrinter);
            nestedPrinter.printClassDefinitionPrivate();
        });
        mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
    }

    printDestructor();
    printFieldsOrdering();
    printRegisterBody();
    printConstructors();
    printCopyFunctionality();
    printMoveSemantic();
    printComparisonOperators();
    printGetters();
}

void MessageDefinitionPrinter::printClassDefinition()
{
    printNamespaces();
    printClassDefinitionPrivate();
    encloseNamespaces();
}

void MessageDefinitionPrinter::printRegisterBody()
{
    mPrinter->Print(mTypeMap,
                    Templates::ManualRegistrationComplexTypeDefinition);
    Indent();
    if (GeneratorOptions::instance().hasQml()) {
        mPrinter->Print(mTypeMap, Templates::RegisterQmlListPropertyMetaTypeTemplate);
        mPrinter->Print(mTypeMap, Templates::QmlRegisterTypeTemplate);
    }

    common::iterateMessageFields(mDescriptor, [this](const FieldDescriptor *field, const PropertyMap &propertyMap) {
        if (field->type() == FieldDescriptor::TYPE_ENUM
                && common::isLocalEnum(field->enum_type(), mDescriptor)) {
            mPrinter->Print(propertyMap, Templates::RegisterLocalEnumTemplate);
        } else if (field->is_map()) {
            mPrinter->Print(propertyMap, Templates::RegisterMapTemplate);
        }
    });

    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
}

void MessageDefinitionPrinter::printFieldsOrdering() {
    mPrinter->Print({{"type", mTypeMap["classname"]}}, Templates::FieldsOrderingContainerTemplate);
    Indent();
    for (int i = 0; i < mDescriptor->field_count(); i++) {
        const FieldDescriptor *field = mDescriptor->field(i);
        if (i != 0) {
            mPrinter->Print("\n,");
        }
        //property_number is incremented by 1 because user properties stating from 1.
        //Property with index 0 is "objectName"
        mPrinter->Print({{"field_number", std::to_string(field->number())},
                         {"property_number", std::to_string(i + 1)},
                         {"json_name", field->json_name()}}, Templates::FieldOrderTemplate);
    }
    Outdent();
    mPrinter->Print(Templates::SemicolonBlockEnclosureTemplate);
    mPrinter->Print("\n");
}

void MessageDefinitionPrinter::printConstructors() {
    for (int i = 0; i <= mDescriptor->field_count(); i++) {
        mPrinter->Print(mTypeMap, Templates::ProtoConstructorDefinitionBeginTemplate);
        printConstructor(i);
        mPrinter->Print(mTypeMap, Templates::ProtoConstructorDefinitionEndTemplate);
        printInitializationList(i);
        mPrinter->Print(Templates::ConstructorContentTemplate);
    }

    if (mDescriptor->full_name() == std::string("google.protobuf.Timestamp")) {
        mPrinter->Print("Timestamp::Timestamp(const QDateTime &datetime, QObject *parent) : QObject(parent)\n"
                        ", m_seconds(datetime.toMSecsSinceEpoch() / 1000)\n"
                        ", m_nanos((datetime.toMSecsSinceEpoch() % 1000) * 1000)\n"
                        "{}\n"
                        "Timestamp::operator QDateTime() const\n"
                        "{\n"
                        "    return QDateTime::fromMSecsSinceEpoch(m_seconds * 1000 + m_nanos / 1000);\n"
                        "}\n");
    }
}

void MessageDefinitionPrinter::printConstructor(int fieldCount)
{
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
}

void MessageDefinitionPrinter::printInitializationList(int fieldCount)
{
    for (int i = 0; i < mDescriptor->field_count(); i++) {
        const FieldDescriptor *field = mDescriptor->field(i);
        auto propertyMap = common::producePropertyMap(field, mDescriptor);
        propertyMap["initializer"] = "";
        if (!field->is_repeated() && !field->is_map()) {
            switch (field->type()) {
            case FieldDescriptor::TYPE_DOUBLE:
            case FieldDescriptor::TYPE_FLOAT:
                propertyMap["initializer"] = "0.0";
                break;
            case FieldDescriptor::TYPE_FIXED32:
            case FieldDescriptor::TYPE_FIXED64:
            case FieldDescriptor::TYPE_INT32:
            case FieldDescriptor::TYPE_INT64:
            case FieldDescriptor::TYPE_SINT32:
            case FieldDescriptor::TYPE_SINT64:
            case FieldDescriptor::TYPE_UINT32:
            case FieldDescriptor::TYPE_UINT64:
                propertyMap["initializer"] = "0";
                break;
            case FieldDescriptor::TYPE_BOOL:
                propertyMap["initializer"] = "false";
                break;
            case FieldDescriptor::TYPE_ENUM:
                propertyMap["initializer"] = propertyMap["scope_type"] + "::" + field->enum_type()->value(0)->name();
                break;
            default:
                break;
            }
        }

        if (common::isPureMessage(field)) {
            if (i < fieldCount) {
                mPrinter->Print(propertyMap, Templates::MessagePropertyInitializerTemplate);
            } else {
                mPrinter->Print(propertyMap, Templates::MessagePropertyDefaultInitializerTemplate);
            }
        } else {
            if (i < fieldCount) {
                mPrinter->Print(propertyMap, Templates::PropertyInitializerTemplate);
            } else {
                if (!propertyMap["initializer"].empty()) {
                    mPrinter->Print(propertyMap, Templates::PropertyDefaultInitializerTemplate);
                }
            }
        }

        if (field->is_optional()) {
            mPrinter->Print(common::produceOptionalPropertyMap(field, mDescriptor), Templates::PropertyDefaultInitializerTemplate);
        }
    }
}

void MessageDefinitionPrinter::printCopyFunctionality()
{
    assert(mDescriptor != nullptr);

    const char *constructorTemplate = Templates::CopyConstructorDefinitionTemplate;
    const char *assignmentOperatorTemplate = Templates::AssignmentOperatorDefinitionTemplate;
    if (mDescriptor->field_count() <= 0) {
        constructorTemplate = Templates::EmptyCopyConstructorDefinitionTemplate;
        assignmentOperatorTemplate = Templates::EmptyAssignmentOperatorDefinitionTemplate;
    }

    mPrinter->Print(mTypeMap,
                    constructorTemplate);
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, const PropertyMap &propertyMap) {
        if (common::isPureMessage(field)) {
            mPrinter->Print(propertyMap, Templates::MessagePropertyDefaultInitializerTemplate);
        }
    });
    mPrinter->Print("\n{\n");

    Indent();
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, const PropertyMap &propertyMap) {
        if (common::isPureMessage(field)) {
            mPrinter->Print(propertyMap, Templates::CopyComplexFieldTemplate);
        } else {
            mPrinter->Print(propertyMap, Templates::CopyFieldTemplate);
        }
    });
    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);

    mPrinter->Print(mTypeMap, assignmentOperatorTemplate);
    Indent();
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, const PropertyMap &propertyMap) {
        if (common::isPureMessage(field)) {
            mPrinter->Print(propertyMap, Templates::AssignComplexFieldTemplate);
        } else {
            mPrinter->Print(propertyMap, Templates::CopyFieldTemplate);
        }
    });
    mPrinter->Print(Templates::AssignmentOperatorReturnTemplate);
    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
}

void MessageDefinitionPrinter::printMoveSemantic()
{
    assert(mDescriptor != nullptr);

    const char *constructorTemplate = Templates::MoveConstructorDefinitionTemplate;
    const char *assignmentOperatorTemplate = Templates::MoveAssignmentOperatorDefinitionTemplate;
    if (mDescriptor->field_count() <= 0) {
        constructorTemplate = Templates::EmptyMoveConstructorDefinitionTemplate;
        assignmentOperatorTemplate = Templates::EmptyMoveAssignmentOperatorDefinitionTemplate;
    }

    mPrinter->Print(mTypeMap,
                    constructorTemplate);
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, const PropertyMap &propertyMap) {
        if (common::isPureMessage(field)) {
            mPrinter->Print(propertyMap, Templates::MessagePropertyDefaultInitializerTemplate);
        }
    });
    mPrinter->Print("\n{\n");

    Indent();
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, const PropertyMap &propertyMap) {
        if (field->type() == FieldDescriptor::TYPE_MESSAGE
                || field->type() == FieldDescriptor::TYPE_STRING
                || field->type() == FieldDescriptor::TYPE_BYTES
                || field->is_repeated()) {
            if (common::isPureMessage(field)) {
                mPrinter->Print(propertyMap, Templates::MoveMessageFieldTemplate);
            } else {
                mPrinter->Print(propertyMap, Templates::MoveComplexFieldConstructorTemplate);
            }
        } else {
            if (field->type() != FieldDescriptor::TYPE_ENUM) {
                mPrinter->Print(propertyMap, Templates::MoveFieldTemplate);
            }
            else {
                mPrinter->Print(propertyMap, Templates::EnumMoveFieldTemplate);
            }
        }
    });
    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);

    mPrinter->Print(mTypeMap, assignmentOperatorTemplate);
    Indent();
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, const PropertyMap &propertyMap) {
        if (field->type() == FieldDescriptor::TYPE_MESSAGE
                || field->type() == FieldDescriptor::TYPE_STRING
                || field->type() == FieldDescriptor::TYPE_BYTES
                || field->is_repeated()) {
            if (common::isPureMessage(field)) {
                mPrinter->Print(propertyMap, Templates::MoveAssignMessageFieldTemplate);
            } else {
                mPrinter->Print(propertyMap, Templates::MoveComplexFieldTemplate);
            }
        } else {
            if (field->type() != FieldDescriptor::TYPE_ENUM) {
                mPrinter->Print(propertyMap, Templates::MoveFieldTemplate);
            }
            else {
                mPrinter->Print(propertyMap, Templates::EnumMoveFieldTemplate);
            }
        }
    });
    mPrinter->Print(Templates::AssignmentOperatorReturnTemplate);
    Outdent();
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
}

void MessageDefinitionPrinter::printComparisonOperators()
{
    assert(mDescriptor != nullptr);
    if (mDescriptor->field_count() <= 0) {
        mPrinter->Print(mTypeMap, Templates::EmptyEqualOperatorDefinitionTemplate);
        mPrinter->Print(mTypeMap, Templates::NotEqualOperatorDefinitionTemplate);
        return;
    }

    mPrinter->Print(mTypeMap, Templates::EqualOperatorDefinitionTemplate);

    bool isFirst = true;
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, PropertyMap &propertyMap) {
        if (!isFirst) {
            mPrinter->Print("\n&& ");
        } else {
            Indent();
            Indent();
            isFirst = false;
        }
        if (common::isPureMessage(field)) {
            mPrinter->Print(propertyMap, Templates::EqualOperatorMessagePropertyTemplate);
        } else if (field->type() == FieldDescriptor::TYPE_MESSAGE && field->is_repeated()) {
            mPrinter->Print(propertyMap, Templates::EqualOperatorRepeatedPropertyTemplate);
        } else {
            mPrinter->Print(propertyMap, Templates::EqualOperatorPropertyTemplate);
        }
    });

    //Only if at least one field "copied"
    if (!isFirst) {
        Outdent();
        Outdent();
    }

    mPrinter->Print(";\n");
    mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);

    mPrinter->Print(mTypeMap, Templates::NotEqualOperatorDefinitionTemplate);
}

void MessageDefinitionPrinter::printGetters()
{
    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, PropertyMap &propertyMap) {
        if (common::isPureMessage(field)) {
            mPrinter->Print(propertyMap, Templates::GetterPrivateMessageDefinitionTemplate);
            mPrinter->Print(propertyMap, Templates::GetterMessageDefinitionTemplate);
        }
        if (field->is_repeated()) {
            if (field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !common::isQtType(field)
                    && GeneratorOptions::instance().hasQml()) {
                mPrinter->Print(propertyMap, Templates::GetterQmlListDefinitionTemplate);
            }
        }
    });

    common::iterateMessageFields(mDescriptor, [&](const FieldDescriptor *field, PropertyMap &propertyMap) {
        switch (field->type()) {
        case FieldDescriptor::TYPE_MESSAGE:
            if (!field->is_map() && !field->is_repeated() && !common::isQtType(field)) {
                mPrinter->Print(propertyMap, Templates::SetterPrivateTemplateDefinitionMessageType);
                if (field->is_optional()) {
                    mPrinter->Print(propertyMap, Templates::EndOptionalSetterTemplate);
                } else {
                    mPrinter->Print(propertyMap, Templates::EndSetterTemplate);
                }

                mPrinter->Print(propertyMap, Templates::SetterTemplateDefinitionMessageType);
            } else {
                mPrinter->Print(propertyMap, Templates::SetterTemplateDefinitionComplexType);
            }
            if (field->is_optional()) {
                mPrinter->Print(propertyMap, Templates::EndOptionalSetterTemplate);
            } else {
                mPrinter->Print(propertyMap, Templates::EndSetterTemplate);
            }
            break;
        case FieldDescriptor::FieldDescriptor::TYPE_STRING:
        case FieldDescriptor::FieldDescriptor::TYPE_BYTES:
            mPrinter->Print(propertyMap, Templates::SetterTemplateDefinitionComplexType);
            if (field->is_optional()) {
                mPrinter->Print(propertyMap, Templates::EndOptionalSetterTemplate);
            } else {
                mPrinter->Print(propertyMap, Templates::EndSetterTemplate);
            }
            break;
        default:
            mPrinter->Print(propertyMap, "/* default ending */");
            break;
        }
    });
}

void MessageDefinitionPrinter::printDestructor()
{
    mPrinter->Print(mTypeMap, Templates::RegistrarTemplate);
    mPrinter->Print(mTypeMap, "$classname$::~$classname$()\n"
                                                 "{}\n\n");
}
