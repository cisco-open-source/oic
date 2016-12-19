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
#ifndef __OC_R_DOOR
#define __OC_R_DOOR

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OC_R_DOOR = "oic.r.door";

/* Attributes */
const std::string OPEN_STATE = "openState";
const std::string OPEN_ALARM = "openAlarm";

class Door : public Resource
{
public:
    Door(std::string uri) : Door(uri, false) {}

    Door(std::string uri, bool secure) :
        Resource(uri,
                 OC_R_DOOR,
                 OC_IF_A,
                 secure)
    {
        m_rep.setValue<bool>(OPEN_ALARM, false);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_DOOR))
            return false;

        if (!checkInterface(rep, OC_IF_A))
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