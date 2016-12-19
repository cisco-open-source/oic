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
#ifndef __OC_R_ICE_MAKER_H
#define __OC_R_ICE_MAKER_H

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OC_R_ICE_MAKER = "oic.r.icemaker";

/* Attributes */
const std::string ICE_STATUS = "value";


class IceMaker : public Resource
{
public:
    IceMaker(std::string uri) : IceMaker(uri, false) {}

    IceMaker(std::string uri, bool secure) :
        Resource(uri,
                 OC_R_ICE_MAKER,
                 OC_IF_A,
                 secure)
    {
        m_rep.setValue<string>(ICE_STATUS, "full");
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_ICE_MAKER))
            return false;

        if (!checkInterface(rep, OC_IF_A))
            return false;

        if (!checkAttr(rep, ICE_STATUS, AttributeType::String))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        std::string b =  rep.getValue<string>(ICE_STATUS);

        if (b != m_rep.getValue<string>(ICE_STATUS))
        {
            m_rep.setValue<string>(ICE_STATUS, b);

            return 1;
        }

        return 0;
    }
};
#endif