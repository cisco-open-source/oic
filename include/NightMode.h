#ifndef __OC_R_NIGHT_MODE_H
#define __OC_R_NIGHT_MODE_H

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OC_R_NIGHT_MODE = "oic.r.nightmode";

/* Attributes */
const std::string NM = "nightMode";


class NightMode : public Resource
{
public:
    NightMode(std::string uri) : NightMode(uri, false) {}

    NightMode(std::string uri, bool secure) :
        Resource(uri,
                 OC_R_NIGHT_MODE,
                 OC_IF_A,
                 secure)
    {
        m_rep.setValue<bool>(NM, false);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_NIGHT_MODE))
            return false;

        if (!checkInterface(rep, OC_IF_A))
            return false;

        if (!checkAttr(rep, NM, AttributeType::Boolean))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        bool b =  rep.getValue<bool>(NM);

        if (b != m_rep.getValue<bool>(NM))
        {
            m_rep.setValue<bool>(NM, b);

            return 1;
        }

        return 0;
    }
};
#endif