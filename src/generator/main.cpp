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

#include <google/protobuf/compiler/plugin.h>

#include "generator.h"
#include "singlefilegenerator.h"
#include "generatoroptions.h"
#include "utils.h"

/*! \mainpage
 * QtProtobuf is project that provides gRPC and Protobuf generator and bindings for Qt framework
 *
 * Look at project <a href="https://github.com/semlanik/qtprotobuf/blob/master/README.md">README</a> for installation and usage details.
 *
 * QtProtobuf distributed in terms of MIT license and is free for modification and use under MIT
 * license <a href="https://github.com/semlanik/qtprotobuf/blob/master/LICENSE">terms</a>.
 *
 * QtProtobuf has 4 modules:
 *  - \ref generator
 *  - \ref QtProtobuf
 *  - \ref QtGrpc
 *  - \ref QtProtobufWellKnownTypes
 */
using namespace ::QtProtobuf::generator;
int main(int argc, char *argv[])
{
    char *optionsPtr = getenv("QT_PROTOBUF_OPTIONS");
    if (optionsPtr != nullptr) {
        QT_PROTOBUF_DEBUG("QT_PROTOBUF_OPTIONS: " << optionsPtr);
        GeneratorOptions::instance().parseFromEnv(optionsPtr);
    }
    if (GeneratorOptions::instance().isMulti()) {
        QtProtobuf::generator::QtGenerator generator;
        return ::google::protobuf::compiler::PluginMain(argc, argv, &generator);
    }
    QtProtobuf::generator::SingleFileGenerator generator;
    return ::google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
