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

#include "stepchildenummessage.h"
#include "simpleboolmessage.h"
#include "simpleintmessage.h"
#include "simplesintmessage.h"
#include "simpleuintmessage.h"
#include "simpleint64message.h"
#include "simplesint64message.h"
#include "simpleuint64message.h"
#include "simplefixedint32message.h"
#include "simplefixedint64message.h"
#include "simplesfixedint32message.h"
#include "simplesfixedint64message.h"
#include "simplestringmessage.h"
#include "simplefloatmessage.h"
#include "simpledoublemessage.h"
#include "simpleenummessage.h"
#include "simpleenumlistmessage.h"
#include "simplefileenummessage.h"
#include "simpleexternalenummessage.h"
#include "externalcomplexmessage.h"
#include "complexmessage.h"
#include "simplebytesmessage.h"
#include "repeatedintmessage.h"
#include "repeatedbytesmessage.h"
#include "repeateddoublemessage.h"
#include "repeatedcomplexmessage.h"
#include "repeatedfloatmessage.h"
#include "repeatedsintmessage.h"
#include "repeatedstringmessage.h"
#include "repeateduintmessage.h"
#include "repeatedint64message.h"
#include "repeatedsint64message.h"
#include "repeateduint64message.h"
#include "repeatedfixedintmessage.h"
#include "repeatedsfixedintmessage.h"
#include "repeatedfixedint64message.h"
#include "repeatedsfixedint64message.h"
#include "repeatedexternalcomplexmessage.h"
#include "simplesint32stringmapmessage.h"
#include "simplestringstringmapmessage.h"
#include "emptymessage.h"
#include "message_uderscore_name.h"
#include "messageuderscorename.h"
#include "messageunderscorefield.h"
#include "priormessageunderscorefield.h"
#include "followingmessageunderscorefield.h"
#include "combinedmessageunderscorefield.h"
#include "testmessagesequence.h"
#include "testmessagesequence2.h"
#include "cyclingseconddependency.h"
#include "cyclingfirstdependency.h"
#include "messageuppercasereserved.h"
#include "messagereserved.h"
#include "messageenumreserved.h"

#include "globalenums.h"

#include "../test_protobuf/simpletest.cpp.inc"
