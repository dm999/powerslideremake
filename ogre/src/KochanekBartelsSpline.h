//---------------------------------------------------------------------------

#ifndef KochanekBartelsSplineH
#define KochanekBartelsSplineH
//---------------------------------------------------------------------------

#include "OgreInclude.h"

#include <vector>

class KochanekBartelsSpline
{
    std::vector<Ogre::Vector3> mKeyPoints;
    std::vector<Ogre::Vector3> mDerivativesA;
    std::vector<Ogre::Vector3> mDerivativesB;
    float mBias;
    float mTension;
    float mContinuity;
    bool mIsComputed;


    void  ComputeDerivatives();

    float Ease(float t, float a, float b);

    KochanekBartelsSpline(const KochanekBartelsSpline&)
    {}

    KochanekBartelsSpline& operator=(const KochanekBartelsSpline&)
    {
        return *this;
    }

public:

    KochanekBartelsSpline()
        : mBias(0.0f)
        , mTension(0.0f)
        , mContinuity(0.0f)
        , mIsComputed(false)
    {};

    KochanekBartelsSpline(const std::vector<Ogre::Vector3> & points, float bias = 0, float tension = 0, float continuity = 0);

    void  ReplacePoints(const std::vector<Ogre::Vector3> & points)
    {
        mKeyPoints = points;
        mIsComputed = false;
    }

    void  Precompute(float bias = -2, float tension = -2, float continuity = -2);

    Ogre::Vector3 ComputePoint(float & t, size_t i, size_t j);

    Ogre::Vector3 ComputePoint(float t);

    const std::vector<Ogre::Vector3> & GetPoints() const
    {
        return mKeyPoints;
    }
};
#endif
