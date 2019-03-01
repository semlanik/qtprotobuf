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

#pragma once

#include <string>

namespace qtprotobuf {

static const char* StartTemplate = "class $classname$ : public QObject\n"
                      "{\n"
                      "    Q_OBJECT\n";

static const char* NamespaceTemplate = "namespace $namespace$\n"
                                  "{\n";
static const char* PropertyTemplate = "    Q_PROPERTY($type$ $property_name$ READ $property_name$ WRITE set$property_name_cap$ NOTIFY $property_name$Changed)\n";
static const char* GetterTemplate = "    $type$ $property_name$() const {\n"
                                    "        return m_$property_name$;\n"
                                    "    }\n";

static const char* SetterTemplateSimpleType = "    void set$property_name_cap$($type$ $property_name$) {\n"
                                              "        if (m_$property_name$ != $property_name$) {\n"
                                              "             m_$property_name$ == $property_name$;\n"
                                              "             $property_name$Changed();\n"
                                              "        }\n"
                                              "    }\n";

static const char* SetterTemplateComplexType = "    void set$property_name_cap$(const $type$ &$property_name$) {\n"
                                              "        if (m_$property_name$ != $property_name$) {\n"
                                              "             m_$property_name$ == $property_name$;\n"
                                              "             $property_name$Changed();\n"
                                              "        }\n"
                                              "    }\n";

static const char* SignalTemplate = "    void $property_name$Changed();\n";
static const char* MemberTemplate = "    $type$ m_$property_name$Changed();\n";

static const char* EnumTemplate = "$type$";

}
