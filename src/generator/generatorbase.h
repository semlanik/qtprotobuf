/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
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

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <string>
#include <memory>
#include <functional>
#include <google/protobuf/descriptor.h>

namespace google { namespace protobuf {
class FileDescriptor;
class Descriptor;
namespace compiler {
class GeneratorContext;
}}}

namespace QtProtobuf {
namespace generator {

/*!
 * \defgroup generator
 *
 * \brief QtProtobuf code generator for protobuf messages and grpc services
 *
 * \details QtProtobuf code generator is program to be used in conjunction with proto compiler aka protoc.
 *
 * It might be used in few ways:
 *  - manualy
 *  - \ref cmake "CMake macro"
 *  - \ref qmake "qmake macro"
 *
 * \section Manual usage
 *
 * \code
 * [QT_PROTOBUF_OPTIONS="[SINGLE|MULTI]:QML:COMMENTS:FOLDER"] protoc --plugin=protoc-gen-qtprotobuf=<path/to/bin/>qtprotobufgen --qtprotobuf_out=<output_dir> [-I/extra/proto/include/path] <protofile>.proto
 * \endcode
 *
 * Generator supports options that could be provided as environment variable to tune generation.
 * Following options are supported:
 *
 * *SINGLE* - enables single-file generation when for each *.proto* file single pair of *.h* *.cpp* files is generated
 *
 * \note Enabled by default. Excluded by SINGLE
 *
 * *MULTI* - enables multi-file generation when for each message separate pair of *.h* *.cpp*
 *
 * \note Has higher priority than SINGLE
 *
 * *QML* - enables QML code generation in protobuf classes. If is set QML-related code for lists and QML registration to be generated.
 *
 * *COMMENTS* - enables comments copying from .proto files
 *
 * *FOLDER* - enables folder-based generation
 *
 * \section cmake CMake
 *
 * For CMake based project QtProtobuf has macroses those should be used to generate code and in link it to your project:
 *
 * - \ref cmake_qtprotobuf_generate
 * - \ref cmake_qtprotobuf_link_target
 *
 *
 * \subsection cmake_qtprotobuf_generate qtprotobuf_generate
 *
 * \brief qtprotobuf_generate is cmake helper function that automatically generates STATIC library target from your .proto files
 *
 * \param TARGET name of you target that generated code archive will be linked to
 * \param GENERATED_TARGET name that will be used for generated archive library target. It's usefull when you supposed to have multiple generated targets to be linked to single one.
 * \param OUT_DIR output directory that will contain generated artifacts. Usually subfolder in build directory should be used
 * \param GENERATED_HEADERS List of header files expected after generator job finished
 * \param EXCLUDE_HEADERS List of header files to be excluded from pre-parsed list of expected header files (e.g. nested messages that are not supported by QtProtobuf generator)
 * \param PROTO_FILES List of .proto files that will be used in generation procedure
 * \param MULTI Enables multi-files generation mode. If provided in parameter list generator will create pair of header/source files for each message
 *        \note multi-files generation mode is defined as deprecated by QtProtobuf team, and might have poor support in future
 * \param QML Enables QML code generation in protobuf classes. If provided in parameter list QML related code for lists and QML registration to be generated.
 * \param COMMENTS Enables comments copying from .proto files. If provided in parameter list message and field related comments will be copied to generated header files.
 * \param FOLDER Enables folder based generation. If provided in parameter list generator will place generated artifacts to folder structure according to package of corresponding .proto file
 *
 * \subsection cmake_qtprotobuf_link_target qtprotobuf_link_target
 *
 * \brief qtprotobuf_link_target is cmake helper function that links generated protobuf target to your binary
 *
 * \details It's useful when you try to link generated target to shared library or/and to executable that doesn't utilize all protobuf generated classes directly from C++ code, but requires them from QML.
 *
 * \param TARGET name of target to link to
 * \param GENERATED_TARGET protobuf generated target name
 *
 * \section qmake
 *  - \ref qmake_qtprotobuf_generate
 *
 * \subsection qmake_qtprotobuf_generate qtprotobuf_generate
 *
 * \brief qtprotobuf_generate is qmake helper <a href="https://doc.qt.io/qt-5/qmake-language.html#test-functions">test function</a> that generates QtProtobuf source code based on files provided by PROTO_FILES global context variable
 * \param generate_qml generate_qml enables/disables QML code generation in protobuf classes. If set to `true` QML-related code for lists and QML registration to be generated.
 */

/*!
 * \ingroup generator
 * \private
 * \brief The GeneratorBase class
 */
class GeneratorBase: public ::google::protobuf::compiler::CodeGenerator
{
public:
    enum Mode {
        SingleMode = 0,
        MultiMode,
    };
    GeneratorBase(Mode mode);
    virtual ~GeneratorBase() = default;

    virtual bool GenerateAll(const std::vector<const ::google::protobuf::FileDescriptor *> &files,
                             const std::string &parameter,
                             ::google::protobuf::compiler::GeneratorContext *generatorContext,
                             std::string *error) const override;
    bool HasGenerateAll() const override { return true; }

    static void printDisclaimer(const std::shared_ptr<::google::protobuf::io::Printer> printer);
    static void printPreamble(const std::shared_ptr<::google::protobuf::io::Printer> printer);
    static void printInclude(const std::shared_ptr<::google::protobuf::io::Printer> printer, const google::protobuf::Descriptor *message, const google::protobuf::FieldDescriptor *field, std::set<std::string> &existingIncludes);
    static void printQtProtobufUsingNamespace(const std::shared_ptr<::google::protobuf::io::Printer> printer);
    static void printNamespaces(const std::shared_ptr<::google::protobuf::io::Printer> printer, const std::vector<std::string> namespaces);
protected:
    void iterateNonNestedFileds(const ::google::protobuf::FileDescriptor *file, std::function<void(const ::google::protobuf::Descriptor *)> callback) const;
    static std::string generateBaseName(const ::google::protobuf::FileDescriptor *file, std::string name);
private:
    Mode m_mode;
};

}
}
