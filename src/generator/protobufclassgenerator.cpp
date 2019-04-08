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

#include "protobufclassgenerator.h"
#include "utils.h"

#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>

using namespace ::qtprotobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

namespace {
const std::string VariantList("QVariantList");
}

ProtobufClassGenerator::ProtobufClassGenerator(const Descriptor *message, std::unique_ptr<::google::protobuf::io::ZeroCopyOutputStream> out)
    : ClassGeneratorBase(message->full_name(), std::move(out))
    , mMessage(message)
{
}

void ProtobufClassGenerator::printCopyFunctionality()
{
    assert(mMessage != nullptr);
    mPrinter.Print({{"classname", mClassName}},
                   Templates::CopyConstructorTemplate);

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        printField(mMessage->field(i), Templates::CopyFieldTemplate);
    }
    Outdent();

    mPrinter.Print(Templates::SimpleBlockEnclosureTemplate);
    mPrinter.Print({{"classname", mClassName}},
                   Templates::AssignmentOperatorTemplate);

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        printField(mMessage->field(i), Templates::CopyFieldTemplate);
    }
    mPrinter.Print(Templates::AssignmentOperatorReturnTemplate);
    Outdent();

    mPrinter.Print(Templates::SimpleBlockEnclosureTemplate);

}

void ProtobufClassGenerator::printMoveSemantic()
{
    assert(mMessage != nullptr);
    mPrinter.Print({{"classname", mClassName}},
                   Templates::MoveConstructorTemplate);

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        if (isComplexType(field) || field->is_repeated()) {
            printField(field, Templates::MoveComplexFieldTemplate);
        } else {
            if (field->type() != FieldDescriptor::TYPE_ENUM) {
                printField(field, Templates::MoveFieldTemplate);
            }
            else {
                printField(field, Templates::EnumMoveFieldTemplate);
            }
        }
    }
    Outdent();

    mPrinter.Print(Templates::SimpleBlockEnclosureTemplate);
    mPrinter.Print({{"classname", mClassName}},
                   Templates::MoveAssignmentOperatorTemplate);

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        if (isComplexType(field) || field->is_repeated()) {
            printField(field, Templates::MoveComplexFieldTemplate);
        } else {
            if (field->type() != FieldDescriptor::TYPE_ENUM) {
                printField(field, Templates::MoveFieldTemplate);
            }
            else {
                printField(field, Templates::EnumMoveFieldTemplate);
            }
        }
    }
    mPrinter.Print(Templates::AssignmentOperatorReturnTemplate);
    Outdent();

    mPrinter.Print(Templates::SimpleBlockEnclosureTemplate);
}

void ProtobufClassGenerator::printComparisonOperators()
{
    assert(mMessage != nullptr);
    bool isFirst = true;
    PropertyMap properties;
    mPrinter.Print({{"type", mClassName}}, Templates::EqualOperatorTemplate);
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        if (producePropertyMap(field, properties)) {
            if (!isFirst) {
                mPrinter.Print("\n&& ");
            } else {
                Indent();
                Indent();
                isFirst = false;
            }
            mPrinter.Print(properties, Templates::EqualOperatorPropertyTemplate);
        }
    }

    //Only if at least one field "copied"
    if (!isFirst) {
        Outdent();
        Outdent();
    }

    mPrinter.Print(";\n");
    mPrinter.Print(Templates::SimpleBlockEnclosureTemplate);

    mPrinter.Print({{"type", mClassName}}, Templates::NotEqualOperatorTemplate);
}

void ProtobufClassGenerator::printIncludes()
{
    assert(mMessage != nullptr);

    mPrinter.Print(Templates::DefaultProtobufIncludesTemplate);

    std::set<std::string> existingIncludes;
    for (int i = 0; i < mMessage->field_count(); i++) {
        printInclude(mMessage->field(i), existingIncludes);
    }
}

void ProtobufClassGenerator::printInclude(const FieldDescriptor *field, std::set<std::string> &existingIncludes)
{
    assert(field != nullptr);
    std::string newInclude;
    const char* includeTemplate;
    switch (field->type()) {
    case FieldDescriptor::TYPE_MESSAGE:
        if ( field->is_map() ) {
            newInclude = "QMap";
            assert(field->message_type() != nullptr);
            assert(field->message_type()->field_count() == 2);
            printInclude(field->message_type()->field(0), existingIncludes);
            printInclude(field->message_type()->field(1), existingIncludes);
            includeTemplate = Templates::ExternalIncludeTemplate;
        } else {
            std::string typeName = field->message_type()->name();
            utils::tolower(typeName);
            newInclude = typeName;
            includeTemplate = Templates::InternalIncludeTemplate;
        }
        break;
    case FieldDescriptor::TYPE_BYTES:
        newInclude = "QByteArray";
        includeTemplate = Templates::ExternalIncludeTemplate;
        break;
    case FieldDescriptor::TYPE_STRING:
        newInclude = "QString";
        includeTemplate = Templates::ExternalIncludeTemplate;
        break;
    case FieldDescriptor::TYPE_ENUM: {
        EnumVisibility enumVisibily = getEnumVisibility(field);
        if (enumVisibily == GLOBAL_ENUM) {
            includeTemplate = Templates::GlobalEnumIncludeTemplate;
        } else if (enumVisibily == NEIGHBOUR_ENUM){
            includeTemplate = Templates::InternalIncludeTemplate;
            std::string fullEnumName = field->enum_type()->full_name();
            std::vector<std::string> fullEnumNameParts;
            utils::split(fullEnumName, fullEnumNameParts, '.');
            std::string enumTypeOwner = fullEnumNameParts.at(fullEnumNameParts.size() - 2);
            utils::tolower(enumTypeOwner);
            newInclude = enumTypeOwner;
        } else {
            return;
        }
    }
        break;
    default:
        return;
    }

    if (existingIncludes.find(newInclude) == std::end(existingIncludes)) {
        mPrinter.Print({{"include", newInclude}}, includeTemplate);
        existingIncludes.insert(newInclude);
    }
}

void ProtobufClassGenerator::printField(const FieldDescriptor *field, const char *fieldTemplate)
{
    assert(field != nullptr);
    std::map<std::string, std::string> propertyMap;
    if (producePropertyMap(field, propertyMap)) {
        mPrinter.Print(propertyMap, fieldTemplate);
    }
}

template<typename T>
std::string ProtobufClassGenerator::getNamespacesList(const T *message, std::vector<std::string> &container)
{
    std::string result;
    utils::split(std::string(message->full_name()), container, '.');

    if (container.size() > 1) {
        //delete type name -> only namespace stays
        container.pop_back();

        for (size_t i = 0; i < container.size(); i++) {
            if (i > 0) {
                result = result.append("::");
            }
            result = result.append(container[i]);
        }
    }

    if (container.size() > 0
            && mNamespacesColonDelimited != result) {
        result = result.append("::");
    } else {
        result.clear();
    }

    return result;
}

std::string ProtobufClassGenerator::getTypeName(const FieldDescriptor *field)
{
    assert(field != nullptr);
    std::string typeName;
    std::string namespaceQtProtoDefinition("qtprotobuf::");

    std::string namespaceTypeName;
    std::vector<std::string> typeNamespace;

    if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
        const Descriptor *msg = field->message_type();
        namespaceTypeName = getNamespacesList(msg, typeNamespace);
        typeName = namespaceTypeName.append(msg->name());

        if (field->is_map()) {
            return mClassName + "::" + field->message_type()->name();
        }
        if (field->is_repeated()) {
            return namespaceTypeName.append("List");
        }
    } else if (field->type() == FieldDescriptor::TYPE_ENUM) {
        const EnumDescriptor *enumType = field->enum_type();
        namespaceTypeName = getNamespacesList(enumType, typeNamespace);
        EnumVisibility visibility = getEnumVisibility(field);
        if (visibility == LOCAL_ENUM) {
            typeName = typeName.append(enumType->name());
        } else if (visibility == GLOBAL_ENUM) {
            typeName = namespaceTypeName.append(Templates::GlobalEnumClassNameTemplate)
                    .append("::").append(enumType->name());
        } else {
            typeName = namespaceTypeName.append(enumType->name());
        }
        if (field->is_repeated()) {
            return typeName.append("List");
        }
    } else {
        auto it = Templates::TypeReflection.find(field->type());
        if (it != std::end(Templates::TypeReflection)) {
            if (field->type() != FieldDescriptor::TYPE_STRING
                    && field->type() != FieldDescriptor::TYPE_BYTES
                    && field->type() != FieldDescriptor::TYPE_BOOL
                    && field->type() != FieldDescriptor::TYPE_FLOAT
                    && field->type() != FieldDescriptor::TYPE_DOUBLE) {
                typeName = typeName.append(namespaceQtProtoDefinition.append(it->second));
            } else {
                typeName = typeName.append(it->second);
            }
            if (field->is_repeated()) {
                if (field->type() == FieldDescriptor::TYPE_FLOAT
                        || field->type() == FieldDescriptor::TYPE_DOUBLE) {
                    typeName[0] = ::toupper(typeName[0]);
                    typeName = namespaceQtProtoDefinition.append(typeName);
                }
                typeName.append("List");
            }
        }
    }

    return typeName;
}

bool ProtobufClassGenerator::producePropertyMap(const FieldDescriptor *field, PropertyMap &propertyMap)
{
    assert(field != nullptr);
    std::string typeName = getTypeName(field);

    if (typeName.size() <= 0) {
        std::cerr << "Type "
                  << field->type_name()
                  << " is not supported by Qt Generator"
                     " please look at https://doc.qt.io/qt-5/qtqml-typesystem-basictypes.html"
                     " for details" << std::endl;
        return false;
    }

    std::string typeNameLower(typeName);
    utils::tolower(typeNameLower);

    std::string capProperty = field->camelcase_name();
    capProperty[0] = ::toupper(capProperty[0]);

    propertyMap = {{"type", typeName},
                   {"type_lower", typeNameLower},
                   {"property_name", field->camelcase_name()},
                   {"property_name_cap", capProperty}};
    return true;
}

bool ProtobufClassGenerator::isListType(const FieldDescriptor *field)
{
    assert(field != nullptr);
    return field && field->is_repeated()
            && field->type() == FieldDescriptor::TYPE_MESSAGE;
}

bool ProtobufClassGenerator::isComplexType(const FieldDescriptor *field)
{
    assert(field != nullptr);
    return field->type() == FieldDescriptor::TYPE_MESSAGE
            || field->type() == FieldDescriptor::TYPE_STRING
            || field->type() == FieldDescriptor::TYPE_BYTES;
}

void ProtobufClassGenerator::printConstructor()
{
    std::string parameterList;
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        std::string fieldTypeName = getTypeName(field);
        std::string fieldName = field->name();
        fieldName[0] = ::tolower(fieldName[0]);
        switch (field->type()) {
        case FieldDescriptor::TYPE_BYTES:
        case FieldDescriptor::TYPE_STRING:
        case FieldDescriptor::TYPE_MESSAGE:
            parameterList += "const " + fieldTypeName + " &" + fieldName + " = " + fieldTypeName + "()";
            break;
        default:
            parameterList += fieldTypeName + " " + fieldName + " = " + "{}";
            break;
        }
        parameterList += ", ";
    }
    mPrinter.Print({{"classname", mClassName},
                    {"parameter_list", parameterList}}, Templates::ProtoConstructorTemplate);

    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        std::string fieldName = field->name();
        fieldName[0] = ::tolower(fieldName[0]);
        mPrinter.Print({{"property_name", fieldName}}, Templates::PropertyInitializerTemplate);
    }
    mPrinter.Print(Templates::ConstructorContentTemplate);
}

void ProtobufClassGenerator::printMaps()
{
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);

        if (field->is_map()) {
            std::string keyType = getTypeName(field->message_type()->field(0));
            std::string valueType = getTypeName(field->message_type()->field(1));
             mPrinter.Print({{"classname",field->message_type()->name()},
                             {"key", keyType},
                             {"value", valueType}}, Templates::MapTypeUsingTemplate);
        }
    }
    Outdent();
}

void ProtobufClassGenerator::printMapsMetaTypesDeclaration()
{
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        if (field->is_map()) {
             mPrinter.Print({{"classname", field->message_type()->name()},
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
        const FieldDescriptor* field = mMessage->field(i);
        const char* propertyTemplate = field->type() == FieldDescriptor::TYPE_MESSAGE ? Templates::MessagePropertyTemplate :
                                                                                        Templates::PropertyTemplate;
        printField(field, propertyTemplate);
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
        printField(mMessage->field(i), Templates::GetterTemplate);
    }
    for (int i = 0; i < mMessage->field_count(); i++) {
        auto field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE
                || field->type() == FieldDescriptor::TYPE_STRING) {
            printField(field, Templates::SetterTemplateComplexType);
        } else {
            printField(field, Templates::SetterTemplateSimpleType);
        }
    }
    Outdent();

    mPrinter.Print(Templates::SignalsBlockTemplate);

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        printField(mMessage->field(i), Templates::SignalTemplate);
    }
    Outdent();
}

void ProtobufClassGenerator::printRegisterTypes()
{
    Indent();
    mPrinter.Print(Templates::ComplexTypeRegistrationMethodTemplate);
    Outdent();
}

void ProtobufClassGenerator::printListType()
{
    mPrinter.Print({{"classname", mClassName}}, Templates::ComplexListTypeUsingTemplate);

}

void ProtobufClassGenerator::printFieldsOrderingDefinition()
{
    Indent();
    mPrinter.Print(Templates::FieldsOrderingDefinitionContainerTemplate);
    Outdent();
}

ProtobufClassGenerator::EnumVisibility ProtobufClassGenerator::getEnumVisibility(const FieldDescriptor *field)
{
    assert(field->enum_type() != nullptr);

    if (isLocalMessageEnum(field)) {
        return LOCAL_ENUM;
    }

    const EnumDescriptor *enumType = field->enum_type();
    const FileDescriptor *enumFile = field->enum_type()->file();

    for (int i = 0; i < enumFile->message_type_count(); i++) {
        const Descriptor* msg = enumFile->message_type(i);
        for (int j = 0; j < msg->enum_type_count(); j++) {
            if (enumType->full_name() == msg->enum_type(j)->full_name()) {
                return NEIGHBOUR_ENUM;
            }
        }
    }

    return GLOBAL_ENUM;
}

bool ProtobufClassGenerator::isLocalMessageEnum(const ::google::protobuf::FieldDescriptor *field)
{
    assert(field->enum_type() != nullptr);
    for (int i = 0; i < mMessage->enum_type_count(); i++) {
        const auto enumDescr = mMessage->enum_type(i);
        if (enumDescr->full_name() == field->enum_type()->full_name()) {
            return true;
        }
    }
    return false;
}

void ProtobufClassGenerator::printClassMembers()
{
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        printField(mMessage->field(i), Templates::MemberTemplate);
    }
    Outdent();
}

std::set<std::string> ProtobufClassGenerator::extractModels() const
{
    std::set<std::string> extractedModels;
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        if (field->is_repeated()
                && field->type() == FieldDescriptor::TYPE_MESSAGE) {
            std::string typeName = field->message_type()->name();
            extractedModels.insert(typeName);
        }
    }
    return std::move(extractedModels);
}

void ProtobufClassGenerator::run()
{
    printPreamble();
    printIncludes();
    printNamespaces();
    printClassDeclaration();
    printProperties();
    printPublic();
    printRegisterTypes();
    printFieldsOrderingDefinition();
    printPrivate();
    printClassMembers();
    encloseClass();
    printListType();
    encloseNamespaces();
    printMetaTypeDeclaration();
    printMapsMetaTypesDeclaration();
}
