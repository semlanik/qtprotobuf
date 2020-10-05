/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>
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

#include "generatorcommon.h"
#include "generatoroptions.h"

#include "templates.h"
#include <assert.h>

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;

std::string common::getNamespacesString(const std::vector<std::string> &namespacesList, const std::string &separator)
{
    std::string namespaces;
    for (auto namespacePart : namespacesList) {
        namespaces += namespacePart + separator;
    }

    if (!namespaces.empty()) {
        namespaces.resize(namespaces.size() - separator.size());
    }
    return namespaces;
}

std::string common::getScopeNamespacesString(std::string original, const std::string &scope)
{
    if (scope.empty()) {
        return original;
    }

    if (original == scope) {
        return "";
    }

    if (original.find(scope + "::") == 0) {
        original = original.replace(0, scope.size() + 2, ""); //Remove trailing :: as well
    }
    return original;
}

TypeMap common::produceQtTypeMap(const ::Descriptor *type, const Descriptor *scope)
{
    std::vector<std::string> namespaceList = getNamespaces(type);
    std::string namespaces = getNamespacesString(namespaceList, "::");
    std::string scopeNamespaces = getScopeNamespacesString(namespaces, getNamespacesString(getNamespaces(scope), "::"));
    std::string qmlPackage = getNamespacesString(namespaceList, ".");

    std::string name = type->name();
    std::string fullName = name;
    std::string scopeName = name;

    std::string listName = std::string("QList<") + Templates::ListSuffix + ">";
    std::string fullListName = listName;
    std::string scopeListName = listName;

    return {
        {"type", name},
        {"full_type", fullName},
        {"scope_type", scopeName},
        {"list_type", listName},
        {"full_list_type", fullListName},
        {"scope_list_type", scopeListName},
        {"namespaces", namespaces},
        {"scope_namespaces", scopeNamespaces},
        {"qml_package", qmlPackage},
        {"property_type", fullName},
        {"property_list_type", fullListName},
        {"getter_type", scopeName},
        {"setter_type", scopeName}
    };
}

TypeMap common::produceMessageTypeMap(const ::Descriptor *type, const Descriptor *scope)
{
    std::vector<std::string> namespaceList = getNamespaces(type);
    std::string namespaces = getNamespacesString(namespaceList, "::");
    std::string scopeNamespaces = getScopeNamespacesString(namespaces, getNamespacesString(getNamespaces(scope), "::"));
    std::string qmlPackage = getNamespacesString(namespaceList, ".");

    std::string name = utils::upperCaseName(type->name());
    std::string fullName = namespaces.empty() ? name : (namespaces + "::" + name);
    std::string scopeName = scopeNamespaces.empty() ? name : (scopeNamespaces + "::" + name);

    std::string listName = name + Templates::ListSuffix;
    std::string fullListName = namespaces.empty() ? listName : (namespaces + "::" + listName);
    std::string scopeListName = scopeNamespaces.empty() ? listName : (scopeNamespaces + "::" + listName);

    return {
        {"type", name},
        {"full_type", fullName},
        {"scope_type", scopeName},
        {"list_type", listName},
        {"full_list_type", fullListName},
        {"scope_list_type", scopeListName},
        {"namespaces", namespaces},
        {"scope_namespaces", scopeNamespaces},
        {"qml_package", qmlPackage},
        {"property_type", fullName},
        {"property_list_type", fullListName},
        {"getter_type", scopeName},
        {"setter_type", scopeName}
    };
}

TypeMap common::produceEnumTypeMap(const EnumDescriptor *type, const Descriptor *scope)
{
    EnumVisibility visibility = enumVisibility(type, scope);
    std::vector<std::string> namespaceList = getNamespaces(type);

    std::string name = utils::upperCaseName(type->name());
    std::string qmlPackage = getNamespacesString(namespaceList, ".");//qml package should consist only from proto spackage
    std::string enumGadget = scope != nullptr ? utils::upperCaseName(scope->name()) : "";//Not used
    if (visibility == GLOBAL_ENUM) {
        enumGadget = name + Templates::EnumClassSuffix;
        namespaceList.push_back(enumGadget);//Global enums are stored in helper Gadget
    }

    std::string namespaces = getNamespacesString(namespaceList, "::");
    std::string scopeNamespaces = getScopeNamespacesString(namespaces, getNamespacesString(getNamespaces(scope), "::"));


    std::string fullName = namespaces.empty() ? name : (namespaces + "::" + name);
    std::string scopeName = scopeNamespaces.empty() ? name : (scopeNamespaces + "::" + name);

    std::string listName = name + Templates::ListSuffix;
    std::string fullListName = namespaces.empty() ? listName : (namespaces + "::" + listName);
    std::string scopeListName = scopeNamespaces.empty() ? listName: (scopeNamespaces + "::" + listName);

    std::string propertyType = fullName;
    if (visibility == LOCAL_ENUM) {
        //Note: For local enum classes it's impossible to use class name space in Q_PROPERTY
        //declaration. So please avoid addition of namespaces in line bellow
        propertyType = name;
    }

    return {
        {"type", name},
        {"full_type", fullName},
        {"scope_type", scopeName},
        {"list_type", listName},
        {"full_list_type", fullListName},
        {"scope_list_type", scopeListName},
        {"namespaces", namespaces},
        {"scope_namespaces", scopeNamespaces},
        {"qml_package", qmlPackage},
        {"property_type", propertyType},
        {"property_list_type", fullListName},
        {"getter_type", scopeName},
        {"setter_type", scopeName},
        {"enum_gadget", enumGadget}
    };
}

TypeMap common::produceSimpleTypeMap(FieldDescriptor::Type type)
{
    std::string namespaces;
    if (type != FieldDescriptor::TYPE_STRING
            && type != FieldDescriptor::TYPE_BYTES
            && type != FieldDescriptor::TYPE_BOOL
            && type != FieldDescriptor::TYPE_FLOAT
            && type != FieldDescriptor::TYPE_DOUBLE) {
        namespaces = Templates::QtProtobufNamespace;
    }

    std::string name;
    std::string qmlPackage = Templates::QtProtobufNamespace;

    auto it = Templates::TypeReflection.find(type);
    if (it != std::end(Templates::TypeReflection)) {
        name = it->second;
    } else {
        assert(name.empty());
    }

    std::string fullName = namespaces.empty() ? name : (namespaces + "::" + name);
    std::string scopeName = fullName;

    std::string listName = name + "List";
    if (type == FieldDescriptor::TYPE_FLOAT
            || type == FieldDescriptor::TYPE_DOUBLE
            || type == FieldDescriptor::TYPE_BOOL) {
        listName = utils::upperCaseName(name) + "List";
    }

    std::string fullListName = listName;
    if (type != FieldDescriptor::TYPE_STRING
            && type != FieldDescriptor::TYPE_BYTES) {
        fullListName = std::string(Templates::QtProtobufNamespace) + "::" + listName;
    }

    std::string scopeListName = fullListName;

    std::string qmlAliasType = fullName;
    switch (type) {
    case FieldDescriptor::TYPE_INT32:
    case FieldDescriptor::TYPE_SFIXED32:
        qmlAliasType = "int";
        break;
    case FieldDescriptor::TYPE_FIXED32:
        qmlAliasType = "unsigned int";
        break;
    default:
        //Do nothing
        break;
    }

    std::string getterType = fullName;
    if (type == FieldDescriptor::TYPE_INT32
            || type == FieldDescriptor::TYPE_FIXED32
            || type == FieldDescriptor::TYPE_SFIXED32
            || type == FieldDescriptor::TYPE_INT64
            || type == FieldDescriptor::TYPE_FIXED64
            || type == FieldDescriptor::TYPE_SFIXED64) {
        getterType = "const " + getterType;
    }

    return {{"type", name},
        {"full_type", fullName},
        {"scope_type", scopeName},
        {"list_type", listName},
        {"full_list_type", fullListName},
        {"scope_list_type", scopeListName},
        {"namespaces", namespaces},
        {"scope_namespaces", namespaces},
        {"qml_package", qmlPackage},
        {"property_type", fullName},
        {"qml_alias_type", qmlAliasType},
        {"property_list_type", fullListName},
        {"getter_type", getterType},
        {"setter_type", fullName}
    };
}

bool common::isQtType(const FieldDescriptor *field)
{
    auto namespaces = getNamespaces(field->message_type());
    return namespaces.size() == 1 && namespaces[0] == "QtProtobuf"
            && field->file()->package() != "QtProtobuf"; //Used for qttypes library to avoid types conversion inside library
}

bool common::isPureMessage(const ::google::protobuf::FieldDescriptor *field)
{
    return field->type() == FieldDescriptor::TYPE_MESSAGE && !field->is_map() && !field->is_repeated() && !common::isQtType(field);
}

TypeMap common::produceTypeMap(const FieldDescriptor *field, const Descriptor *scope)
{
    TypeMap typeMap;

    assert(field != nullptr);
    std::string namespaceTypeName;
    std::vector<std::string> typeNamespace;

    switch (field->type()) {
    case FieldDescriptor::TYPE_MESSAGE: {
        if (isQtType(field)) {
            typeMap = produceQtTypeMap(field->message_type(), nullptr);
        } else {
            typeMap = produceMessageTypeMap(field->message_type(), scope);
        }
    }
        break;
    case FieldDescriptor::TYPE_ENUM:
        typeMap = produceEnumTypeMap(field->enum_type(), scope);
        break;
    default:
        typeMap = produceSimpleTypeMap(field->type());
        break;
    }

    return typeMap;
}

PropertyMap common::producePropertyMap(const FieldDescriptor *field, const Descriptor *scope)
{
    assert(field != nullptr);

    PropertyMap propertyMap = produceTypeMap(field, scope);

    std::string scriptable = "true";
    if (!field->is_map() && !field->is_repeated() && (field->type() == FieldDescriptor::TYPE_INT64
                                                      || field->type() == FieldDescriptor::TYPE_SINT64
                                                      || field->type() == FieldDescriptor::TYPE_FIXED64
                                                      || field->type() == FieldDescriptor::TYPE_SFIXED64)) {
        scriptable = "false";
    }

    std::string propertyName =  qualifiedName(utils::lowerCaseName(field->camelcase_name()));
    std::string propertyNameCap = utils::upperCaseName(propertyName);

    propertyMap["property_name"] = propertyName;
    propertyMap["property_name_cap"] = propertyNameCap;
    propertyMap["scriptable"] = scriptable;

    propertyMap["key_type"] = "";
    propertyMap["value_type"] = "";
    propertyMap["classname"] = scope != nullptr ? utils::upperCaseName(scope->name()) : "";

    if (field->is_map()) {
        const Descriptor *type = field->message_type();
        auto keyMap = common::producePropertyMap(type->field(0), scope);
        auto valueMap = common::producePropertyMap(type->field(1), scope);
        propertyMap["key_type"] = keyMap["scope_type"];
        propertyMap["value_type"] = valueMap["scope_type"];
        propertyMap["value_list_type"] = valueMap["scope_list_type"];
    } else if (field->is_repeated()) {
        propertyMap["getter_type"] = propertyMap["scope_list_type"];
        propertyMap["setter_type"] = propertyMap["scope_list_type"];
    }

    return propertyMap;
}

std::string common::qualifiedName(const std::string &name)
{
    std::string fieldName(name);
    const std::vector<std::string> &searchExeptions = Templates::ListOfQmlExeptions;

    auto searchResult = std::find(searchExeptions.begin(), searchExeptions.end(), fieldName);
    if (searchResult != searchExeptions.end()) {
        return fieldName.append(Templates::ProtoSufix);
    }
    return fieldName;
}

bool common::isLocalEnum(const EnumDescriptor *type, const google::protobuf::Descriptor *scope)
{
    if (scope == nullptr) {
        return false;
    }

    assert(type != nullptr);
    for (int i = 0; i < scope->enum_type_count(); i++) {
        const auto scopeEnum = scope->enum_type(i);
        if (scopeEnum && scopeEnum->full_name() == type->full_name()) {
            return true;
        }
    }
    return false;
}

common::EnumVisibility common::enumVisibility(const EnumDescriptor *type, const Descriptor *scope)
{
    assert(type != nullptr);

    if (isLocalEnum(type, scope)) {
        return LOCAL_ENUM;
    }

    const FileDescriptor *typeFile = type->file();

    for (int i = 0; i < typeFile->message_type_count(); i++) {
        const Descriptor *msg = typeFile->message_type(i);
        for (int j = 0; j < msg->enum_type_count(); j++) {
            if (type->full_name() == msg->enum_type(j)->full_name()) {
                return NEIGHBOR_ENUM;
            }
        }
    }

    return GLOBAL_ENUM;
}

bool common::hasQmlAlias(const ::google::protobuf::FieldDescriptor *field)
{
    return !field->is_map() && !field->is_repeated()
            && (field->type() == FieldDescriptor::TYPE_INT32
                || field->type() == FieldDescriptor::TYPE_SFIXED32
                || field->type() == FieldDescriptor::TYPE_FIXED32)
            && GeneratorOptions::instance().hasQml();
}

MethodMap common::produceMethodMap(const MethodDescriptor *method, const std::string& scope)
{
    std::string inputTypeName = method->input_type()->full_name();
    std::string outputTypeName = method->output_type()->full_name();
    std::string methodName = method->name();
    std::string methodNameUpper = method->name();
    methodNameUpper[0] =  static_cast<char>(::toupper(methodNameUpper[0]));
    utils::replace(inputTypeName, ".", "::");
    utils::replace(outputTypeName, ".", "::");
    return {{"classname", scope},
                  {"return_type", outputTypeName},
                  {"method_name", methodName},
                  {"method_name_upper", methodNameUpper},
                  {"param_type", inputTypeName},
                  {"param_name", "arg"},
                  {"return_name", "ret"}
                 };
}

void common::iterateMessages(const ::google::protobuf::FileDescriptor *file, std::function<void(const ::google::protobuf::Descriptor *)> callback)
{
    for (int i = 0; i < file->message_type_count(); i++) {
        callback(file->message_type(i));
    }
}

void common::iterateNestedMessages(const ::google::protobuf::Descriptor *message, std::function<void(const ::google::protobuf::Descriptor *)> callback)
{
    for (int i = 0; i < message->nested_type_count(); i++) {
        auto nestedMessage = message->nested_type(i);
        if (message->field_count() <= 0) {
            callback(nestedMessage);
        }
        for (int j = 0; j < message->field_count(); j++) {
            auto field = message->field(j);
            if (!field->is_map() && field->message_type() == nestedMessage) { //Probably there is more correct way to detect map in nested messages.                                                                              //TODO: Have idea to make maps nested classes instead of typedefs.
                callback(nestedMessage);
            }
        }
    }
}

bool common::hasNestedMessages(const ::google::protobuf::Descriptor *message)
{
    if (message->nested_type_count() > 0 && message->field_count() <= 0) {
        return true;
    }

    for (int i = 0; i < message->nested_type_count(); i++) {
        auto nestedMessage = message->nested_type(i);
        for (int j = 0; j < message->field_count(); j++) {
            auto field = message->field(j);
            if (!field->is_map() && field->message_type() == nestedMessage) { //Probably there is more correct way to detect map in nested messages.                                                                              //TODO: Have idea to make maps nested classes instead of typedefs.
                return true;
            }
        }
    }

    return false;
}

bool common::isNested(const ::google::protobuf::Descriptor *message)
{
    return message->containing_type() != nullptr;
}

const ::google::protobuf::Descriptor *common::findHighestMessage(const ::google::protobuf::Descriptor *message)
{
    const ::google::protobuf::Descriptor *highestMessage = message;
    while (highestMessage->containing_type() != nullptr) {
        highestMessage = highestMessage->containing_type();
    }
    return highestMessage;
}
