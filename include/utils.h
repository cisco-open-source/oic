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

void printOCRep(const OCRepresentation &rep);

bool checkType(const OCRepresentation &rep, const std::string requiredType);

bool checkInterface(const OCRepresentation &rep, const std::string requiredInt);

bool checkAttr(const OCRepresentation &rep, const std::string requiredIName, const AttributeType requiredType);

bool checkAttr(const OCRepresentation &rep, const std::string requiredIName, const AttributeType requiredType, bool mandatory);