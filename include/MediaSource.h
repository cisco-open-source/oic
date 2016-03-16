#ifndef __OC_R_MEDIA_INPUT_H
#define __OC_R_MEDIA_INPUT_H

#include "Resource.h"
#include "ocpayload.h"

using namespace OC;

/* Resource types */
const std::string OC_R_MEDIA_SOURCE_LIST = "oic.r.media.input";

/* Attributes */
const std::string SOURCES = "sources";
const std::string SRC_STATUS = "status";
const std::string SRC_NAME = "sourceName";
const std::string SRC_NB = "sourceNumber";
const std::string SRC_TYPE = "sourceType";
const std::string MSL_TYPE_A = "audioOnly";
const std::string MSL_TYPE_V = "videoOnly";
const std::string MSL_TYPE_AV = "audioPlusVideo";

class MediaSource
{
public:
    MediaSource(std::string name, int number, std::string type, bool status) :
        sourceName(name),
        sourceNumber(number),
        sourceType(type),
        status(status)
    {}

    std::string sourceName;
    int sourceNumber;
    std::string sourceType;
    bool status;
};

class MediaSourceList : public Resource
{
public:
    MediaSourceList(std::string uri,
                    std::vector<MediaSource> sources) :
        MediaSourceList(uri,
                        sources,
                        false) {}

    MediaSourceList(std::string uri,
                    std::vector<MediaSource> sources,
                    bool secure) :
        Resource(uri,
                 OC_R_MEDIA_SOURCE_LIST,
                 OC_IF_A,
                 secure)
    {
        OCRepresentation rep;
        rep.setValue<std::string>(SRC_NAME, sources[0].sourceName);
        rep.setValue<int>(SRC_NB, sources[0].sourceNumber);
        rep.setValue<std::string>(SRC_TYPE, sources[0].sourceType);
        rep.setValue<bool>(SRC_STATUS, sources[0].status);
        m_sources.push_back(rep);
        m_rep.setValue(SOURCES, m_sources);
    }
private:
    std::vector<OCRepresentation> m_sources;
protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_MEDIA_SOURCE_LIST))
            return false;

        if (!checkInterface(rep, OC_IF_A))
            return false;

        if (!checkAttr(rep, SOURCES, AttributeType::Vector))
            return false;

        std::vector<OCRepresentation> sources = rep.getValue<std::vector<OCRepresentation>>(SOURCES);

        for (const auto& src : sources)
        {
            for (const auto& cur : src)
            {
                std::string attrname = cur.attrname();

                if (!checkAttr(src, SRC_STATUS, AttributeType::Boolean))
                    return false;

                if (!checkAttr(src, SRC_NAME, AttributeType::String))
                    return false;

                if (!checkAttr(src, SRC_NB, AttributeType::Integer, false))
                    return false;

                if (!checkAttr(src, SRC_TYPE, AttributeType::String, false))
                    return false;
            }
        }
        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        const std::vector<OCRepresentation>& nv = rep[SOURCES];

        if (nv.empty())
            return 0;

        const OCRepresentation& noc = nv[0];
        OCRepresentation& moc = m_sources[0];
        std::string nName = noc[SRC_NAME];
        std::string mName = moc[SRC_NAME];

        bool nStatus = noc[SRC_STATUS];
        bool mStatus = moc[SRC_STATUS];

        std::cout << m_rep.getUri() << " before, mstatus " << mStatus << std::endl;
        std::cout << m_rep.getUri() << " before, nstatus " << nStatus << std::endl;

        if (nStatus != mStatus || nName != mName)
        {
            moc.setValue<bool>(SRC_STATUS, nStatus);
            moc.setValue<std::string>(SRC_NAME, nName);
            m_rep.setValue<std::vector<OCRepresentation>>(SOURCES, m_sources);

            return 1;
        }

        return 0;
    }
};
#endif
