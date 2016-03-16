#ifndef __OC_R_AIRFLOW
#define __OC_R_AIRFLOW

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OC_R_AIRFLOW = "oic.r.airflow";

/* Attributes */
const std::string SPEED = "speed";
const std::string DIRECTION = "direction";
const std::string AIR_RANGE = "range";

class AirFlow : public Resource
{
public:
    AirFlow(std::string uri) : AirFlow(uri, false) {}

    AirFlow(std::string uri, bool secure) :
        Resource(uri,
                 OC_R_AIRFLOW,
                 OC_IF_A,
                 secure)
    {
        m_rep.setValue<int>(SPEED, 0);
        m_rep.setValue<string>(DIRECTION, "left");
        m_rep.setValue<string>(AIR_RANGE, "0,120");
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_AIRFLOW))
            return false;

        if (!checkInterface(rep, OC_IF_A))
            return false;

        if (!checkAttr(rep, SPEED, AttributeType::Integer))
            return false;

        if (!checkAttr(rep, DIRECTION, AttributeType::String, false))
            return false;

        if (!checkAttr(rep, AIR_RANGE, AttributeType::String, false))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        int changed = 0;
        int nTemp =  rep.getValue<int>(SPEED);

        if (nTemp >= 0 && nTemp != m_rep.getValue<int>(SPEED))
        {
            m_rep.setValue<int>(TEMP, nTemp);

            changed =  1;
        }
        if (rep.hasAttribute(DIRECTION)) {
            string nDir =  rep.getValue<string>(DIRECTION);

            if (nDir == "left" || nDir == "right") {
               if (!m_rep.hasAttribute(DIRECTION) || m_rep.getValue<string>(DIRECTION) != nDir) {
                    m_rep.setValue<string>(DIRECTION, nDir);
                    changed = 1;
                }
            }
        }
        if (rep.hasAttribute(AIR_RANGE)) {
            string nRange =  rep.getValue<string>(AIR_RANGE);

            if (!m_rep.hasAttribute(AIR_RANGE) || m_rep.getValue<string>(AIR_RANGE) != nRange) {
                m_rep.setValue<string>(AIR_RANGE, nRange);
                changed = 1;
            }
        }

        return changed;
    }
};
#endif