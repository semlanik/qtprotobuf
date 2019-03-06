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

#include "classgeneratorbase.h"

#include "templates.h"
#include "utils.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <set>

using namespace qtprotobuf;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

namespace {
const std::string VariantList("QVariantList");
}

ClassGeneratorBase::ClassGeneratorBase(std::string mClassName, std::unique_ptr<::google::protobuf::io::ZeroCopyOutputStream> out) : mOutput(std::move(out))
  , mPrinter(mOutput.get(), '$')
  , mClassName(std::move(mClassName))
  , mNamespaceCount(0)
{

}

bool ClassGeneratorBase::producePropertyMap(const FieldDescriptor *field, PropertyMap &propertyMap)
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
    std::transform(std::begin(typeName), std::end(typeName), std::begin(typeNameLower), ::tolower);

    std::string capProperty = field->camelcase_name();
    capProperty[0] = ::toupper(capProperty[0]);

    propertyMap = {{"type", typeName},
                   {"type_lower", typeNameLower},
                   {"property_name", field->camelcase_name()},
                   {"property_name_cap", capProperty}};
    return true;
}

void ClassGeneratorBase::printPreamble()
{
    mPrinter.Print(PreambleTemplate);
}

void ClassGeneratorBase::printIncludes(const Descriptor *message, std::set<std::string> listModel)
{
    assert(message != nullptr);
    PropertyMap properties;
    std::set<std::string> existingIncludes;
    std::string newinclude;
    const char* includeTemplate;
    for (int i = 0; i < message->field_count(); i++) {
        const FieldDescriptor* field = message->field(i);
        if (producePropertyMap(field, properties)) {
            if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                newinclude = properties["type_lower"];
                includeTemplate = InternalIncludeTemplate;
            } else if (field->type() == FieldDescriptor::TYPE_STRING) {
                includeTemplate = ExternalIncludeTemplate;
            } else {
                continue;
            }

            if (!field->is_repeated()) {
                if (existingIncludes.find(newinclude) == std::end(existingIncludes)) {
                    mPrinter.Print(properties, includeTemplate);
                    existingIncludes.insert(newinclude);
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
        std::transform(std::begin(modelTypeNameLower), std::end(modelTypeNameLower), std::begin(modelTypeNameLower), ::tolower);
        mPrinter.Print({{"type_lower", modelTypeNameLower}}, InternalIncludeTemplate);
    }
}

void ClassGeneratorBase::printNamespaces(const std::string &package)
{
    std::vector<std::string> namespaces;
    utils::split(package, namespaces, '.');
    mNamespaceCount = namespaces.size();
    for (auto ns: namespaces) {
        mPrinter.Print({{"namespace", ns}}, NamespaceTemplate);
    }
}

void ClassGeneratorBase::printClass()
{
    mPrinter.Print({{"classname", mClassName}}, ClassDefinitionTemplate);
}

void ClassGeneratorBase::printProperties(const Descriptor *message)
{
    assert(message != nullptr);
    //private section
    Indent();
    for (int i = 0; i < message->field_count(); i++) {
        const FieldDescriptor* field = message->field(i);
        if (!isListType(field)) {
            const char* propertyTemplate = field->type() == FieldDescriptor::TYPE_MESSAGE ? MessagePropertyTemplate :
                                                                                            PropertyTemplate;
            printField(field, propertyTemplate);
        }
    }
    for (int i = 0; i < message->field_count(); i++) {
        printField(message->field(i), MemberTemplate);
    }
    Outdent();

    printQEnums(message);

    //public section
    printPublic();

    //Body
    Indent();
    printConstructor();
    printCopyFunctionality(message);
    printMoveSemantic(message);
    printComparisonOperators(message);

    for (int i = 0; i < message->field_count(); i++) {
        printField(message->field(i), GetterTemplate);
    }
    for (int i = 0; i < message->field_count(); i++) {
        auto field = message->field(i);
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
    for (int i = 0; i < message->field_count(); i++) {
        printField(message->field(i), SignalTemplate);
    }
    Outdent();
}

void ClassGeneratorBase::printField(const FieldDescriptor *field, const char *fieldTemplate)
{
    assert(field != nullptr);
    std::map<std::string, std::string> propertyMap;
    if (producePropertyMap(field, propertyMap)) {
        mPrinter.Print(propertyMap, fieldTemplate);
    }
}

void ClassGeneratorBase::encloseClass()
{
    mPrinter.Print(SemicolonBlockEnclosureTemplate);
}

void ClassGeneratorBase::enclose()
{
    while (mNamespaceCount > 0) {
        mPrinter.Print(SimpleBlockEnclosureTemplate);
        --mNamespaceCount;
    }
}

std::string ClassGeneratorBase::getTypeName(const FieldDescriptor *field)
{
    assert(field != nullptr);
    if (field->is_repeated()) {
        return VariantList;
    }

    std::string typeName;
    if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
        typeName = field->message_type()->name();
    } else if (field->type() == FieldDescriptor::TYPE_ENUM) {
        typeName = field->enum_type()->name();
    } else {
        auto it = TypeReflection.find(field->type());
        if (it != std::end(TypeReflection)) {
            typeName = it->second;
        }
    }

    return typeName;
}

void ClassGeneratorBase::printCopyFunctionality(const Descriptor *message)
{
    assert(message != nullptr);
    mPrinter.Print({{"classname", mClassName}},
                   CopyConstructorTemplate);

    Indent();
    for (int i = 0; i < message->field_count(); i++) {
        printField(message->field(i), CopyFieldTemplate);
    }
    Outdent();

    mPrinter.Print(SimpleBlockEnclosureTemplate);
    mPrinter.Print({{"classname", mClassName}},
                   AssignmentOperatorTemplate);

    Indent();
    for (int i = 0; i < message->field_count(); i++) {
        printField(message->field(i), CopyFieldTemplate);
    }
    mPrinter.Print(AssignmentOperatorReturnTemplate);
    Outdent();

    mPrinter.Print(SimpleBlockEnclosureTemplate);

}

bool ClassGeneratorBase::isListType(const FieldDescriptor *field)
{
    assert(field != nullptr);
    return field && field->is_repeated()
            && field->type() == FieldDescriptor::TYPE_MESSAGE;
}

bool ClassGeneratorBase::isComplexType(const FieldDescriptor *field)
{
    assert(field != nullptr);
    return field->type() == FieldDescriptor::TYPE_MESSAGE
            || field->type() == FieldDescriptor::TYPE_STRING
            || field->type() == FieldDescriptor::TYPE_BYTES;
}

void ClassGeneratorBase::printMoveSemantic(const Descriptor *message)
{
    assert(message != nullptr);
    mPrinter.Print({{"classname", mClassName}},
                   MoveConstructorTemplate);

    Indent();
    for (int i = 0; i < message->field_count(); i++) {
        const FieldDescriptor* field = message->field(i);
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
    for (int i = 0; i < message->field_count(); i++) {
        const FieldDescriptor* field = message->field(i);
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

void ClassGeneratorBase::printConstructor()
{
    mPrinter.Print({{"classname", mClassName}},
                   ConstructorTemplate);

    //FIXME: Explicit default values are not allowed in proto3 seems
    //this function is useless
    //    for (int i = 0; i < mMessage->field_count(); i++) {
    //        const FieldDescriptor* field = mMessage->field(i);
    //        std::string defaultValue;
    //        if (field->has_default_value()) {
    //            switch (field->type()) {
    //            case FieldDescriptor::TYPE_DOUBLE:
    //                defaultValue = std::to_string(field->default_value_double());
    //                break;
    //            case FieldDescriptor::TYPE_FLOAT:
    //                defaultValue = std::to_string(field->default_value_float());
    //                break;
    //            case FieldDescriptor::TYPE_INT32:
    //            case FieldDescriptor::TYPE_UINT32:
    //            case FieldDescriptor::TYPE_SFIXED32:
    //            case FieldDescriptor::TYPE_SINT32:
    //                defaultValue = std::to_string(field->default_value_int32());
    //                break;
    //            case FieldDescriptor::TYPE_BOOL:
    //                defaultValue = field->default_value_bool() ? "true" : "false";
    //                break;
    //            case FieldDescriptor::TYPE_STRING:
    //                defaultValue = field->default_value_string();
    //                break;
    //            case FieldDescriptor::TYPE_ENUM:
    //                defaultValue = field->default_value_enum()->name();
    //                break;
    //            default:
    //                std::cerr << "Default value substitution"
    //                             " is not supported for type"
    //                          << field->type_name() << std::endl;
    //                break;
    //            }
    //            if (defaultValue.size() > 0) {
    //                mPrinter.Print({{"property_name", field->camelcase_name()},
    //                                {"default_value", defaultValue}},
    //                               "    , $m_property_name$($default_value$)\n");
    //            }
    //        }
    //    }
    mPrinter.Print(EmptyBlockTemplate);
}

void ClassGeneratorBase::printPublic()
{
    mPrinter.Print(PublicBlockTemplate);
}

void ClassGeneratorBase::printComparisonOperators(const Descriptor *message)
{
    assert(message != nullptr);
    bool isFirst = true;
    PropertyMap properties;
    mPrinter.Print({{"type", mClassName}}, EqualOperatorTemplate);
    for (int i = 0; i < message->field_count(); i++) {
        const FieldDescriptor* field = message->field(i);
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

void ClassGeneratorBase::printMetaTypeDeclaration(const std::string &package)
{
    std::string namespaces = package;
    utils::replace(namespaces, std::string("."), std::string("::"));
    mPrinter.Print({{"type", mClassName},
                    {"namespaces", namespaces}}, DeclareMetaTypeTemplate);
}

