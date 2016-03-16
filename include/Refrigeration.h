#ifndef __OC_R_REFRIGERATION
#define __OC_R_REFRIGERATION

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OC_R_REFRIGERATION = "oic.r.refrigeration";

/* Attributes */
const std::string FILTER = "filter";
const std::string RAPID_FREEZE = "rapidFreeze";
const std::string RAPID_COOL = "rapidCool";
const std::string DEFROST = "defrost";


class Refrigeration : public Resource
{
public:
    Refrigeration(std::string uri) : Refrigeration(uri, false) {}

    Refrigeration(std::string uri, bool secure) :
        Resource(uri,
                 OC_R_REFRIGERATION,
                 OC_IF_A,
                 secure)
    {
        m_rep.setValue<double>(FILTER, 100);
        m_rep.setValue<bool>(RAPID_COOL, false);
        m_rep.setValue<bool>(RAPID_FREEZE, false);
        m_rep.setValue<bool>(DEFROST, false);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_REFRIGERATION))
            return false;

        if (!checkInterface(rep, OC_IF_A))
            return false;

        //Both mandatory
        if (!checkAttr(rep, FILTER, AttributeType::Integer) || !checkAttr(rep, DEFROST, AttributeType::Boolean))
            return false;

        if (!checkAttr(rep, RAPID_COOL, AttributeType::Boolean, false))
            return false;

        if (!checkAttr(rep, RAPID_FREEZE, AttributeType::Boolean, false))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        bool changed = false;
        double nFrost =  rep.getValue<bool>(DEFROST);

        if (nFrost != m_rep.getValue<bool>(DEFROST))
        {
            m_rep.setValue<bool>(DEFROST, nFrost);
            changed = true;
        }

        if (rep.hasAttribute(RAPID_FREEZE)) {
            bool nFreeze =  rep.getValue<bool>(RAPID_FREEZE);
            if (!m_rep.hasAttribute(RAPID_FREEZE) || m_rep.getValue<bool>(RAPID_FREEZE) != nFreeze) {
                m_rep.setValue<bool>(RAPID_FREEZE, nFreeze);
                changed = true;
            }
        }
        if (rep.hasAttribute(RAPID_COOL)) {
            bool nCool =  rep.getValue<bool>(RAPID_COOL);
            if (!m_rep.hasAttribute(RAPID_COOL) || m_rep.getValue<bool>(RAPID_COOL) != nCool) {
                m_rep.setValue<bool>(RAPID_COOL, nCool);
                changed = true;
            }
        }

        return changed ? 1 : 0;
    }
};
#endif