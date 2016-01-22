#ifndef __OIC_R_SWITCH_BINARY_H
#define __OIC_R_SWITCH_BINARY_H

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OIC_R_BINARY_SWITCH = "oic.r.switch.binary";

/* Attributes */
const std::string VAL = "value";


class BinarySwitch : public Resource
{
public:
    BinarySwitch(std::string uri) : BinarySwitch(uri, false) {}

    BinarySwitch(std::string uri, bool secure) :
        Resource(uri,
                 OIC_R_BINARY_SWITCH,
                 OIC_IF_A,
                 secure)
    {
        m_rep.setValue<bool>(VAL, false);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OIC_R_BINARY_SWITCH))
            return false;

        if (!checkInterface(rep, OIC_IF_A))
            return false;

        if (!checkAttr(rep, VAL, AttributeType::Boolean))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        bool b =  rep.getValue<bool>(VAL);

        if (b != m_rep.getValue<bool>(VAL))
        {
            m_rep.setValue<bool>(VAL, b);

            return 1;
        }

        return 0;
    }
};
#endif