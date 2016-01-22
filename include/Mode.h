#ifndef __OIC_R_MODE
#define __OIC_R_MODE

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OIC_R_MODE = "oic.r.mode";

/* Attributes */
const std::string MODES = "modes";
const std::string SUPPORTED_MODES = "supportedModes";


class Modes : public Resource
{
public:
    Modes(std::string uri) : Modes(uri, false) {}

    Modes(std::string uri, bool secure) :
        Resource(uri,
                 OIC_R_MODE,
                 OIC_IF_A,
                 secure)
    {
        m_rep.setValue<string>(SUPPORTED_MODES, "home,away,quiet,sleep");
        m_rep.setValue<string>(MODES, "home");
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OIC_R_MODE))
            return false;

        if (!checkInterface(rep, OIC_IF_S))
            return false;

        if (!checkAttr(rep, MODES, AttributeType::String) || !checkAttr(rep, SUPPORTED_MODES, AttributeType::String))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        string nModes =  rep.getValue<string>(MODES);
        string oModes = m_rep.getValue<string>(MODES);
//TODO HE tokenize
        if (nModes != oModes)
        {
            m_rep.setValue<string>(MODES, nModes);

            return 1;
        }

        return 0;
    }
};
#endif