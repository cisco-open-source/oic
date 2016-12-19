/****************************************************************************
**
** Copyright © 1992-2016 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
**
** $CISCO_BEGIN_LICENSE:APACHE$
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** http://www.apache.org/licenses/LICENSE-2.0
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <array>
#include <iostream>
#include <thread>
#include <stdexcept>
#include <sstream>
#include "OCApi.h"
#include "OCPlatform.h"

using namespace OC;

char * duplicateStr(const std::string sourceString);

std::string getUserHome();

bool file_exist(const char *filename);

void printOCResource(const OC::OCResource& oCResource);

void printOCRep(const OCRepresentation &rep);

bool checkType(const OCRepresentation &rep, const std::string requiredType);

bool checkInterface(const OCRepresentation &rep, const std::string requiredInt);

bool checkAttr(const OCRepresentation &rep, const std::string requiredIName, const AttributeType requiredType);

bool checkAttr(const OCRepresentation &rep, const std::string requiredIName, const AttributeType requiredType, bool mandatory);