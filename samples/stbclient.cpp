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
#include "Maintenance.h"

using namespace OC;

std::shared_ptr<OCResource> curResource;

const std::string RSC_URI = "/oic/mnt";

void onPut(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{}

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

    if (resource->uri() == RSC_URI)
    {
        if (!curResource)
            curResource = resource;

        std::cout << "\t###########CHANGE RSC: " << std::endl;

        OCRepresentation rep;

        rep.setValue<bool>(RB, true);
        rep.setValue<bool>(FR, false);
        rep.setValue<bool>(STATS, false);
        curResource->post(rep, QueryParamsMap(), &onPut);
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

    OCPlatform::getDeviceInfo(host, OC_RSRVD_DEVICE_URI, CT_DEFAULT,
                                  &onGetDevice);
    OCPlatform::findResource("", OC_RSRVD_WELL_KNOWN_URI,
                                 CT_DEFAULT, &foundResource);

    std::mutex blocker;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(blocker);
    cv.wait(lock);

    return 0;
}
