#ifndef __OIC_R_CONTACT_H
#define __OIC_R_CONTACT_H

#include "Resource.h"

using namespace OC;
namespace PH = std::placeholders;

/* Resource types */
const std::string OIC_R_CONTACT = "oic.r.sensor.contact";

/* Attributes */
const std::string CONTACT_VAL = "value";


class Contact : public Resource
{
public:
    Contact(std::string uri) : Contact(uri, false) {}

    Contact(std::string uri, bool secure) :
        Resource(uri,
                 OIC_R_CONTACT,
                 OIC_IF_S,
                 secure)
    {
        m_rep.setValue<bool>(CONTACT_VAL, false);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OIC_R_CONTACT))
            return false;

        if (!checkInterface(rep, OIC_IF_S))
            return false;

        if (!checkAttr(rep, CONTACT_VAL, AttributeType::Boolean))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        bool b =  rep.getValue<bool>(CONTACT_VAL);

        if (b != m_rep.getValue<bool>(CONTACT_VAL))
        {
            m_rep.setValue<bool>(CONTACT_VAL, b);

            return 1;
        }

        return 0;
    }
};
#endif