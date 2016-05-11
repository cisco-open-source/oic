//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// OCClient.cpp : Defines the entry point for the console application.
//
#include <string>
#include <map>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include "AudioResource.h"
#include "BinarySwitch.h"
#include "MediaSource.h"
#include "NightMode.h"

using namespace OC;

std::shared_ptr<OCResource> curResource;

// Callback handler on GET request
void onGet(const HeaderOptions&, const OCRepresentation& rep, const int eCode)
{
    std::cout << "GET request was successful" << std::endl;
    //printOCRep(rep);
}

void onGetDevice(const OCRepresentation& rep)
{
    std::cout << "GET Device request was successful" << std::endl;
    //printOCRep(rep);
}

// Callback to found resources
void foundResource(std::shared_ptr<OCResource> resource)
{

    if (resource->uri() == "/oic/d" && !curResource)
    {
        curResource = resource;
        std::cout << "DISCOVERED Resource:" << std::endl;
        std::cout << "\tURI of the resource: " << resource->uri() << std::endl;
        std::cout << "\tHost address of the resource: " << resource->host() << std::endl;
        std::cout << "\tList of resource types: " << std::endl;

        for (auto &resourceTypes : resource->getResourceTypes())
        {
            std::cout << "\t\t" << resourceTypes << std::endl;
        }

        std::cout << "\tList of resource interfaces: " << std::endl;
        for (auto &resourceInterfaces : resource->getResourceInterfaces())
        {
            std::cout << "\t\t" << resourceInterfaces << std::endl;
        }

        QueryParamsMap test;
        resource->get(test, &onGet);
    }
}

static FILE* client_open(const char* /*path*/, const char *mode)
{
    return fopen("./oic_svr_db_client.dat", mode);
}

void PrintUsage()
{
    std::cout << "Usage : stbserver [-res|-dev]" << std::endl;
}

int main(int argc, char* argv[]) {
    int dev = 0, res = 0;

    std::cout << "main " << argc << std::endl;
    for (int opt = 1; opt < argc; opt++ )
    {
        std::cout << "opt nb = " << opt << " key = " <<  argv[opt] << " val = " << argv[opt + 1] << std::endl;

        if (!strcmp(argv[opt], "-dev") || !strcmp(argv[opt], "--dev"))
        {
            dev = 1;
            std::cout << "dev passed = " << dev << std::endl;
        }
        else if (!strcmp(argv[opt], "-res") || !strcmp(argv[opt], "--res"))
        {
            res = 1;
            std::cout << "res passed = " << res << std::endl;
        }
    }

    if (dev == 0 && res == 0)
    {
        std::cout << "Print help" << std::endl;
        PrintUsage(); return -1;
    }

    std::ostringstream requestURI;
    OCPersistentStorage ps {client_open, fread, fwrite, fclose, unlink };
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Both,
        "0.0.0.0",
        0,
        OC::QualityOfService::LowQos,
        &ps
    };

    OCPlatform::Configure(cfg);
    std::cout.setf(std::ios::boolalpha);
    std::string host = "";

    if (dev)
        OCPlatform::getDeviceInfo(host, OC_RSRVD_DEVICE_URI, CT_DEFAULT,
                                  &onGetDevice);
    if (res)
        OCPlatform::findResource("", OC_RSRVD_WELL_KNOWN_URI,
                                 CT_DEFAULT, &foundResource);

    std::mutex blocker;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(blocker);
    cv.wait(lock);

    return 0;
}
