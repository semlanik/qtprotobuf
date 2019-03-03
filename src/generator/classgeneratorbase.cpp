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

ClassGeneratorBase::ClassGeneratorBase(std::string mClassName, std::unique_ptr<::google::protobuf::io::ZeroCopyOutputStream> out) : mOutput(std::move(out))
  , mPrinter(mOutput.get(), '$')
  , mClassName(std::move(mClassName))
  , mNamespaceCount(0)
{

}

bool ClassGeneratorBase::producePropertyMap(const FieldDescriptor *field, PropertyMap &propertyMap)
{
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

void ClassGeneratorBase::printIncludes(const Descriptor *message)
{
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
            }
        }
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
    mPrinter.Print({{"classname", mClassName}}, StartTemplate);
}

void ClassGeneratorBase::printProperties(const Descriptor *message)
{
    //private section
    for (int i = 0; i < message->field_count(); i++) {
        printField(message->field(i), PropertyTemplate);
    }
    for (int i = 0; i < message->field_count(); i++) {
        printField(message->field(i), MemberTemplate);
    }
    printQEnums(message);

    //public section
    printPublic();
    printConstructor();
    printEqualOperator(message);
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
    mPrinter.Print("\nsignals:\n");
    for (int i = 0; i < message->field_count(); i++) {
        printField(message->field(i), SignalTemplate);
    }
}

void ClassGeneratorBase::printField(const FieldDescriptor *field, const char *fieldTemplate)
{
    std::map<std::string, std::string> propertyMap;
    if (producePropertyMap(field, propertyMap)) {
        mPrinter.Print(propertyMap, fieldTemplate);
    }
}

void ClassGeneratorBase::enclose()
{
    mPrinter.Print("};\n");
    while (mNamespaceCount > 0) {
        mPrinter.Print(SimpleBlockEnclosureTemplate);
        --mNamespaceCount;
    }
}

std::string ClassGeneratorBase::getTypeName(const FieldDescriptor *field)
{
    std::string typeName;
    if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
        typeName = field->message_type()->name();
        if (field->is_repeated()) {
            typeName = typeName.append("Model");
        }
    } else if (field->type() == FieldDescriptor::TYPE_ENUM) {
        if (field->is_repeated()) {
            typeName = std::string("QVariantList");
        } else {
            typeName = field->enum_type()->name();
        }
    } else {
        auto it = TypeReflection.find(field->type());
        if (it != std::end(TypeReflection)) {
            if (field->is_repeated()) {
                typeName = std::string("QVariantList");
            } else {
                typeName = it->second;
            }
        }
    }


    return typeName;
}

void ClassGeneratorBase::printConstructor()
{
    mPrinter.Print({{"classname", mClassName}},
                   "    $classname$(QObject *parent = nullptr) : QObject(parent)\n");

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
    mPrinter.Print("    {}\n\n");
}

void ClassGeneratorBase::printPublic()
{
    mPrinter.Print("\npublic:\n");
}

void ClassGeneratorBase::printEqualOperator(const Descriptor *message)
{
    bool isFirst = true;
    PropertyMap properties;
    mPrinter.Print({{"type", message->name()}}, EqualOperatorTemplate);
    for (int i = 1; i < message->field_count(); i++) {
        const FieldDescriptor* field = message->field(i);
        if (producePropertyMap(field, properties)) {
            if (!isFirst) {
                mPrinter.Print("\n            && ");
            }
            isFirst = false;
            mPrinter.Print(properties, EqualOperatorPropertyTemplate);
        }
    }
    mPrinter.Print(";\n    ");
    mPrinter.Print(SimpleBlockEnclosureTemplate);
}
