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
#ifndef __OC_R_ILLUMINANCE
#define __OC_R_ILLUMINANCE

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OC_R_ILLUMINANCE = "oic.r.sensor.illuminance";

/* Attributes */
const std::string ILLUMINANCE = "illuminance";


class Illuminance : public Resource
{
public:
    Illuminance(std::string uri) : Illuminance(uri, false) {}

    Illuminance(std::string uri, bool secure) :
        Resource(uri,
                 OC_R_ILLUMINANCE,
                 OC_IF_S,
                 secure)
    {
        m_rep.setValue<double>(ILLUMINANCE, 100);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_ILLUMINANCE))
            return false;

        if (!checkInterface(rep, OC_IF_S))
            return false;

        if (!checkAttr(rep, ILLUMINANCE, AttributeType::Double) && !checkAttr(rep, ILLUMINANCE, AttributeType::Integer))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        //Read only
        return -1;
    }
};
#endif