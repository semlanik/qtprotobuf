/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
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
                   CopyConstructorTemplate);

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        printField(mMessage->field(i), CopyFieldTemplate);
    }
    Outdent();

    mPrinter.Print(SimpleBlockEnclosureTemplate);
    mPrinter.Print({{"classname", mClassName}},
                   AssignmentOperatorTemplate);

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        printField(mMessage->field(i), CopyFieldTemplate);
    }
    mPrinter.Print(AssignmentOperatorReturnTemplate);
    Outdent();

    mPrinter.Print(SimpleBlockEnclosureTemplate);

}

void ProtobufClassGenerator::printMoveSemantic()
{
    assert(mMessage != nullptr);
    mPrinter.Print({{"classname", mClassName}},
                   MoveConstructorTemplate);

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        if (isComplexType(field) || field->is_repeated()) {
            printField(field, MoveComplexFieldTemplate);
        } else {
            printField(field, MoveFieldTemplate);
        }
    }
    Outdent();

    mPrinter.Print(SimpleBlockEnclosureTemplate);
    mPrinter.Print({{"classname", mClassName}},
                   MoveAssignmentOperatorTemplate);

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        if (isComplexType(field) || field->is_repeated()) {
            printField(field, MoveComplexFieldTemplate);
        } else {
            printField(field, MoveFieldTemplate);
        }
    }
    mPrinter.Print(AssignmentOperatorReturnTemplate);
    Outdent();

    mPrinter.Print(SimpleBlockEnclosureTemplate);
}

void ProtobufClassGenerator::printComparisonOperators()
{
    assert(mMessage != nullptr);
    bool isFirst = true;
    PropertyMap properties;
    mPrinter.Print({{"type", mClassName}}, EqualOperatorTemplate);
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
            mPrinter.Print(properties, EqualOperatorPropertyTemplate);
        }
    }

    //Only if at least one field "copied"
    if (!isFirst) {
        Outdent();
        Outdent();
    }

    mPrinter.Print(";\n");
    mPrinter.Print(SimpleBlockEnclosureTemplate);

    mPrinter.Print({{"type", mClassName}}, NotEqualOperatorTemplate);
}

void ProtobufClassGenerator::printIncludes(std::set<std::string> listModel)
{
    assert(mMessage != nullptr);

    mPrinter.Print(DefaultProtobufIncludesTemplate);

    PropertyMap properties;
    std::set<std::string> existingIncludes;
    std::string newInclude;
    const char* includeTemplate;
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        if (producePropertyMap(field, properties)) {
            if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                newInclude = properties["type_lower"];
                includeTemplate = InternalIncludeTemplate;
            } else if (field->type() == FieldDescriptor::TYPE_STRING) {
                includeTemplate = ExternalIncludeTemplate;
            } else {
                continue;
            }

            if (!field->is_repeated()) {
                if (existingIncludes.find(newInclude) == std::end(existingIncludes)) {
                    mPrinter.Print(properties, includeTemplate);
                    existingIncludes.insert(newInclude);
                }
            } else {
                std::string stringInclude = properties["type"];
                if (stringInclude == VariantList
                        && existingIncludes.find(stringInclude) == std::end(existingIncludes)) {
                    mPrinter.Print(properties, ExternalIncludeTemplate);
                    existingIncludes.insert(stringInclude);
                }
            }
        }
    }

    // Print List model class name
    if (listModel.size() > 0) {
        mPrinter.Print(ListModelsIncludeTemplate);
    }

    for(auto modelTypeName : listModel) {
        std::string modelTypeNameLower(modelTypeName);
        utils::tolower(modelTypeNameLower);
        mPrinter.Print({{"type_lower", modelTypeNameLower}}, InternalIncludeTemplate);
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

std::string ProtobufClassGenerator::getTypeName(const FieldDescriptor *field)
{
    assert(field != nullptr);
    std::string typeName;
    if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
        if (field->is_repeated()) {
            return VariantList;
        }
        typeName = field->message_type()->name();
    } else if (field->type() == FieldDescriptor::TYPE_ENUM) {
        if (field->is_repeated()) {
            return VariantList;
        }
        typeName = field->enum_type()->name();
    } else {
        auto it = TypeReflection.find(field->type());
        if (it != std::end(TypeReflection)) {
            typeName = it->second;
            if (field->is_repeated()) {
                std::string namespaceDefinition("qtprotobuf::");
                typeName[0] = ::toupper(typeName[0]);
                typeName.append("List");
                if (typeName != "QStringList"
                        && typeName != "QByteArrayList") {
                    typeName = namespaceDefinition.append(typeName);
                }
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


void ProtobufClassGenerator::printProperties()
{
    assert(mMessage != nullptr);
    //private section
    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        const FieldDescriptor* field = mMessage->field(i);
        if (!isListType(field)) {
            const char* propertyTemplate = field->type() == FieldDescriptor::TYPE_MESSAGE ? MessagePropertyTemplate :
                                                                                            PropertyTemplate;
            printField(field, propertyTemplate);
        }
    }
    for (int i = 0; i < mMessage->field_count(); i++) {
        printField(mMessage->field(i), MemberTemplate);
    }
    Outdent();

    printQEnums(mMessage);

    //public section
    printPublic();

    //Body
    Indent();
    printConstructor();
    printCopyFunctionality();
    printMoveSemantic();
    printComparisonOperators();

    for (int i = 0; i < mMessage->field_count(); i++) {
        printField(mMessage->field(i), GetterTemplate);
    }
    for (int i = 0; i < mMessage->field_count(); i++) {
        auto field = mMessage->field(i);
        if (field->type() == FieldDescriptor::TYPE_MESSAGE
                || field->type() == FieldDescriptor::TYPE_STRING) {
            printField(field, SetterTemplateComplexType);
        } else {
            printField(field, SetterTemplateSimpleType);
        }
    }
    Outdent();

    mPrinter.Print(SignalsBlockTemplate);

    Indent();
    for (int i = 0; i < mMessage->field_count(); i++) {
        printField(mMessage->field(i), SignalTemplate);
    }
    Outdent();
}

void ProtobufClassGenerator::printFieldsOrderingDefinition()
{
    Indent();
    mPrinter.Print(FieldsOrderingDefinitionContainerTemplate);
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
    printIncludes(extractModels());
    printNamespaces();
    printClassDeclaration();
    printProperties();
    printPublic();
    printFieldsOrderingDefinition();
    encloseClass();
    encloseNamespaces(mNamespaces.size());
    printMetaTypeDeclaration();
}
