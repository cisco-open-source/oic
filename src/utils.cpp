#include "utils.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

std::string getUserHome()
{
    char *p = getenv("HOME");
    std::string ret;

    if (p == NULL)
        ret = "";
    else
        ret =  std::string(p);

    return ret;
}

bool file_exist(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

char* duplicateStr(const std::string sourceString)
{
    return strdup(sourceString.c_str());
}

void printOCResource(const OCResource& oCResource)
{
    printf( "PrintfOcResource\n");
    printf( "\tRes[sId] = %s\n", oCResource.sid().c_str());
    printf( "\tRes[Uri] = %s\n", oCResource.uri().c_str());
    printf( "\tRes[Host] = %s\n", oCResource.host().c_str());
    printf( "\tRes[Resource types]:\n");

    for (const auto& resourceTypes : oCResource.getResourceTypes()) {
        printf( "\t\t%s\n", resourceTypes.c_str());
    }

    printf( "Res[Resource interfaces] \n");

    for (const auto& resourceInterfaces : oCResource.getResourceInterfaces()) {
        printf( "\t\t%s\n", resourceInterfaces.c_str());
    }
}

void printOCRep(const OCRepresentation &oCRepr)
{
    printf(">>printOCRep:\n");

    std::string uri = oCRepr.getUri();
    printf("\turi=%s\n", uri.c_str());

    printf("\thost=%s\n", oCRepr.getHost().c_str());

    printf("\ttypes\n");
    for (const auto& resourceTypes : oCRepr.getResourceTypes()) {
        printf("\t\t%s\n", resourceTypes.c_str());
    }

    printf("\tinterfaces\n");
    for (const auto& resourceInterfaces : oCRepr.getResourceInterfaces()) {
        printf("\t\t%s\n", resourceInterfaces.c_str());
    }

    for (const auto& cur : oCRepr) {
        std::string attrname = cur.attrname();
        printf("\tattr  key=%s\n", attrname.c_str());

        if (AttributeType::String == cur.type()) {
            std::string curStr = cur.getValue<std::string>();
            printf("\tRep[String]: key=%s, value=%s\n", attrname.c_str(),
                   curStr.c_str());
        } else if (AttributeType::Integer == cur.type()) {
            printf("\tRep[Integer]: key=%s, value=%d\n", attrname.c_str(),
                   cur.getValue<int>());
        } else if (AttributeType::Double == cur.type()) {
            printf("\tRep[Double]: key=%s, value=%f\n", attrname.c_str(),
                   cur.getValue<double>());
        } else if (AttributeType::Boolean == cur.type()) {
            printf("\tRep[Boolean]: key=%s, value=%d\n", attrname.c_str(),
                   cur.getValue<bool>());
        } else if (AttributeType::OCRepresentation == cur.type()) {
            printf("\tRep[OCRepresentation]: key=%s, value=%s\n",
                   attrname.c_str(), "OCRep");
            printOCRep(cur.getValue<OCRepresentation>());
        } else if (AttributeType::Vector == cur.type()) {
            printf("\tRep[OCRepresentation]: key=%s, value=%s\n",
                   attrname.c_str(), "Vector");
            if (cur.base_type() == AttributeType::OCRepresentation) {
                printf("\tVector of OCRepresentation\n");

                std::vector<OCRepresentation> v =
                    cur.getValue<std::vector<OCRepresentation>>();
                for (const auto& curv : v) {
                    printf("\t>>Print sub OCRepresentation\n");
                    printOCRep(curv);
                    printf("\t<<Print sub OCRepresentation\n");
                }
            }
        }
    }
    printf("<<printOCRep:\n");
}

bool checkType(const OCRepresentation &rep, const std::string requiredType)
{
    /*
    std::vector<std::string> v = rep.getResourceTypes();

    if (std::find(v.begin(), v.end(), requiredType) == v.end())
    {
        std::cout << rep.getUri() << " invalid because missing type " << requiredType << std::endl;
        return false;
    }*/
    return true;
}

bool checkInterface(const OCRepresentation &rep, const std::string requiredInt) {
    /* std::vector<std::string> v = rep.getResourceInterfaces();
     if (std::find(v.begin(), v.end(), requiredInt) == v.end())
     {
         std::cout << rep.getUri() << " invalid because missing interface " << requiredInt << std::endl;
         return false;
     }*/
    return true;
}

bool checkAttr(const OCRepresentation &rep, const std::string requiredName, const AttributeType requiredType) {
    return checkAttr(rep, requiredName, requiredType, true);
}

bool checkAttr(const OCRepresentation &rep, const std::string requiredName, const AttributeType requiredType, bool mandatory) {
    if (!rep.hasAttribute(requiredName))
    {
        if (mandatory) {
            std::cout << rep.getUri() << " invalid because missing attribute " << requiredName << std::endl;
            return false;
        }
        return true;
    }

    for (const auto& cur : rep)
    {
        std::string attrname = cur.attrname();

        if (attrname == requiredName && requiredType != cur.type())
        {
            std::cout << rep.getUri() << " invalid because " << attrname << " has wrong type:" << cur.type() << " instead of " << requiredType << std::endl;
            return false;
        }
    }

    return true;
}


