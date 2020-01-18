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

#include "classgeneratorbase.h"

#include "templates.h"
#include "generatoroptions.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <set>

using namespace ::QtProtobuf::generator;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::google::protobuf::compiler;

ClassGeneratorBase::ClassGeneratorBase(const std::string &fullClassName, const std::shared_ptr<::google::protobuf::io::Printer> &printer) :
    mPrinter(printer)
{
    utils::split(fullClassName, mNamespaces, '.');
    assert(mNamespaces.size() > 0);
    mClassName = utils::upperCaseName(mNamespaces.back());
    mNamespaces.erase(mNamespaces.end() - 1);
    for (size_t i = 0; i < mNamespaces.size(); i++) {
        if (i > 0) {
            mNamespacesColonDelimited = mNamespacesColonDelimited.append("::");
        }
        mNamespacesColonDelimited = mNamespacesColonDelimited.append(mNamespaces[i]);
    }
}

ClassGeneratorBase::ClassGeneratorBase(const std::string &fullClassName, const std::shared_ptr<::google::protobuf::io::ZeroCopyOutputStream> &out) :
  ClassGeneratorBase(fullClassName, std::shared_ptr<::google::protobuf::io::Printer>(new ::google::protobuf::io::Printer(out.get(), '$')))
{
    mOutput = out;
}


void ClassGeneratorBase::printDisclaimer()
{
    mPrinter->Print(Templates::DisclaimerTemplate);
}

void ClassGeneratorBase::printPreamble()
{
    mPrinter->Print(Templates::PreambleTemplate);
}

void ClassGeneratorBase::printNamespaces()
{
    printNamespaces(mNamespaces);
}

void ClassGeneratorBase::printNamespaces(const std::vector<std::string> &namespaces)
{
    for (auto ns: namespaces) {
        mPrinter->Print({{"namespace", ns}}, Templates::NamespaceTemplate);
    }
}

void ClassGeneratorBase::printClassDeclaration()
{
    mPrinter->Print({{"classname", mClassName}}, Templates::ProtoClassDefinitionTemplate);
}

void ClassGeneratorBase::encloseClass()
{
    mPrinter->Print(Templates::SemicolonBlockEnclosureTemplate);
}

void ClassGeneratorBase::encloseNamespaces(int count)
{
    for (int i = 0; i < count; i++) {
        mPrinter->Print(Templates::SimpleBlockEnclosureTemplate);
    }
}

void ClassGeneratorBase::encloseNamespaces()
{
    encloseNamespaces(mNamespaces.size());
}

void ClassGeneratorBase::printPublic()
{
    mPrinter->Print(Templates::PublicBlockTemplate);
}

void ClassGeneratorBase::printPrivate()
{
    mPrinter->Print(Templates::PrivateBlockTemplate);
}

void ClassGeneratorBase::printMetaTypeDeclaration()
{
    mPrinter->Print({{"classname", mClassName}, {"namespaces", mNamespacesColonDelimited}},
                   Templates::DeclareMetaTypeTemplate);
    mPrinter->Print({{"classname", mClassName}, {"namespaces", mNamespacesColonDelimited}},
                   Templates::DeclareComplexListTypeTemplate);
    if (GeneratorOptions::instance().hasQml()) {
        mPrinter->Print({{"classname", mClassName}, {"namespaces", mNamespacesColonDelimited}},
                       Templates::DeclareComplexQmlListTypeTemplate);
    }
}

bool ClassGeneratorBase::isLocalMessageEnum(const google::protobuf::Descriptor *message,
                                            const ::google::protobuf::FieldDescriptor *field)
{
    assert(field->enum_type() != nullptr);
    for (int i = 0; i < message->enum_type_count(); i++) {
        const auto enumDescr = message->enum_type(i);
        if (enumDescr && enumDescr->full_name() == field->enum_type()->full_name()) {
            return true;
        }
    }
    return false;
}

std::string ClassGeneratorBase::getTypeName(const FieldDescriptor *field, const Descriptor *messageFor)
{
    assert(field != nullptr);
    std::string typeName;
    std::string namespaceQtProtoDefinition("QtProtobuf::");

    std::string namespaceTypeName;
    std::vector<std::string> typeNamespace;

    if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
        const Descriptor *msg = field->message_type();
        namespaceTypeName = getNamespacesList(msg, typeNamespace, mNamespacesColonDelimited);
        typeName = namespaceTypeName.append(utils::upperCaseName(msg->name()));

        if (field->is_map()) {
            return mClassName + "::" + utils::upperCaseName(msg->name());
        }
        if (field->is_repeated()) {
            return namespaceTypeName.append(Templates::ListSuffix);
        }
    } else if (field->type() == FieldDescriptor::TYPE_ENUM) {
        const EnumDescriptor *enumType = field->enum_type();
        namespaceTypeName = getNamespacesList(enumType, typeNamespace, mNamespacesColonDelimited);
        EnumVisibility visibility = getEnumVisibility(field, messageFor);
        if (visibility == LOCAL_ENUM) {
            if (field->is_repeated()) {
                typeName = typeName.append(mClassName + "::" + enumType->name());
            } else {
                //Note: For local enum classes it's impossible to use class name space in Q_PROPERTY
                //declaration. So please avoid addition of mClassName in line bellow
                typeName = typeName.append(enumType->name());
            }
        } else if (visibility == GLOBAL_ENUM) {
            namespaceTypeName = getNamespacesList(enumType, typeNamespace, "");
            typeName = namespaceTypeName.append(enumType->name() + Templates::EnumClassSuffix)
                    .append("::").append(enumType->name());
        } else {
            typeName = namespaceTypeName.append(enumType->name());
        }
        if (field->is_repeated()) {
            return typeName.append(Templates::ListSuffix);
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
                    typeName[0] =  static_cast<char>(::toupper(typeName[0]));
                    typeName = namespaceQtProtoDefinition.append(typeName);
                }
                typeName.append("List");
            }
        }
    }

    return typeName;
}

std::string ClassGeneratorBase::getQmlAliasTypeName(const ::google::protobuf::FieldDescriptor *field, const ::google::protobuf::Descriptor *messageFor)
{
    if (!field->is_repeated() && !field->is_map()) {
        switch(field->type()) {
        case FieldDescriptor::TYPE_INT32:
        case FieldDescriptor::TYPE_SFIXED32:
            return "int";
        case FieldDescriptor::TYPE_FIXED32:
            return "unsigned int";
        default:
            break;//Do nothing
        }
    }
    return getTypeName(field, messageFor);
}

template<typename T>
std::string ClassGeneratorBase::getNamespacesList(const T *message, std::vector<std::string> &container, const std::string &localNamespace)
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

    if (!container.empty()
            && localNamespace != result) {
        result = result.append("::");
    } else {
        result.clear();
    }

    return result;
}

ClassGeneratorBase::EnumVisibility ClassGeneratorBase::getEnumVisibility(const FieldDescriptor *field, const Descriptor *messageFor)
{
    assert(field->enum_type() != nullptr);

    if (isLocalMessageEnum(messageFor, field)) {
        return LOCAL_ENUM;
    }

    const EnumDescriptor *enumType = field->enum_type();
    const FileDescriptor *enumFile = field->enum_type()->file();

    for (int i = 0; i < enumFile->message_type_count(); i++) {
        const Descriptor *msg = enumFile->message_type(i);
        for (int j = 0; j < msg->enum_type_count(); j++) {
            if (enumType->full_name() == msg->enum_type(j)->full_name()) {
                return NEIGHBOUR_ENUM;
            }
        }
    }

    return GLOBAL_ENUM;
}

void ClassGeneratorBase::getMethodParameters(const MethodDescriptor *method, std::map<std::string, std::string> &parameters)
{
    std::string inputTypeName = method->input_type()->full_name();
    std::string outputTypeName = method->output_type()->full_name();
    std::string methodName = method->name();
    std::string methodNameUpper = method->name();
    methodNameUpper[0] =  static_cast<char>(::toupper(methodNameUpper[0]));
    utils::replace(inputTypeName, ".", "::");
    utils::replace(outputTypeName, ".", "::");
    parameters = {{"classname", mClassName},
                  {"return_type", outputTypeName},
                  {"method_name", methodName},
                  {"method_name_upper", methodNameUpper},
                  {"param_type", inputTypeName},
                  {"param_name", "arg"},
                  {"return_name", "ret"}
                 };
}

bool ClassGeneratorBase::hasGlobalEnum(const std::list<const FileDescriptor *> &list)
{
    for (auto file : list) {
        if (file->enum_type_count() <= 0) {
            return false;
        }
    }
    return true;
}

void ClassGeneratorBase::printField(const google::protobuf::Descriptor *message, const FieldDescriptor *field, const char *fieldTemplate)
{
    assert(field != nullptr);
    std::map<std::string, std::string> propertyMap;
    if (producePropertyMap(message, field, propertyMap)) {
        mPrinter->Print(propertyMap, fieldTemplate);
    }
}

bool ClassGeneratorBase::producePropertyMap(const google::protobuf::Descriptor *message, const FieldDescriptor *field, PropertyMap &propertyMap)
{
    assert(field != nullptr);
    std::string scriptable = "true";
    std::string typeName = getTypeName(field, message);
    std::string qmlAliasTypeName = getQmlAliasTypeName(field, message);
    std::string getterType = typeName;

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

    std::string capProperty = field->name();
    capProperty[0] = static_cast<char>(::toupper(capProperty[0]));

    std::string typeNameNoList = typeName;
    if (field->is_repeated() && !field->is_map()) {
        if(field->type() == FieldDescriptor::TYPE_MESSAGE
                || field->type() == FieldDescriptor::TYPE_ENUM) {
            typeNameNoList.resize(typeNameNoList.size() - strlen(Templates::ListSuffix));
        } else {
            typeNameNoList.resize(typeNameNoList.size() - strlen("List"));
        }
    }

    if (!field->is_map() && !field->is_repeated() && (field->type() == FieldDescriptor::TYPE_INT64
                                                      || field->type() == FieldDescriptor::TYPE_SINT64
                                                      || field->type() == FieldDescriptor::TYPE_FIXED64
                                                      || field->type() == FieldDescriptor::TYPE_SFIXED64)) {
        scriptable = "false";
    }

    if (field->type() == FieldDescriptor::TYPE_INT32
         || field->type() == FieldDescriptor::TYPE_FIXED32
         || field->type() == FieldDescriptor::TYPE_SFIXED32
         || field->type() == FieldDescriptor::TYPE_INT64
         || field->type() == FieldDescriptor::TYPE_FIXED64
         || field->type() == FieldDescriptor::TYPE_SFIXED64) {
        getterType = "const " + getterType;
    }

    std::string fieldName = utils::lowerCaseName(field->name());

    fieldName = qualifiedName(fieldName);
    propertyMap = {{"type", typeName},
                   {"classname", utils::upperCaseName(message->name())},
                   {"type_lower", typeNameLower},
                   {"property_name", fieldName},
                   {"property_name_cap", capProperty},
                   {"type_nolist", typeNameNoList},
                   {"qml_alias_type", qmlAliasTypeName},
                   {"scriptable", scriptable},
                   {"getter_type", getterType}
                  };
    return true;
}

bool ClassGeneratorBase::isComplexType(const FieldDescriptor *field)
{
    assert(field != nullptr);
    return field->type() == FieldDescriptor::TYPE_MESSAGE
            || field->type() == FieldDescriptor::TYPE_STRING
            || field->type() == FieldDescriptor::TYPE_BYTES;
}

std::string ClassGeneratorBase::qualifiedName(const std::string &name)
{
    std::string fieldName(name);
    const std::vector<std::string> &searchExeptions = Templates::ListOfQmlExeptions;

    auto searchResult = std::find(searchExeptions.begin(), searchExeptions.end(), fieldName);
    if (searchResult != searchExeptions.end()) {
        return fieldName.append(Templates::ProtoSufix);
    }
    return fieldName;
}

void ClassGeneratorBase::printInclude(const google::protobuf::Descriptor *message, const FieldDescriptor *field, std::set<std::string> &existingIncludes)
{
    assert(field != nullptr);
    std::string newInclude;
    const char *includeTemplate;
    switch (field->type()) {
    case FieldDescriptor::TYPE_MESSAGE:
        if ( field->is_map() ) {
            newInclude = "QMap";
            assert(field->message_type() != nullptr);
            assert(field->message_type()->field_count() == 2);
            printInclude(message, field->message_type()->field(0), existingIncludes);
            printInclude(message, field->message_type()->field(1), existingIncludes);
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
        EnumVisibility enumVisibily = getEnumVisibility(field, message);
        if (enumVisibily == GLOBAL_ENUM) {
            includeTemplate = Templates::GlobalEnumIncludeTemplate;
        } else if (enumVisibily == NEIGHBOUR_ENUM) {
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
        mPrinter->Print({{"include", newInclude}}, includeTemplate);
        existingIncludes.insert(newInclude);
    }
}

bool ClassGeneratorBase::hasQmlAlias(const ::google::protobuf::FieldDescriptor *field)
{
    return !field->is_map() && !field->is_repeated()
            && (field->type() == FieldDescriptor::TYPE_INT32
                || field->type() == FieldDescriptor::TYPE_SFIXED32
                || field->type() == FieldDescriptor::TYPE_FIXED32)
            && GeneratorOptions::instance().hasQml();
}

