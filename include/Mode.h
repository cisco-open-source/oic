/****************************************************************************
**
** Copyright © 1992-2016 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
**
** $CISCO_BEGIN_LICENSE:APACHE$
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** http://www.apache.org/licenses/LICENSE-2.0
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/
#ifndef __OC_R_MODE
#define __OC_R_MODE

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OC_R_MODE = "oic.r.mode";

/* Attributes */
const std::string MODES = "modes";
const std::string SUPPORTED_MODES = "supportedModes";


class Modes : public Resource
{
public:
    Modes(std::string uri) : Modes(uri, false) {}

    Modes(std::string uri, bool secure) :
        Resource(uri,
                 OC_R_MODE,
                 OC_IF_A,
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

        if (!checkType(rep, OC_R_MODE))
            return false;

        if (!checkInterface(rep, OC_IF_S))
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