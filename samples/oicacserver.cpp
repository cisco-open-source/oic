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
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "securevirtualresourcetypes.h"
#include "srmutility.h"
#include "pmtypes.h"
#include "OICPlatform.h"
#include "Mode.h"
#include "Temperature.h"
#include "BinarySwitch.h"
#include "AirFlow.h"

using namespace OIC;
/* Device Info */
const std::string DEVICE_NAME = "oic-aircond";

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
const std::string VERSION = "1.0.0";

/* DEVICE types */
const std::string OIC_WK_D = "oic.wk.d";
const std::string OIC_D_TV = "oic.d.tv";
//oic_svr_db_server.json
//oic_svr_db.json
const std::string JSON_FILE = "oic_ac_server.json";
const std::string JSON_PATH  = getUserHome() + "/" + JSON_FILE;

#define TAG  "Aircond-SERVER"

class Aircond
{
public:
    Aircond() : Aircond(false)
    {}

    Aircond(bool secure) : m_power ("/sec/aircon/power", secure),
        m_temp ("/sec/aircon/temperature", secure),
        m_airflow("/sec/aircon/airFlow", secure),
        m_mode("/sec/aircon/mode", secure)
    {}

private:
    BinarySwitch m_power;
    Temperature m_temp;
    AirFlow m_airflow;
    Modes m_mode;

};

FILE* server_fopen(const char *path, const char *mode)
{
    (void)path;
    const char* credFile = JSON_PATH.c_str();
    OIC_LOG_V(DEBUG, TAG, "############FOPEN ############### path %s mode %s EXISTS %d", credFile, mode, file_exist(credFile));

    assert(file_exist(credFile));

    FILE* file = fopen(credFile, mode);

    assert(file != NULL);

    if (file == NULL) {
        OIC_LOG_V(ERROR, TAG, "###########################");
        OIC_LOG_V(ERROR, TAG, "########################### FAIL OPEN DB file %s mode %s errno %d", credFile, mode, errno);
        OIC_LOG_V(ERROR, TAG, "###########################");
    }

    return file;
}

void PrintUsage()
{
    std::cout << "Usage : oicfridgeserver -ipv4 <0|1> -ipv6 <0|1> -secure <0|1> --highqos <0|1>" << std::endl;
}

int main(int argc, char* argv[])
{
    int ipv4 = 1;
    int ipv6 = 1;
    int secure = 1;
    int q = 1;
    OIC::QualityOfService qos = OIC::QualityOfService::HighQos;

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
        else if (!strcmp(argv[opt], "-secure") || !strcmp(argv[opt], "--secure") )
        {
            secure = atoi(argv[opt + 1]);
            std::cout << "secure passed = " << secure << std::endl;
        }
        else if (!strcmp(argv[opt], "-highqos") || !strcmp(argv[opt], "--highqos") )
        {
            int q = atoi(argv[opt + 1]);
            std::cout << "qos passed = " << q << std::endl;

            if (q)
                qos = OIC::QualityOfService::HighQos;
            else
                qos = OIC::QualityOfService::LowQos;
        }
    }

    std::cout <<  "Start Aircond server ipv4 =  " << ipv4 << " ipv6 = " << ipv6 << " secure = " << secure << " qos = "  << q << std::endl;

    if (!file_exist(JSON_PATH.c_str()))
    {
        std::cerr << "ERROR:: Missing JSON:" << JSON_PATH << std::endl;
        //exit(-1);
    }

    OICPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };
    int result = OICRegisterPersistentStorageHandler(&ps);

    if (result != OIC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "OICRegisterPersistentStorageHandler Failed %d", result);
        return -1;
    }

    result = OICInit(NULL, 0, OIC_SERVER);

    if (result != OIC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "OICInit Failed %d", result);
        return 0;
    }

    // 2-Set Platform config
    int ctVal = CT_ADAPTER_IP;

    if (ipv4)
        ctVal |= CA_IPV4;

    if (ipv6)
        ctVal |= CA_IPV6;

    if (secure)
        ctVal |= CA_SECURE;

    OICConnectivityType ct =  (OICConnectivityType) (ctVal);

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

    OICPlatform::Configure(cfg);

    // 3-Set Platform info
    const OICPlatformInfo platformInfo =
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

    result = OICPlatform::registerPlatformInfo(platformInfo);

    if (result != OIC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "SetPlatformInfo Failed %d", result);
        return -1;
    }

    // 4-Set Device info
    const OICDeviceInfo deviceInfo =
    {
        .deviceName = duplicateStr(DEVICE_NAME)
    };

    result = OICPlatform::registerDeviceInfo(deviceInfo);

    if (result != OIC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "SetDeviceInfo Failed %d\n", result);
        return -1;
    }

    // 5-Create resources
    bool sec = secure == 1;
    Aircond frg(sec);

    // we will keep the server alive for at most 12 hours
    std::this_thread::sleep_for(std::chrono::hours(12));
    return 0;
}


