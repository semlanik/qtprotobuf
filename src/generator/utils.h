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
#include <vector>
#include <sstream>
#include <unordered_map>
#include <list>
#include <algorithm>

#ifdef QT_PROTOBUF_DEBUG_GENERATOR
#define QT_PROTOBUF_DEBUG(X) std::cout << X << std::endl
#else
#define QT_PROTOBUF_DEBUG(X)
#endif

namespace google { namespace protobuf {
class FileDescriptor;
}}

namespace QtProtobuf {
namespace generator {

/*!
 * \ingroup generator
 * \private
 * \brief The utils class
 */
class utils {
public:
static void split(const std::string &str, std::vector<std::string> &container, char delim)
{
    container.clear();
    if (str.size() <= 0) {
        return;
    }
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

static void tolower(std::string &str) {
    std::transform(std::begin(str), std::end(str), std::begin(str), ::tolower);
}

static std::string removeFileSuffix(std::string fileName) {
    size_t index = fileName.rfind(".");
    fileName.resize(index);
    return fileName;
}

static std::string extractFileBasename(std::string fileName) {
    size_t index = fileName.rfind(".");
    fileName.resize(index);
    index = fileName.rfind("/");
    if (index != std::string::npos) {
        return fileName.substr(index + 1, fileName.size() - 1);
    }
    return fileName;
}


static std::string upperCaseName(const std::string &name)
{
    std::string upperCaseName(name);
    upperCaseName[0] = static_cast<char>(::toupper(upperCaseName[0]));
    return upperCaseName;
}

static std::string lowerCaseName(const std::string &name)
{
    std::string lowerCaseName(name);
    lowerCaseName[0] = static_cast<char>(::tolower(lowerCaseName[0]));
    return lowerCaseName;
}

inline static std::string& rtrim(std::string& s)
{
    s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
    return s;
}

// trim from beginning of string (left)
inline static std::string& ltrim(std::string& s)
{
    s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
    return s;
}

// trim from both ends of string (right then left)
inline static std::string& trim(std::string& s)
{
    return ltrim(rtrim(s));
}

};

#define UNUSED(expr) do { (void)(expr); } while (0)

using PackagesList = std::unordered_map<std::string/*package*/, std::list<const ::google::protobuf::FileDescriptor *>>;

} //namespace generator
} //namespace QtProtobuf
