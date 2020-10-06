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

#pragma once

#include <vector>
#include <map>
#include <string>
#include <functional>

#include "utils.h"
#include "templates.h"

#include <google/protobuf/descriptor.h>

namespace QtProtobuf {
namespace generator {

using TypeMap = std::map<std::string, std::string>;
using PropertyMap = std::map<std::string, std::string>;
using MethodMap = std::map<std::string, std::string>;

struct common {
    enum EnumVisibility {
        GLOBAL_ENUM,
        LOCAL_ENUM,
        NEIGHBOR_ENUM
    };

    template <typename T>
    static std::vector<std::string> getNamespaces(const T *type) {
        if (type == nullptr) {
            return {};
        }

        std::vector<std::string> namespacesList = utils::split(type->full_name(), '.');
        if (!namespacesList.empty()) {
            namespacesList.pop_back(); //Remove name
        }
        return namespacesList;
    }

    static std::vector<std::string> getNestedNamespaces(const ::google::protobuf::Descriptor *type) {
        auto namespaces = getNamespaces(type);
        auto package = utils::split(type->file()->package(), '.');
        for (size_t i = package.size(); i < namespaces.size(); i++) {
                namespaces[i] += Templates::QtProtobufNestedNamespace;
        }
        return namespaces;
    }


    static std::string getNamespacesString(const std::vector<std::string> &namespacesList, const std::string &separator);
    static std::string getScopeNamespacesString(std::string original, const std::string &scope);
    static TypeMap produceQtTypeMap(const ::google::protobuf::Descriptor *type, const ::google::protobuf::Descriptor *scope);
    static TypeMap produceMessageTypeMap(const ::google::protobuf::Descriptor *type, const ::google::protobuf::Descriptor *scope);
    static TypeMap produceEnumTypeMap(const ::google::protobuf::EnumDescriptor *type, const ::google::protobuf::Descriptor *scope);
    static TypeMap produceSimpleTypeMap(::google::protobuf::FieldDescriptor::Type type);
    static TypeMap produceTypeMap(const ::google::protobuf::FieldDescriptor *field, const ::google::protobuf::Descriptor *scope);
    static PropertyMap producePropertyMap(const ::google::protobuf::FieldDescriptor *field, const ::google::protobuf::Descriptor *scope);
    static std::string qualifiedName(const std::string &name);
    static bool isLocalEnum(const ::google::protobuf::EnumDescriptor *type, const google::protobuf::Descriptor *scope);
    static EnumVisibility enumVisibility(const ::google::protobuf::EnumDescriptor *type, const ::google::protobuf::Descriptor *scope);
    static bool hasQmlAlias(const ::google::protobuf::FieldDescriptor *field);
    static bool isQtType(const ::google::protobuf::FieldDescriptor *field);
    static bool isPureMessage(const ::google::protobuf::FieldDescriptor *field);

    using InterateMessageLogic = std::function<void(const ::google::protobuf::FieldDescriptor *, PropertyMap &)>;
    static void iterateMessageFields(const ::google::protobuf::Descriptor *message, InterateMessageLogic callback) {
        for (int i = 0; i < message->field_count(); i++) {
            const ::google::protobuf::FieldDescriptor *field = message->field(i);
            auto propertyMap = common::producePropertyMap(field, message);
            callback(field, propertyMap);
        }
    }

    static MethodMap produceMethodMap(const ::google::protobuf::MethodDescriptor *method, const std::string &scope); //TODO: scope should be ServiceDescriptor

    static void iterateMessages(const ::google::protobuf::FileDescriptor *file, std::function<void(const ::google::protobuf::Descriptor *)> callback);
    static void iterateNestedMessages(const ::google::protobuf::Descriptor *message, std::function<void(const ::google::protobuf::Descriptor *)> callback);

    static bool hasNestedMessages(const ::google::protobuf::Descriptor *message);

    static bool isNested(const ::google::protobuf::Descriptor *message);
    static bool isNestedOf(const ::google::protobuf::Descriptor *message, const ::google::protobuf::Descriptor *containing) {
        return containing == message->containing_type();
    }
    static const ::google::protobuf::Descriptor *findHighestMessage(const ::google::protobuf::Descriptor *message);
};

}
}
