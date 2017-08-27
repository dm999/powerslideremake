
#include "Tools.h"

#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

float Tools::randomSmallValue()
{
    float ret = 0.0f;

    ret = rand() * 0.000061037019f - 1.0f;

    return ret;
}

bool Tools::CheckSphereIntersectionWithTriangle(const Ogre::Vector3& sphereCenter, Ogre::Real sphereRadius, const OgreTriangle& triangle)
{
    //http://realtimecollisiondetection.net/blog/?p=103

    Ogre::Vector3 A = triangle.A - sphereCenter;
    Ogre::Vector3 B = triangle.B - sphereCenter;
    Ogre::Vector3 C = triangle.C - sphereCenter;

    Ogre::Real squaredRadius = sphereRadius * sphereRadius;

    Ogre::Vector3 AB(B - A);
    Ogre::Vector3 temp2(C - A);
    Ogre::Vector3 normal = AB.crossProduct(temp2);

    Ogre::Real d = A.dotProduct(normal);
    Ogre::Real e = normal.dotProduct(normal);

    bool sep1 = (d * d) > (squaredRadius * e);

    Ogre::Real aa = A.dotProduct(A);
    Ogre::Real ab = A.dotProduct(B);
    Ogre::Real ac = A.dotProduct(C);
    Ogre::Real bb = B.dotProduct(B);
    Ogre::Real bc = B.dotProduct(C);
    Ogre::Real cc = C.dotProduct(C);

    bool sep2 = (aa > squaredRadius) && (ab > aa) && (ac > aa);
    bool sep3 = (bb > squaredRadius) && (ab > bb) && (bc > bb);
    bool sep4 = (cc > squaredRadius) && (ac > cc) && (bc > cc);

    Ogre::Vector3 BC = (C - B);
    Ogre::Vector3 CA(A - C);

    Ogre::Real d1 = ab - aa;
    Ogre::Real d2 = bc - bb;
    Ogre::Real d3 = ac - cc;

    Ogre::Real e1 = AB.dotProduct(AB);
    Ogre::Real e2 = BC.dotProduct(BC);
    Ogre::Real e3 = CA.dotProduct(CA);

    Ogre::Vector3 Q1 = A * e1 - d1 * AB;
    Ogre::Vector3 Q2 = B * e2 - d2 * BC;
    Ogre::Vector3 Q3 = C * e3 - d3 * CA;
    Ogre::Vector3 QC = C * e1 - Q1;
    Ogre::Vector3 QA = A * e2 - Q2;
    Ogre::Vector3 QB = B * e3 - Q3;
    
    bool sep5 = (Q1.dotProduct(Q1) > (squaredRadius * e1 * e1)) && (Q1.dotProduct(QC) > 0.0f);
    bool sep6 = (Q2.dotProduct(Q2) > (squaredRadius * e2 * e2)) && (Q2.dotProduct(QA) > 0.0f);
    bool sep7 = (Q3.dotProduct(Q3) > (squaredRadius * e3 * e3)) && (Q3.dotProduct(QB) > 0.0f);

    return !sep1 && !sep2 && !sep3 && !sep4 && !sep5 && !sep6 && !sep7;
}

std::string Tools::SecondsToString(Ogre::Real timeSeconds)
{
    std::string res = "00:00.00";

    if(timeSeconds > 0.0f)
    {
        size_t timeInSeconds = static_cast<size_t>(timeSeconds);
        size_t timeInMilliseconds = static_cast<size_t>(timeSeconds * 100.0f);

        //hours = (timeInMilliseconds / 60 / 60) % 24;
        size_t minutes = (timeInSeconds / 60) % 60;
        size_t seconds = timeInSeconds % 60;
        size_t miliseconds = timeInMilliseconds % 100;
        char buf[256];
        sprintf(buf, "%02d:%02d.%02d", minutes, seconds, miliseconds);
        res = buf;
    }

    return res;
}

std::vector<std::string> Tools::splitpath(const std::string& str, const std::set<char> delimiters, bool insertEmpty)
{
    //http://stackoverflow.com/questions/8520560/get-a-file-name-from-a-path

    std::vector<std::string> result;

    char const* pch = str.c_str();
    char const* start = pch;
    for(; *pch; ++pch)
    {
        if (delimiters.find(*pch) != delimiters.end())
        {
            if (start != pch)
            {
                std::string str(start, pch);

                if(str.empty())
                {
                    if(insertEmpty)
                        result.push_back(str);
                }
                else result.push_back(str);

            }
            else
            {
                if(insertEmpty) result.push_back("");
            }
            start = pch + 1;
        }
    }

    std::string final = start;
    if(final.empty())
    {
        if(insertEmpty)
            result.push_back(final);
    }
    else result.push_back(final);

    return result;
}