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
#ifndef __OC_R_BRIGHTNESS
#define __OC_R_BRIGHTNESS

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OC_R_BRIGHTNESS = "oic.r.light.brightness";

/* Attributes */
const std::string BRIGHTNESS = "brightness";

class Brightness : public Resource
{
public:
    Brightness(std::string uri) : Brightness(uri, false) {}

    Brightness(std::string uri, bool secure) :
        Resource(uri,
                 OC_R_BRIGHTNESS,
                 OC_IF_A,
                 secure)
    {
        m_rep.setValue<int>(BRIGHTNESS, 0);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_BRIGHTNESS))
            return false;

        if (!checkInterface(rep, OC_IF_A))
            return false;

        if (!checkAttr(rep, BRIGHTNESS, AttributeType::Integer, false))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        int nbrightness =  rep.getValue<int>(BRIGHTNESS);

       std::cout << m_rep.getUri() << " put new " << " BRIGHTNESS " << nbrightness << "/" << m_rep.getValue<bool>(BRIGHTNESS) << std::endl;

        if (nbrightness != m_rep.getValue<int>(BRIGHTNESS))
        {
            m_rep.setValue<int>(BRIGHTNESS, nbrightness);

            return 1;
        }
        return  0;
    }
};
#endif