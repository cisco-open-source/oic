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
#ifndef __OC_R_OPEN_LEVEL
#define __OC_R_OPEN_LEVEL

#include "Resource.h"

using namespace OC;

/* Resource types */
const std::string OC_R_OPEN_LEVEL = "oic.r.openlevel";

/* Attributes */
const std::string OPEN_LEVEL = "openLevel";

class OpenLevel : public Resource
{
public:
    OpenLevel(std::string uri) : OpenLevel(uri, false) {}

    OpenLevel(std::string uri, bool secure) :
        Resource(uri,
                 OC_R_OPEN_LEVEL,
                 OC_IF_A,
                 secure)
    {
        m_rep.setValue<int>(OPEN_LEVEL, 0);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_OPEN_LEVEL))
            return false;

        if (!checkInterface(rep, OC_IF_A))
            return false;

        if (!checkAttr(rep, OPEN_LEVEL, AttributeType::Integer, false))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        int nOpenLevel =  rep.getValue<int>(OPEN_LEVEL);

       std::cout << m_rep.getUri() << " put new " << " OPEN_LEVEL " << nOpenLevel << "/" << m_rep.getValue<bool>(OPEN_LEVEL) << std::endl;

        if (nOpenLevel != m_rep.getValue<int>(OPEN_LEVEL))
        {
            m_rep.setValue<int>(OPEN_LEVEL, nOpenLevel);

            return 1;
        }
        return  0;
    }
};
#endif