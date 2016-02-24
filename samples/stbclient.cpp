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

typedef std::map<OCResourceIdentifier, std::shared_ptr<OCResource>> DiscoveredResourceMap;

DiscoveredResourceMap discoveredResources;
static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;

// Callback handler on GET request
void onGet(const HeaderOptions&, const OCRepresentation& rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        std::cout << "GET request was successful" << std::endl;
        printOCRep(rep);
    }
    else
    {
        std::cout << "onGET Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

void getRepresentation(std::shared_ptr<OCResource> resource, std::string type, std::string interface)
{
    std::cout << "Getting Representation " << resource->uri() << " type " << type << " interface " << interface << std::endl;

    if (resource)
    {
        QueryParamsMap test;
        resource->get(type, interface, test, &onGet);
    }
}

// Callback to found resources
void foundResource(std::shared_ptr<OCResource> resource)
{
    std::cout << "In foundResource\n";
    std::string resourceURI;
    std::string hostAddress;
    try
    {
        // Do some operations with resource object.
        if (resource)
        {
            std::cout << "DISCOVERED Resource:" << std::endl;
            // Get the resource URI
            resourceURI = resource->uri();
            std::cout << "\tURI of the resource: " << resourceURI << std::endl;

            // Get the resource host address
            hostAddress = resource->host();
            std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

            // Get the resource types
            std::cout << "\tList of resource types: " << std::endl;
            for (auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;
                // Get the resource interfaces
                std::cout << "\t\tList of resource interfaces: " << std::endl;
                for (auto &resourceInterfaces : resource->getResourceInterfaces())
                {
                    std::cout << "\t\t\t" << resourceInterfaces << std::endl;
                }
            }
            for (auto &resourceTypes : resource->getResourceTypes())
            {
                for (auto &resourceInterfaces : resource->getResourceInterfaces())
                {
                    getRepresentation(resource, resourceTypes, resourceInterfaces);
                }
            }
        }
        else
        {
            // Resource is invalid
            std::cout << "Resource is invalid" << std::endl;
        }

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in foundResource: " << e.what() << std::endl;
    }
}

static FILE* client_open(const char* /*path*/, const char *mode)
{
    return fopen("./oic_svr_db_client.json", mode);
}

int main(int argc, char* argv[]) {

    std::ostringstream requestURI;
    OCPersistentStorage ps {client_open, fread, fwrite, fclose, unlink };

    // Create PlatformConfig object
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Both,
        "0.0.0.0",
        0,
        OC::QualityOfService::LowQos,
        &ps
    };

    OCPlatform::Configure(cfg);
    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all resources
        requestURI << OC_RSRVD_WELL_KNOWN_URI;// << "?rt=core.light";

        OCPlatform::findResource("", requestURI.str(),
                                 CT_DEFAULT, &foundResource);
        std::cout << "Finding Resource... " << std::endl;

        std::mutex blocker;
        std::condition_variable cv;
        std::unique_lock<std::mutex> lock(blocker);
        cv.wait(lock);

    } catch (OCException& e)
    {
        oclog() << "Exception in main: " << e.what();
    }

    return 0;
}


