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

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <list>
#include <algorithm>

namespace google { namespace protobuf {
class FileDescriptor;
}}

namespace qtprotobuf {
namespace generator {
class utils {
public:
static void split(const std::string &str, std::vector<std::string> &container, char delim)
{
    container.clear();
    std::stringstream stream(str);
    std::string token;
    while (std::getline(stream, token, delim)) {
        container.push_back(token);
    }
}

static void replace(std::string &data, const std::string &from, const std::string &to) {
    size_t pos = data.find(from);
    while (pos != std::string::npos) {
        data.replace(pos, from.size(), to);
        pos = data.find(from, pos + to.size());
    }
}

static void tolower(std::string& str) {
    std::transform(std::begin(str), std::end(str), std::begin(str), ::tolower);
}

};

#define UNUSED(expr) do { (void)(expr); } while (0)

using PackagesList = std::unordered_map<std::string/*package*/, std::list<const ::google::protobuf::FileDescriptor *>>;

} //namespace generator
} //namespace qtprotobuf
