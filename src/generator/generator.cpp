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
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "generator.h"
#include "templates.h"

#include <google/protobuf/descriptor.pb.h>

#include <google/protobuf/stubs/logging.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.h>

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <string>

using namespace ::google::protobuf;
using namespace ::google::protobuf::compiler;

using namespace qtprotobuf;

namespace utils {

void split(const std::string& str, std::vector<std::string>& cont, char delim)
{
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

}

static std::unordered_map<FieldDescriptor::Type, std::string> TypeReflection = {
    {FieldDescriptor::TYPE_DOUBLE, "qreal"},
    {FieldDescriptor::TYPE_FLOAT, "qreal"},
    //        {FieldDescriptor::TYPE_INT64, "int"},//Not supported see https://doc.qt.io/qt-5/qtqml-typesystem-basictypes.html
    //        {FieldDescriptor::TYPE_UINT64,"int"},//Not supported see https://doc.qt.io/qt-5/qtqml-typesystem-basictypes.html
    {FieldDescriptor::TYPE_INT32, "int"},
    //        {FieldDescriptor::TYPE_FIXED64, "int"},//Not supported see https://doc.qt.io/qt-5/qtqml-typesystem-basictypes.html
    //        {FieldDescriptor::TYPE_FIXED32, "int"},//Not supported see https://doc.qt.io/qt-5/qtqml-typesystem-basictypes.html
    {FieldDescriptor::TYPE_BOOL, "bool"},
    {FieldDescriptor::TYPE_STRING, "QString"},
    {FieldDescriptor::TYPE_GROUP, ""},//Not supported and deprecated in protobuf
//    {FieldDescriptor::TYPE_MESSAGE, ""},//Custom typename
    {FieldDescriptor::TYPE_BYTES, "QByteArray"},
    {FieldDescriptor::TYPE_UINT32, "int"},//Limited usage see https://doc.qt.io/qt-5/qtqml-typesystem-basictypes.html
    {FieldDescriptor::TYPE_ENUM, "$custom_enumtype$"},
    {FieldDescriptor::TYPE_SFIXED32, "int"},
    //        {FieldDescriptor::TYPE_SFIXED64, "int"},//Not supported see https://doc.qt.io/qt-5/qtqml-typesystem-basictypes.html
    {FieldDescriptor::TYPE_SINT32, "int"},
    //        {FieldDescriptor::TYPE_SINT64, "int"},//Not supported see https://doc.qt.io/qt-5/qtqml-typesystem-basictypes.html
};

class QtClassGenerator {
    std::unique_ptr<io::ZeroCopyOutputStream> mOutput;
    string mPackage;
    const Descriptor* mMessage;
    int mNamespaceCount;
    io::Printer mPrinter;

public:
    QtClassGenerator(const std::string& package, const Descriptor* message, std::unique_ptr<io::ZeroCopyOutputStream> out) : mOutput(std::move(out))
      , mPackage(package)
      , mMessage(message)
      , mNamespaceCount(0)
      , mPrinter(mOutput.get(), '$')
    {
    }

    void run() {
        printPreamble();
        printNamespaces();
        printClass();
        enclose();
    }

private:
    bool producePropertyMap(const FieldDescriptor* field, std::map<string, string>& propertyMap) {
        std::string typeName = getTypeName(field);

        if (typeName.size() <= 0) {
            std::cerr << "Type "
                      << field->type_name()
                      << " is not supported by Qt Generator"
                         " please look at https://doc.qt.io/qt-5/qtqml-typesystem-basictypes.html"
                         " for details" << std::endl;
            return false;
        }

        string capProperty = field->camelcase_name();
        capProperty[0] = ::toupper(capProperty[0]);

        propertyMap = {{"type", typeName},
            {"property_name", field->camelcase_name()},
            {"property_name_cap", capProperty}};
        return true;
    }
    void printPreamble() {
        mPrinter.Print("#pragma once\n"
                       "#include <QObject>\n");
    }

    void printNamespaces() {
        std::vector<string> namespaces;
        utils::split(mPackage, namespaces, '.');
        mNamespaceCount = namespaces.size();
        for(auto ns: namespaces) {
            mPrinter.Print({{"namespace", ns}}, NamespaceTemplate);
        }
    }

    void printClass() {
        mPrinter.Print({{"classname", mMessage->name()}}, StartTemplate);

        //private section
        for(int i = 0; i < mMessage->field_count(); i++) {
            printField(mMessage->field(i), PropertyTemplate);
        }
        for(int i = 0; i < mMessage->field_count(); i++) {
            printField(mMessage->field(i), MemberTemplate);
        }
        printQEnums();

        //public section
        mPrinter.Print("public:\n");
        printConstructor();
        for(int i = 0; i < mMessage->field_count(); i++) {
            printField(mMessage->field(i), GetterTemplate);
        }
        for(int i = 0; i < mMessage->field_count(); i++) {
            auto field = mMessage->field(i);
            if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                printField(field, SetterTemplateComplexType);
            } else {
                printField(field, SetterTemplateSimpleType);
            }
        }
        mPrinter.Print("signals:\n");
        for(int i = 0; i < mMessage->field_count(); i++) {
            printField(mMessage->field(i), SignalTemplate);
        }
    }

    void printField(const FieldDescriptor* field, const char* fieldTemplate) {
        std::map<string, string> propertyMap;
        if(producePropertyMap(field, propertyMap)) {
            mPrinter.Print(propertyMap, fieldTemplate);
        }
    }

    void enclose() {
        mPrinter.Print("};\n");
        while(mNamespaceCount > 0) {
            mPrinter.Print("}\n");
            --mNamespaceCount;
        }
    }

    std::string getTypeName(const FieldDescriptor* field) {
        string typeName;
        if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
            typeName = field->message_type()->name();
        } else {
            auto it = TypeReflection.find(field->type());
            if(it != std::end(TypeReflection)) {
                typeName = it->second;
            }
        }

        if (field->is_repeated()) {
            typeName = typeName.append("Model");
        }

        return typeName;
    }

    void printQEnums() {
        if (mMessage->enum_type_count() <= 0) {
            return;
        }

        mPrinter.Print("    Q_ENUMS(");
        for(int i = 0; mMessage->enum_type_count(); i++) {
            const auto enumDescr = mMessage->enum_type(i);
            mPrinter.Print(enumDescr->name().c_str());
        }
        mPrinter.Print(")\n\n");
        mPrinter.Print("public:\n");
        for(int i = 0; mMessage->enum_type_count(); i++) {
            const auto enumDescr = mMessage->enum_type(i);
            mPrinter.Print({{"enum", enumDescr->name()}}, "    enum $enum$ {\n");
            for (int j = 0; j < enumDescr->value_count(); j++) {
                const auto valueDescr = enumDescr->value(j);
                mPrinter.Print({{"enumvalue", valueDescr->name()},
                                {"value", std::to_string(valueDescr->number())}}, "        $enumvalue$ = $value$,\n");
            }
            mPrinter.Print("};\n");
        }
    }

    void printConstructor() {
        //FIXME: Explicit default values are not allowed in proto3 seems
        //this function is useless
        mPrinter.Print({{"classname", mMessage->name()}},
                       "$classname$(QObject parent = nullptr) : QObject(parent)\n");
        for (int i = 0; i < mMessage->field_count(); i++) {
            const FieldDescriptor* field = mMessage->field(i);
            std::string defaultValue;
            if (field->has_default_value()) {
                switch(field->type()) {
                case FieldDescriptor::TYPE_DOUBLE:
                    defaultValue = std::to_string(field->default_value_double());
                    break;
                case FieldDescriptor::TYPE_FLOAT:
                    defaultValue = std::to_string(field->default_value_float());
                    break;
                case FieldDescriptor::TYPE_INT32:
                case FieldDescriptor::TYPE_UINT32:
                case FieldDescriptor::TYPE_SFIXED32:
                case FieldDescriptor::TYPE_SINT32:
                    defaultValue = std::to_string(field->default_value_int32());
                    break;
                case FieldDescriptor::TYPE_BOOL:
                    defaultValue = field->default_value_bool() ? "true" : "false";
                    break;
                case FieldDescriptor::TYPE_STRING:
                    defaultValue = field->default_value_string();
                    break;
                case FieldDescriptor::TYPE_ENUM:
                    defaultValue = field->default_value_enum()->name();
                    break;
                default:
                    std::cerr << "Default value substitution"
                                 " is not supported for type"
                              << field->type_name() << std::endl;
                    break;
                }
                if (defaultValue.size() > 0) {
                    mPrinter.Print({{"property_name", field->camelcase_name()},
                                    {"default_value", defaultValue}},
                                   ", $m_property_name$($default_value$)\n");
                }
            }
        }
        mPrinter.Print("{}\n\n");
    }
};

bool QtGenerator::Generate(const FileDescriptor* file,
                           const std::string& parameter,
                           GeneratorContext* generator_context,
                           std::string* error) const
{
    if (file->syntax() != FileDescriptor::SYNTAX_PROTO3) {
        *error = "Invalid proto used. This plugin only supports 'proto3' syntax";
        return false;
    }

    for(int i = 0; i < file->message_type_count(); i++) {
        const Descriptor* message = file->message_type(i);
        string filename = message->name() + ".h";
        std::transform(std::begin(filename), std::end(filename), std::begin(filename), ::tolower);
        QtClassGenerator classGen(file->package(), message, std::move(std::unique_ptr<io::ZeroCopyOutputStream>(generator_context->Open(filename))));
        classGen.run();
    }

    return true;
}
