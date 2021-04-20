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

#include "generatoroptions.h"

#include "utils.h"

#include <iostream>

static const std::string MultifileBuildOption("MULTI");
static const std::string QmlPluginOption("QML");
static const std::string CommentsGenerationOption("COMMENTS");
static const std::string FolderGenerationOption("FOLDER");
static const std::string FieldEnumGenerationOption("FIELDENUM");
static const std::string ExtraNamespaceGenerationOption("EXTRA_NAMESPACE");

using namespace ::QtProtobuf::generator;

GeneratorOptions::GeneratorOptions() : mIsMulti(false)
  , mHasQml(false)
  , mGenerateComments(false)
  , mIsFolder(false)
  , mGenerateFieldEnum(false)
{
}

void GeneratorOptions::parseFromEnv(const std::string &options)
{
    std::vector<std::string> optionsList = utils::split(options, ':');
    for (auto option : optionsList) {
        QT_PROTOBUF_DEBUG("option: " << option);
        if (option.compare(MultifileBuildOption) == 0) {
            QT_PROTOBUF_DEBUG("set mIsMulti: true");
            mIsMulti = true;
        } else if (option.compare(QmlPluginOption) == 0) {
            QT_PROTOBUF_DEBUG("set mHasQml: true");
            mHasQml = true;
        } else if (option.compare(CommentsGenerationOption) == 0) {
            QT_PROTOBUF_DEBUG("set mGenerateComments: true");
            mGenerateComments = true;
        } else if (option.compare(FolderGenerationOption) == 0) {
            QT_PROTOBUF_DEBUG("set mIsFolder: true");
            mIsFolder = true;
        } else if (option.compare(FieldEnumGenerationOption) == 0) {
            QT_PROTOBUF_DEBUG("set mGenerateFieldEnum: true");
            mGenerateFieldEnum = true;
        } else if (option.find(ExtraNamespaceGenerationOption) == 0) {
            QT_PROTOBUF_DEBUG("set mGenerateFieldEnum: true");
            std::vector<std::string> compositeOption = utils::split(options, '=');
            if (compositeOption.size() == 2) {
                mExtraNamespace = compositeOption[1];
                if (mExtraNamespace.size() > 1 && mExtraNamespace[0] == '\"'
                        && mExtraNamespace[mExtraNamespace.size() - 1] == '\"') {
                    mExtraNamespace = mExtraNamespace.substr(1, mExtraNamespace.size() - 2);
                }
            }
        }
    }
}
