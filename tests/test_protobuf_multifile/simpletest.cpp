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

#include "qtprotobufnamespace/tests/stepchildenummessage.h"
#include "qtprotobufnamespace/tests/simpleboolmessage.h"
#include "qtprotobufnamespace/tests/simpleintmessage.h"
#include "qtprotobufnamespace/tests/simplesintmessage.h"
#include "qtprotobufnamespace/tests/simpleuintmessage.h"
#include "qtprotobufnamespace/tests/simpleint64message.h"
#include "qtprotobufnamespace/tests/simplesint64message.h"
#include "qtprotobufnamespace/tests/simpleuint64message.h"
#include "qtprotobufnamespace/tests/simplefixedint32message.h"
#include "qtprotobufnamespace/tests/simplefixedint64message.h"
#include "qtprotobufnamespace/tests/simplesfixedint32message.h"
#include "qtprotobufnamespace/tests/simplesfixedint64message.h"
#include "qtprotobufnamespace/tests/simplestringmessage.h"
#include "qtprotobufnamespace/tests/simplefloatmessage.h"
#include "qtprotobufnamespace/tests/simpledoublemessage.h"
#include "qtprotobufnamespace/tests/simpleenummessage.h"
#include "qtprotobufnamespace/tests/simpleenumlistmessage.h"
#include "qtprotobufnamespace/tests/simplefileenummessage.h"
#include "qtprotobufnamespace/tests/simpleexternalenummessage.h"
#include "qtprotobufnamespace1/externaltests/externalcomplexmessage.h"
#include "qtprotobufnamespace/tests/complexmessage.h"
#include "qtprotobufnamespace/tests/simplebytesmessage.h"
#include "qtprotobufnamespace/tests/repeatedintmessage.h"
#include "qtprotobufnamespace/tests/repeatedbytesmessage.h"
#include "qtprotobufnamespace/tests/repeateddoublemessage.h"
#include "qtprotobufnamespace/tests/repeatedcomplexmessage.h"
#include "qtprotobufnamespace/tests/repeatedfloatmessage.h"
#include "qtprotobufnamespace/tests/repeatedsintmessage.h"
#include "qtprotobufnamespace/tests/repeatedstringmessage.h"
#include "qtprotobufnamespace/tests/repeateduintmessage.h"
#include "qtprotobufnamespace/tests/repeatedint64message.h"
#include "qtprotobufnamespace/tests/repeatedsint64message.h"
#include "qtprotobufnamespace/tests/repeateduint64message.h"
#include "qtprotobufnamespace/tests/repeatedfixedintmessage.h"
#include "qtprotobufnamespace/tests/repeatedsfixedintmessage.h"
#include "qtprotobufnamespace/tests/repeatedfixedint64message.h"
#include "qtprotobufnamespace/tests/repeatedsfixedint64message.h"
#include "qtprotobufnamespace/tests/repeatedexternalcomplexmessage.h"
#include "qtprotobufnamespace/tests/simplesint32stringmapmessage.h"
#include "qtprotobufnamespace/tests/simplestringstringmapmessage.h"
#include "qtprotobufnamespace/tests/emptymessage.h"
#include "qtprotobufnamespace/tests/message_uderscore_name.h"
#include "qtprotobufnamespace/tests/messageuderscorename.h"
#include "qtprotobufnamespace/tests/messageunderscorefield.h"
#include "qtprotobufnamespace/tests/priormessageunderscorefield.h"
#include "qtprotobufnamespace/tests/followingmessageunderscorefield.h"
#include "qtprotobufnamespace/tests/combinedmessageunderscorefield.h"
#include "qtprotobufnamespace/tests/sequence/testmessagesequence.h"
#include "qtprotobufnamespace/tests/sequence/testmessagesequence2.h"
#include "qtprotobufnamespace/tests/sequence/cyclingseconddependency.h"
#include "qtprotobufnamespace/tests/sequence/cyclingfirstdependency.h"
#include "qtprotobufnamespace/tests/messageuppercasereserved.h"
#include "qtprotobufnamespace/tests/messagereserved.h"
#include "qtprotobufnamespace/tests/messageenumreserved.h"
#include "qtprotobufnamespace/tests/sequence/repeatedfieldsequence.h"
#include "qtprotobufnamespace/tests/sequence/repeatedfieldsequence2.h"
#include "qtprotobufnamespace/tests/sequence/mapfieldsequence.h"
#include "qtprotobufnamespace/tests/sequence/mapfieldsequence2.h"
#include "qtprotobufnamespace/tests/sequence/lowercasesequence.h"
#include "qtprotobufnamespace/tests/sequence/lowercasesequence2.h"
#include "qtprotobufnamespace/tests/lowercasemessagename.h"
#include "qtprotobufnamespace/tests/lowercasefieldmessagename.h"
#include "qtprotobufnamespace/tests/messageuppercase.h"
#include "qtprotobufnamespace/tests/simpleint32complexmessagemapmessage.h"
#include "emptymessage.h"
#include "simpleintmessage.h"
#include "nopackageexternalmessage.h"
#include "qtprotobufnamespace/tests/nopackagemessage.h"

#include "globalenums.h"

#include "../test_protobuf/simpletest.cpp.inc"
