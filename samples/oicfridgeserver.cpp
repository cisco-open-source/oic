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
#include "ocpayload.h"
#include "Refrigeration.h"
#include "Temperature.h"
#include "BinarySwitch.h"
#include "Door.h"
#include "OpenLevel.h"
#include "Brightness.h"
#include "ColourChroma.h"
#include "Contact.h"
#include "Water.h"
#include "IceMaker.h"

using namespace OC;
/* Device Info */
const std::string DEVICE_NAME = "oic-fridge";

/* Platform Info */
const std::string DATE_OF_MANUFACTURE = "January 14th, 2015";
const std::string DEVICE_UUID = "";
const std::string FIRMWARE_VERSION = "15.1";
const std::string HARDWARE_VERSION = "myHardwareVersion";
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
const std::string OC_WK_D = "oic.wk.d";
const std::string OC_D_FRDG = "oic.d.refrigerator";
//oic_svr_db_server.dat
//oic_svr_db.dat
const std::string DAT_FILE = "oic_fridge_server.dat";
const std::string DAT_PATH  = getUserHome() + "/" + DAT_FILE;

#define TAG  "FRIDGE-SERVER"

class Fridge
{
public:
    Fridge() : Fridge(false)
    {}

    Fridge(bool secure) : m_power ("/sec/fridge/power", secure),
        m_ice_maker ("/sec/fridge/icemaker", secure),
        m_refr ("/sec/fridge/refrigeration", secure),
        m_cooler ("/sec/fridge/cooler/temp", secure),
        m_freezer("/sec/fridge/freezer/temp", secure),
        m_cooler_door("/sec/fridge/cooler/door", secure),
        m_freezer_door("/sec/fridge/freezer/door", secure),
        m_cv_door("/sec/fridge/cvroom/door", secure),
        m_open_level("/sec/fridge/openlevel", secure),
        m_brightness("/sec/fridge/light/1/brightness", secure),
        m_colour_chroma("/sec/fridge/light/1/chroma", secure),
        m_contact("/sec/fridge/sensor/contact", secure),
        m_water("/sec/fridge/sensor/water", secure)
    {}

private:
    BinarySwitch m_power;
    IceMaker m_ice_maker;
    Refrigeration m_refr;
    Temperature m_cooler, m_freezer;
    Door m_cooler_door, m_freezer_door, m_cv_door;
    OpenLevel m_open_level;
    Brightness m_brightness;
    ColourChroma m_colour_chroma;
    Contact m_contact;
    Water m_water;
};

FILE* server_fopen(const char *path, const char *mode)
{
    (void)path;
    const char* credFile = DAT_PATH.c_str();
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
    std::cout << "Usage : oicfridgeserver -ipv4 <0|1> -ipv6 <0|1> --highqos <0|1>" << std::endl;
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

    std::cout <<  "Start Fridge server ipv4 =  " << ipv4 << " ipv6 = " << ipv6 << " qos = "  << q << std::endl;

    if (!file_exist(DAT_PATH.c_str()))
    {
        std::cerr << "ERROR:: Missing JSON:" << DAT_PATH << std::endl;
#ifdef SECURE
        exit(-1);
 #endif
    }

    int result;
#ifdef SECURE
    OCPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };
    result = OCRegisterPersistentStorageHandler(&ps);

    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "OCRegisterPersistentStorageHandler Failed %d", result);
        return -1;
    }
#endif
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
#ifdef SECURE
        qos,
        &ps
#else
        qos
#endif
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
    OCResourcePayloadAddStringLL(&deviceInfo.types, duplicateStr(OC_D_FRDG));

    result = OCPlatform::registerDeviceInfo(deviceInfo);

    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "SetDeviceInfo Failed %d\n", result);
        return -1;
    }

    // 5-Create resources
#ifdef SECURE
    Fridge frg(true);
#else
    Fridge frg(false);
#endif

    // we will keep the server alive for at most 12 hours
    std::this_thread::sleep_for(std::chrono::hours(12));
    return 0;
}


