#ifndef __OC_R_WATER_H
#define __OC_R_WATER_H

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OC_R_WATER = "oic.r.sensor.water";

/* Attributes */
const std::string WATER_VAL = "value";


class Water : public Resource
{
public:
    Water(std::string uri) : Water(uri, false) {}

    Water(std::string uri, bool secure) :
        Resource(uri,
                 OC_R_WATER,
                 OC_IF_S,
                 secure)
    {
        m_rep.setValue<bool>(WATER_VAL, false);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_WATER))
            return false;

        if (!checkInterface(rep, OC_IF_S))
            return false;

        if (!checkAttr(rep, WATER_VAL, AttributeType::Boolean))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        bool b =  rep.getValue<bool>(WATER_VAL);

        if (b != m_rep.getValue<bool>(WATER_VAL))
        {
            m_rep.setValue<bool>(WATER_VAL, b);

            return 1;
        }

        return 0;
    }
};
#endif