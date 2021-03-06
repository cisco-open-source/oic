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
#ifndef __OC_RESOURCE_H
#define __OC_RESOURCE_H

#include <iostream>
#include <thread>
#include <stdexcept>
#include <stdio.h>
#include <sstream>
#include "OCPlatform.h"
#include "OCApi.h"
#include "utils.h"
#include "cacommon.h"
#include "Observer.h"

using namespace OC;

namespace PH = std::placeholders;
/* Resource types */
const std::string OC_IF_BASE = "oic.if.baseline";
const std::string OC_IF_A = "oic.if.a";
const std::string OC_IF_S = "oic.if.s";
const std::string OC_IF_RW = "oic.if.rw";
const std::string OC_IF_R = "oic.if.r";

class Resource
{
public:
    const OCResourceHandle& getResourceHandle() const
    {
        return m_resourceHandle;
    }

    virtual OCRepresentation get()
    {
        return m_rep;
    }

protected:
    int change = 0;
    OCResourceHandle m_resourceHandle;
    OCRepresentation m_rep;
    ObservationIds m_observers;
    shared_ptr<IoTObserver> m_observerLoop;

    Resource(std::string uri,
             std::string rt):
        Resource(uri,
                 rt,
                 DEFAULT_INTERFACE) {}

    Resource(std::string uri,
             std::string rt,
             std::string rif):
        Resource(uri,
                 rt,
                 rif,
                 (uint8_t) (OC_DISCOVERABLE | OC_OBSERVABLE)) {}

    Resource(std::string uri,
             std::string rt,
             std::string rif,
             bool secure):
        Resource(uri,
                 rt,
                 rif,
                 secure ?
                 (uint8_t) (OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE) :
                 (uint8_t) (OC_DISCOVERABLE | OC_OBSERVABLE)) {}
    /**
     * @param uri the resource uri
     * @param rt the resource type
     * @param rif the resource interface
     * @param rp resource property
     */
    Resource(std::string uri,
             std::string rt,
             std::string rif,
             uint8_t rp)
    {
        EntityHandler cb = std::bind(&Resource::entityHandler, this, PH::_1);
        OCStackResult result = OCPlatform::registerResource(m_resourceHandle,
                               uri, rt, rif, cb, rp);

        if (OC_STACK_OK != result)
        {
            throw std::runtime_error(
                std::string(uri) + std::string(" Resource failed to start") + std::to_string(result));
        }

        m_rep.setUri(uri);
        addType(rt);
        addInterface(OC_IF_BASE);
        addInterface(rif);
        IoTObserverCb obsCb = bind(&Resource::observe, this);
        m_observerLoop = make_shared<IoTObserver>(obsCb);
    }

    void observe(void)
    {
        int changeLocal = change;
        usleep(1500000);
        std::cout << m_rep.getUri() << " change before " << changeLocal << " after " << change << std::endl;

        if (changeLocal != change)
        {
            std::cout << m_rep.getUri() << " Observer Callback" << std::endl;
            std::shared_ptr<OCResourceResponse> resourceResponse(new OCResourceResponse());
            resourceResponse->setErrorCode(200);
            resourceResponse->setResourceRepresentation(get(), "");
            OCStackResult result = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                   m_observers,
                                   resourceResponse);

            if (result == OC_STACK_NO_OBSERVERS)
            {
                std::cout << "No more observers..Stopping observer loop..." << std::endl;
                m_observerLoop->stop();
            }
        }
    }

    /** Return
        a) 0 if no change because identical,
        b) -1 if invalid,
        c) 1 if changed
    */
    virtual int put(const OCRepresentation& rep)
    {
        if (!isValid(rep))
            return -1;

        return 1;
    }

    virtual bool isValid(const OCRepresentation& rep)
    {
        for (const auto& cur : rep)
        {
            std::string attrname = cur.attrname();

            if (AttributeType::String != cur.type())
                continue;

            if (attrname != "href")
                continue;

            std::cout << "return false because \"href\" attribute" << std::endl;
            return false;
        }
        return true;
    }

    virtual OCEntityHandlerResult unsupported(std::string rt, std::shared_ptr<OCResourceResponse> response)
    {
        std::cout << m_rep.getUri() << "Unsupported request type" << rt << std::endl;
        if (response)
        {
            response->setResponseResult(OC_EH_ERROR);
            OCPlatform::sendResponse(response);
        }
        return OC_EH_ERROR;
    }

    virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OCResourceRequest> request)
    {
        if (!request)
        {
            std::cout << "In entity handler :INVALID request" << std::endl;

            return OC_EH_ERROR;
        }

        OCEntityHandlerResult ehResult = OC_EH_ERROR;
        std::string uri = m_rep.getUri();
        int rhf = request->getRequestHandlerFlag();
        std::string rt = request->getRequestType();
        std::cout << "In entity handler for:" << uri << " rhf = " << rhf << " rt = " << rt << std::endl;

        if (rhf & RequestHandlerFlag::RequestFlag)
        {
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());
            QueryParamsMap queries = request->getQueryParameters();
            std::string reqInterface = queries[OC::Key::INTERFACESKEY];

            if (rt == "GET")
            {
                std::cout << uri << " GET Request reqInterface " << reqInterface << std::endl;

                if (reqInterface.empty() || reqInterface == OC_IF_BASE)
                {
                    pResponse->setResourceRepresentation(get(), "");
                    ehResult = send(pResponse, OC_EH_OK);
                }
                else
                {
                    if (!checkInterface(get(), reqInterface))
                    {
                        std::cout << uri << " GET Request wrong reqInterface" << reqInterface << std::endl;
                        ehResult = send(pResponse, OC_EH_ERROR);
                    }
                    else
                    {
                        pResponse->setResourceRepresentation(get(), "");
                        ehResult = send(pResponse, OC_EH_OK);
                    }
                }
            }
            else if (rt == "POST")
            {
                std::cout << uri << " POST Request" << std::endl;
                int ret = put(request->getResourceRepresentation());

                if (ret < 0)
                {
                    ehResult = send(pResponse, OC_EH_ERROR);
                }
                else
                {
                    if (ret > 0)
                        ++change;

                    pResponse->setResourceRepresentation(get(), "");
                    ehResult = send(pResponse, OC_EH_OK);
                }
            }
            else return unsupported(rt, pResponse); //PUT DELETE not supported yet
        }

        if (rhf & RequestHandlerFlag::ObserverFlag)
        {
            std::cout << uri << " ObserveFlag " << std::endl;
            ObservationInfo obsInfo = request->getObservationInfo();
            ObserveAction obsAction = obsInfo.action;

            if (obsAction == ObserveAction::ObserveRegister)
            {
                std::cout << uri << " ObserveFlag ON" << std::endl;
                m_observers.push_back(obsInfo.obsId);
                m_observerLoop->start();
            }
            else if (obsAction == ObserveAction::ObserveUnregister )
            {
                std::cout << uri << "\tObserveFlag OFF" << std::endl;
                m_observers.erase(std::remove(
                                      m_observers.begin(),
                                      m_observers.end(),
                                      obsInfo.obsId),
                                  m_observers.end());

                if (m_observers.empty())
                    m_observerLoop->stop();
            }
            ehResult = OC_EH_OK;
        }
        return ehResult;
    }


    void addType(const std::string& type)
    {
        OCStackResult result = OCPlatform::bindTypeToResource(m_resourceHandle, type);

        if (OC_STACK_OK != result)
        {
            std::cerr << "Binding TypeName " << type << " to Resource was unsuccessful" << std::endl;
            return;
        }
        m_rep.addResourceType(type);
    }

    void addInterface(const std::string& interface)
    {
        OCStackResult result = OCPlatform::bindInterfaceToResource(m_resourceHandle, interface);

        if (OC_STACK_OK != result)
        {
            std::cerr << "Binding InterfaceName " << interface << " to Resource was unsuccessful" << std::endl;
            return;
        }
        m_rep.addResourceInterface(interface);
    }

    OCEntityHandlerResult send(const std::shared_ptr<OCResourceResponse> pResponse, OCEntityHandlerResult handlerRes)
    {
        pResponse->setResponseResult(handlerRes);

        if (handlerRes == OC_EH_OK)
            pResponse->setErrorCode(200);

        if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
            return handlerRes;

        return OC_EH_ERROR;
    }
};
#endif
