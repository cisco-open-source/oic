#ifndef __OC_R_MAINTENANCE
#define __OC_R_MAINTENANCE

#include <unistd.h>
#include <sys/reboot.h>
#include "Resource.h"

using namespace OC;

/* Resource uri */
const std::string MAINT_RSVD_URI = "/oic/mnt";

/* Resource types */
const std::string OC_R_MAINTENANCE = "oic.wk.mnt";

/* Attributes */
const std::string FR = "fr";
const std::string RB = "rb";
const std::string STATS = "ssc";

class Maintenance : public Resource
{
public:
    Maintenance() : Maintenance(false) {}

    Maintenance(bool secure) :
        Resource(MAINT_RSVD_URI,
                 OC_R_MAINTENANCE,
                 OC_IF_RW,
                 secure)
    {
        m_rep.setValue<bool>(FR, false);
        m_rep.setValue<bool>(RB, false);
        m_rep.setValue<bool>(STATS, false);
    }

protected:
    bool isValid(const OCRepresentation& rep)
    {
        if (!Resource::isValid(rep))
            return false;

        if (!checkType(rep, OC_R_MAINTENANCE))
            return false;

        if (!checkInterface(rep, OC_IF_RW))
            return false;

        if (!checkAttr(rep, FR, AttributeType::Boolean))
            return false;

        if (!checkAttr(rep, RB, AttributeType::Boolean))
            return false;

        if (!checkAttr(rep, STATS, AttributeType::Boolean))
            return false;

        return true;
    }

    int put(const OCRepresentation& rep)
    {
        if (Resource::put(rep) < 0)
            return -1;

        bool rb =  rep.getValue<bool>(RB);

        if (rb && rb != m_rep.getValue<bool>(RB))
        {
            m_rep.setValue<bool>(RB, rb);
            sync();
            reboot(RB_POWER_OFF);
            return 1;
        }

        return 0;
    }
};
#endif