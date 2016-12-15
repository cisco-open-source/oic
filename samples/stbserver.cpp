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

#include <chrono>
#include <iostream>
#include <thread>
#include <stdexcept>
#include <assert.h>

#include "oic_string.h"
#include "logger.h"
#ifdef SECURE
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "securevirtualresourcetypes.h"
#include "srmutility.h"
#include "pmtypes.h"
#endif
#include "OCPlatform.h"
#include "AudioResource.h"
#include "BinarySwitch.h"
#include "MediaSource.h"
#include "NightMode.h"
#include "Maintenance.h"


using namespace OC;
/* Device Info */
const std::string DEVICE_NAME = "Set-Top-Box";

/* Platform Info */
const std::string DATE_OF_MANUFACTURE = "January 14th, 2015";
const std::string DEVICE_UUID = "";
const std::string FIRMWARE_VERSION = "15.1";
const std::string HARDWARE_VERSION = "1.0";
const std::string MANUFACTURER_NAME = "Cisco";
const std::string MANUFACTURER_URL = "http://www.cisco.com";
const std::string MODEL_NUMBER = "7448_LLC";
const std::string OPERATING_SYSTEM_VERSION = "3.0.0";
const std::string PLATFORM_ID = "Tizen-IoTivity-Nexus";
const std::string PLATFORM_VERSION = "1.2";
const std::string SUPPORT_URL = "http://cisco.com";
const std::string SYSTEM_TIME = "";
const std::string VERSION = "1.0.1";

/* DEVICE types */
const std::string OC_WK_D = "oic.wk.d";
const std::string OC_D_TV = "oic.d.tv";
//oic_svr_db_server.dat
//oic_svr_db.dat
const std::string DAT_FILE = "oic_stb_server.dat";
const std::string DAT_PATH  = getUserHome() + "/" + DAT_FILE;

#define TAG  "STB-SERVER"
#define NIGHTMODE  0
#define MAINT  1

class SetTopBox
{
public:
    SetTopBox() : SetTopBox(false)
    {}

    SetTopBox(bool secure) : m_audio ("/stb/audio", secure),
        m_bswitch ("/stb/binaryswitch", secure),
        m_media ("/stb/media", getSources(), secure)
#if MAINT
        ,
        m_maint(secure)
#endif
#if NIGHTMODE
        ,
        m_nightmode("/stb/nightmode", secure)
#endif
    {}

private:
    AudioControl m_audio;
    BinarySwitch m_bswitch;
    MediaSourceList m_media;
#if MAINT
    Maintenance m_maint;
#endif
#if NIGHTMODE
    NightMode m_nightmode;
#endif
    std::vector<MediaSource> getSources()
    {
        std::vector<MediaSource> sources;
        MediaSource src = MediaSource("HDMI", 1, MSL_TYPE_AV, true);
        sources.push_back(src);
        return sources;
    }
};

FILE* server_fopen(const char *path, const char *mode)
{
    (void)path;
    const char* credFile = DAT_PATH.c_str();
    OIC_LOG_V(DEBUG, TAG, " ####################OPEN JSON DB %s", credFile);

    FILE* file = fopen(credFile, mode);

    if (file == NULL) {
        OIC_LOG_V(ERROR, TAG, "########################### FAIL OPEN DB file %s mode %s errno %d", credFile, mode, errno);
    }

    return file;
}

void PrintUsage()
{
    std::cout << "Usage : stbserver -ipv4 <0|1> -ipv6 <0|1> --highqos <0|1>" << std::endl;
}

int main(int argc, char* argv[])
{
    int ipv4 = 1;
    int ipv6 = 1;
    int q = 1;
    OC::QualityOfService qos = OC::QualityOfService::HighQos;

    if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
    {
        std::cout << "Print help" << std::endl;
        PrintUsage(); return -1;
    }

    for (int opt = 1; opt < argc - 1 ; opt++ )
    {
        std::cout << "opt nb = " << opt << " key = " <<  argv[opt] << " val = " << argv[opt + 1] << std::endl;

        if (!strcmp(argv[opt], "-ipv4") || !strcmp(argv[opt], "--ipv4"))
        {
            ipv4 = atoi(argv[opt + 1]);
            std::cout << "ipv4 passed = " << ipv4 << std::endl;
        }
        else if (!strcmp(argv[opt], "-ipv6") || !strcmp(argv[opt], "--ipv6"))
        {
            ipv6 = atoi(argv[opt + 1]);
            std::cout << "ipv6 passed = " << ipv6 << std::endl;
        }
        else if (!strcmp(argv[opt], "-highqos") || !strcmp(argv[opt], "--highqos") )
        {
            int q = atoi(argv[opt + 1]);
            std::cout << "qos passed = " << q << std::endl;

            if (q)
                qos = OC::QualityOfService::HighQos;
            else
                qos = OC::QualityOfService::LowQos;
        }
    }

    std::cout <<  "Start STB server ipv4 =  " << ipv4 << " ipv6 = " << ipv6 << " qos = "  << q << std::endl;

    if (!file_exist(DAT_PATH.c_str()))
    {
        std::cerr << "ERROR:: Missing JSON:" << DAT_PATH << std::endl;
        //exit(-1);
    }

    OCPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };
    int result = OCRegisterPersistentStorageHandler(&ps);

    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "OCRegisterPersistentStorageHandler Failed %d", result);
        return -1;
    }

    result = OCInit(NULL, 0, OC_SERVER);

    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "OCInit Failed %d", result);
        return 0;
    }

    // 2-Set Platform config
    int ctVal = CT_ADAPTER_IP;

    if (ipv4)
        ctVal |= CA_IPV4;

    if (ipv6)
        ctVal |= CA_IPV6;

#ifdef SECURE
        ctVal |= CA_SECURE;
#endif
    OCConnectivityType ct =  (OCConnectivityType) (ctVal);

    PlatformConfig cfg
    {
        ServiceType::InProc,
        ModeType::Server,
        "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
        0,         // Uses randomly available port
        qos,
        &ps
    };

    cfg.clientConnectivity = ct;
    cfg.serverConnectivity = ct;

    OCPlatform::Configure(cfg);

    // 3-Set Platform info
    const OCPlatformInfo platformInfo =
    {
        .platformID = duplicateStr(PLATFORM_ID),
        .manufacturerName = duplicateStr(MANUFACTURER_NAME),
        .manufacturerUrl = duplicateStr(MANUFACTURER_URL),
        .modelNumber = duplicateStr(MODEL_NUMBER),
        .dateOfManufacture = duplicateStr(DATE_OF_MANUFACTURE),
        .platformVersion = duplicateStr(PLATFORM_VERSION),
        .operatingSystemVersion = duplicateStr(OPERATING_SYSTEM_VERSION),
        .hardwareVersion = duplicateStr(HARDWARE_VERSION),
        .firmwareVersion = duplicateStr(FIRMWARE_VERSION),
        .supportUrl = duplicateStr(SUPPORT_URL),
        .systemTime = duplicateStr(SYSTEM_TIME)
    };

    result = OCPlatform::registerPlatformInfo(platformInfo);

    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "SetPlatformInfo Failed %d", result);
        return -1;
    }

    // 4-Set Device info
    OCDeviceInfo deviceInfo =
    {
        .deviceName = duplicateStr(DEVICE_NAME),
        .types = NULL
    };
    OCResourcePayloadAddStringLL(&deviceInfo.types, duplicateStr(OC_WK_D));
    OCResourcePayloadAddStringLL(&deviceInfo.types, duplicateStr(OC_D_TV));

    result = OCPlatform::registerDeviceInfo(deviceInfo);

    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "SetDeviceInfo Failed %d\n", result);
        return -1;
    }

    // 5-Create resources
#ifdef SECURE
    SetTopBox stb(true);
    std::cout << "Secure server" << std::endl;
#else
    SetTopBox stb(false);
    std::cout << "UnSecure server " << std::endl;
#endif
    // we will keep the server alive for at most 12 hours
    std::this_thread::sleep_for(std::chrono::hours(12));
    return 0;
}


