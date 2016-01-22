#ifndef __OIC_R_COLOURCHROMA
#define __OIC_R_COLOURCHROMA

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OIC_R_COLOURCHROMA = "oic.r.colour.chroma";

/* Attributes */
const std::string HUE = "hue";
const std::string SATURATION = "saturation";
const std::string COLOUR_SPACE_VALUE = "colourspacevalue";

class ColourChroma : public Resource
{
public:
    ColourChroma(std::string uri) : ColourChroma(uri, false) {}

    ColourChroma(std::string uri, bool secure) :
        Resource(uri,
                 OIC_R_COLOURCHROMA,
                 OIC_IF_A,
                 secure)
    {
        m_rep.setValue<int>(HUE, 0);
        m_rep.setValue<int>(SATURATION, 0);
        m_rep.setValue<string>(COLOUR_SPACE_VALUE, "CSV");
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OIC_R_COLOURCHROMA))
            return false;

        if (!checkInterface(rep, OIC_IF_A))
            return false;

        if (!checkAttr(rep, HUE, AttributeType::Integer))
            return false;

        if (!checkAttr(rep, SATURATION, AttributeType::Integer))
            return false;

        if (!checkAttr(rep, COLOUR_SPACE_VALUE, AttributeType::String))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        bool change = false;
        int nHue =  rep.getValue<int>(HUE);

        if (nHue != m_rep.getValue<int>(HUE))
        {
            m_rep.setValue<int>(HUE, nHue);

            change = true;
        }

        int nSat =  rep.getValue<int>(SATURATION);

        if (nSat != m_rep.getValue<int>(SATURATION))
        {
            m_rep.setValue<int>(SATURATION, nSat);

            change = true;
        }

        std::string nCsv =  rep.getValue<string>(COLOUR_SPACE_VALUE);

        if (nCsv != m_rep.getValue<string>(COLOUR_SPACE_VALUE))
        {
            m_rep.setValue<string>(COLOUR_SPACE_VALUE, nCsv);

            change = true;
        }
        return  change ? 1 : 0;
    }
};
#endif