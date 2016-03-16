#ifndef __OC_R_AUDIO_H
#define __OC_R_AUDIO_H

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OC_R_AUDIO = "oic.r.audio";

/* Attributes */
const std::string VOL = "volume";
const std::string MUTE = "mute";

class AudioControl : public Resource
{
public:
    AudioControl(std::string uri) : AudioControl(uri, false) {}

    AudioControl(std::string uri, bool secure) :
        Resource(uri,
                 OC_R_AUDIO,
                 OC_IF_A,
                 secure)
    {
        m_rep.setValue<bool>(MUTE, true);
        m_rep.setValue<int>(VOL, 0);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_AUDIO))
            return false;

        if (!checkInterface(rep, OC_IF_A))
            return false;

        //both mandatory
        if (!checkAttr(rep, VOL, AttributeType::Integer) || !checkAttr(rep, MUTE, AttributeType::Boolean))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        int changed = 0;

        bool b = rep.getValue<bool>(MUTE);
        if (b != m_rep.getValue<bool>(MUTE))
        {
            m_rep.setValue(MUTE, b);
            changed = 1;
        }

        int v = std::max(0, rep.getValue<int>(VOL));
        if (v >= 0 && v != m_rep.getValue<int>(VOL))
        {
            m_rep.setValue(VOL, v);
            changed = 1;
        }

        return changed;
    }
};
#endif