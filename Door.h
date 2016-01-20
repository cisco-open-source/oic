#ifndef __OIC_R_DOOR
#define __OIC_R_DOOR

#include "Resource.h"

using namespace OC;
namespace PH = std::placeholders;

/* Resource types */
const std::string OIC_R_DOOR = "oic.r.door";

/* Attributes */
const std::string OPEN_STATE = "openState";
const std::string OPEN_ALARM = "openAlarm";

class Door : public Resource
{
public:
    Door(std::string uri) : Door(uri, false) {}

    Door(std::string uri, bool secure) :
        Resource(uri,
                 OIC_R_DOOR,
                 OIC_IF_A,
                 secure)
    {
        m_rep.setValue<bool>(OPEN_ALARM, false);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OIC_R_DOOR))
            return false;

        if (!checkInterface(rep, OIC_IF_A))
            return false;

        if (!checkAttr(rep, OPEN_STATE, AttributeType::String))
            return false;

        if (!checkAttr(rep, OPEN_ALARM, AttributeType::Boolean, false))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        bool nAlarm =  rep.getValue<bool>(OPEN_ALARM);

        std::cout << m_rep.getUri() << " put new " << " OPEN_ALARM " << nAlarm << "/" << m_rep.getValue<bool>(OPEN_ALARM) << std::endl;

        if (nAlarm != m_rep.getValue<bool>(OPEN_ALARM))
        {
            m_rep.setValue<bool>(OPEN_ALARM, nAlarm);

            return 1;
        }
        return  0;
    }
};
#endif