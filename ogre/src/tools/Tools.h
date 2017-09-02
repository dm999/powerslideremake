
#ifndef TOOLS_H
#define TOOLS_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

struct AIData
{
    Ogre::Vector3 pos;
    Ogre::Vector3 tangent;
    Ogre::Vector3 magic;
};

struct AIWhole
{
    //slot
    std::vector<std::vector<float> > slotMatrix;
    std::vector<std::vector<float> > activation;
    Ogre::Vector3 multiplier;
    std::vector<std::vector<size_t> > remapper;

    //rec
    std::vector<AIData> aiData;
    size_t hackType;
    float hack1;
    float hack2;
};

struct Exclusion
{
    Ogre::Vector3 min;
    Ogre::Vector3 max;
    Ogre::Real falloff;
    Ogre::Real minExclusion;
    Ogre::Matrix4 transform;

    Ogre::Matrix4 ogreInverseTransform;

    Ogre::Real reciprocalFalloff;
};

struct LightEclusion
{
    std::vector<Exclusion> exclusions;
};

struct Particle
{
    size_t index;
    Ogre::ColourValue rgb1;
    Ogre::ColourValue rgb2;
    bool isVisible;
};

struct OgreTriangle
{
    Ogre::Vector3 A, B, C;
};

struct NormalAdjustmentHelper
{
    Ogre::Vector3 point;
    std::vector<Ogre::Vector3> normals;
    std::vector<float*> normalsOffset;

    bool operator == (const Ogre::Vector3& rhs)
    {
        return point == rhs;
    }
};

enum OverlapStatus
{
    DONT_OVERLAP,
    PARTLY_OVERLAP,
    FULLY_OVERLAP
};

class Tools
{
public:

    template <typename T>
    static T lerp(const T& src, const T& dst, float step)
    {
        T res;
        res = src + (dst - src) * step;
        return res;
    }

    template<typename T>
    static T herp3(const T& start, const T& end, float t)
    {
        T time = t * t * (static_cast<T>(3.0f) - static_cast<T>(2.0f) * t);
        return start + time * (end - start);
    }

    static Ogre::Real clamp(Ogre::Real n, Ogre::Real lower, Ogre::Real upper)
    {
        return std::max(lower, std::min(n, upper));
    }

    static bool CheckSphereIntersectionWithTriangle(const Ogre::Vector3& sphereCenter, Ogre::Real sphereRadius, const OgreTriangle& triangle);

    static std::string SecondsToString(Ogre::Real timeSeconds);

    static std::vector<std::string> splitpath(const std::string& str, const std::set<char> delimiters, bool insertEmpty = true);

    static void Sleep(size_t milliseconds)
    {
        SLEEP(milliseconds);
    }

private:
    Tools(){}
    Tools(const Tools&){}

};

#endif