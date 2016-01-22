#ifndef __OIC_R_BRIGHTNESS
#define __OIC_R_BRIGHTNESS

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OIC_R_BRIGHTNESS = "oic.r.light.brightness";

/* Attributes */
const std::string BRIGHTNESS = "brightness";

class Brightness : public Resource
{
public:
    Brightness(std::string uri) : Brightness(uri, false) {}

    Brightness(std::string uri, bool secure) :
        Resource(uri,
                 OIC_R_BRIGHTNESS,
                 OIC_IF_A,
                 secure)
    {
        m_rep.setValue<int>(BRIGHTNESS, 0);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OIC_R_BRIGHTNESS))
            return false;

        if (!checkInterface(rep, OIC_IF_A))
            return false;

        if (!checkAttr(rep, BRIGHTNESS, AttributeType::Integer, false))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        int nbrightness =  rep.getValue<int>(BRIGHTNESS);

       std::cout << m_rep.getUri() << " put new " << " BRIGHTNESS " << nbrightness << "/" << m_rep.getValue<bool>(BRIGHTNESS) << std::endl;

        if (nbrightness != m_rep.getValue<int>(BRIGHTNESS))
        {
            m_rep.setValue<int>(BRIGHTNESS, nbrightness);

            return 1;
        }
        return  0;
    }
};
#endif