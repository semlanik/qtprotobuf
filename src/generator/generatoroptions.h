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

#include <string>

namespace QtProtobuf {
namespace generator {

/*!
 * \ingroup generator
 * \private
 * \brief The GeneratorOptions class
 */
class GeneratorOptions
{
    GeneratorOptions();
public:
    static GeneratorOptions &instance() {
        static GeneratorOptions _instance;
        return _instance;
    }

    void parseFromEnv(const std::string &options);

    bool isMulti() const { return mIsMulti; }
    bool hasQml() const { return mHasQml; }
    bool generateComments() const { return mGenerateComments; }
    bool isFolder() const { return mIsFolder; }
    bool generateFieldEnum() const { return mGenerateFieldEnum; }

private:
    bool mIsMulti;
    bool mHasQml;
    bool mGenerateComments;
    bool mIsFolder;
    bool mGenerateFieldEnum;
};

}}
