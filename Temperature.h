#ifndef __OIC_R_TEMPERATURE
#define __OIC_R_TEMPERATURE

#include "Resource.h"

using namespace OC;
namespace PH = std::placeholders;

/* Resource types */
const std::string OIC_R_TEMPERATURE = "oic.r.temperature";

/* Attributes */
const std::string TEMP = "temperature";
const std::string UNITS = "units";
const std::string TEMP_RANGE = "range";
const std::string UNIT_C = "C";
const std::string UNIT_F = "F";
const std::string UNIT_K = "K";


class Temperature : public Resource
{
public:
    Temperature(std::string uri) : Temperature(uri, false) {}

    Temperature(std::string uri, bool secure) :
        Resource(uri,
                 OIC_R_TEMPERATURE,
                 OIC_IF_A,
                 secure)
    {
        m_rep.setValue<double>(TEMP, 0.0);
        m_rep.setValue<string>(UNITS, UNIT_C);
        m_rep.setValue<string>(TEMP_RANGE, "-20,120");
        addInterface(OIC_IF_S);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OIC_R_TEMPERATURE))
            return false;

        if (!checkInterface(rep, OIC_IF_A))
            return false;

        if (!checkAttr(rep, TEMP, AttributeType::Integer) && !checkAttr(rep, TEMP, AttributeType::Double))
            return false;

        if (!checkAttr(rep, UNITS, AttributeType::String, false))
            return false;

        if (!checkAttr(rep, TEMP_RANGE, AttributeType::String, false))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        double dTemp;
        bool isDouble = rep.getValue<double>(TEMP, dTemp);
        std::cout << " value is double ? " << TEMP << " = " << isDouble << std::endl;

        if (isDouble) {
            if (dTemp != m_rep.getValue<double>(TEMP))
            {
                m_rep.setValue<double>(TEMP, dTemp);

                return 1;
            }
        }
        else {
            int iTemp;
            bool isInt = rep.getValue<int>(TEMP, iTemp);
            std::cout << " value is int ? " << TEMP << " = " << isInt << std::endl;

            if (isInt) {
                if (iTemp != m_rep.getValue<int>(TEMP))
                {
                    m_rep.setValue<int>(TEMP, iTemp);

                    return 1;
                }
            }
        }
        return 0;
    }
};
#endif