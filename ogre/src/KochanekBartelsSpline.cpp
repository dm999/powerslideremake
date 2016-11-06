//---------------------------------------------------------------------------
#include "pcheader.h"

#include <cmath>

#include "KochanekBartelsSpline.h"

#include <cassert>

KochanekBartelsSpline::KochanekBartelsSpline(const std::vector<Ogre::Vector3> & points, float bias, float tension, float continuity)
    : mKeyPoints(points)
    , mBias(bias)
    , mTension(tension)
    , mContinuity(continuity)
    , mIsComputed(false)
{
}

void KochanekBartelsSpline::Precompute(float bias, float tension, float continuity)
{
    if(bias != -2)
    {
        mBias = bias;
    }

    if(tension != -2)
    {
        mTension = tension;
    }

    if(continuity != -2)
    {
        mContinuity = continuity;
    }

    ComputeDerivatives();
}

void KochanekBartelsSpline::ComputeDerivatives()
{
    mDerivativesA.clear();
    mDerivativesB.clear();

    mDerivativesA.push_back( Ogre::Vector3() );
    mDerivativesB.push_back( Ogre::Vector3() );

    for(unsigned int i = 1; i < mKeyPoints.size() - 1; i++)
    {
        int p = i - 1;
        int s = i + 1;

        Ogre::Vector3 g1 = (mKeyPoints[i] - mKeyPoints[p]) * (1.0f + mBias);
        Ogre::Vector3 g2 = (mKeyPoints[s] - mKeyPoints[i]) * (1.0f - mBias);
        Ogre::Vector3 g3 = g2 - g1;

        Ogre::Vector3 ra = (1.0f - mTension) * (g1 + 0.5f * g3 * (1.0f + mContinuity));
        Ogre::Vector3 rb = (1.0f - mTension) * (g1 + 0.5f * g3 * (1.0f - mContinuity));
        mDerivativesA.push_back(ra);
        mDerivativesB.push_back(rb);
    }

    if(mKeyPoints.size() == 2)
    {
        mDerivativesA.push_back( Ogre::Vector3() );
        mDerivativesB.push_back( Ogre::Vector3() );

        mDerivativesA[0] = mKeyPoints[1] - mKeyPoints[0];
        mDerivativesB[0] = (mKeyPoints[1] - mKeyPoints[0]) * (1.0f - mTension);

        mDerivativesA[1] = (mKeyPoints[1] - mKeyPoints[0]) * (1.0f - mTension);
        mDerivativesB[1] = mKeyPoints[1] - mKeyPoints[0];
    }
    else
    {
        Ogre::Vector3 firsta = mKeyPoints[1] - mKeyPoints[0];
        Ogre::Vector3 firstb = (1.5f * (mKeyPoints[1] - mKeyPoints[0]) - 0.5f * mDerivativesA[1]) * (1.0f - mTension);
        mDerivativesA[0] = firsta;
        mDerivativesB[0] = firstb;

        Ogre::Vector3 lasta  = (1.5f * (mKeyPoints[ mKeyPoints.size() - 1] - mKeyPoints[ mKeyPoints.size() - 2]) - 0.5f * mDerivativesB[mDerivativesB.size() - 1]) * (1.0f - mTension);
        Ogre::Vector3 lastb  = mKeyPoints[ mKeyPoints.size() - 1] - mKeyPoints[ mKeyPoints.size() - 2];
        mDerivativesA.push_back(lasta);
        mDerivativesB.push_back(lastb);
    }

    mIsComputed = true;
}

Ogre::Vector3 KochanekBartelsSpline::ComputePoint(float & t, size_t i, size_t j)
{
    if(t >= 0.0f && t <= 1.0f && false == mKeyPoints.empty())
    {
        if(false == mIsComputed)
        {
            ComputeDerivatives();
        }

        assert(i < mKeyPoints.size());
        assert(j < mKeyPoints.size());
        assert(i < mDerivativesA.size());
        assert(j < mDerivativesB.size());

        t = Ease(t, 0.0f, 0.0f);
        Ogre::Vector3 p1 = mKeyPoints[i];
        Ogre::Vector3 p2 = mKeyPoints[j];
        Ogre::Vector3 r1 = mDerivativesA[i];
        Ogre::Vector3 r2 = mDerivativesB[j];
        return p1 * (2.0f * std::pow(t, 3) - 3.0f * std::pow(t, 2) + 1.0f) + r1 * (std::pow(t, 3) - 2.0f * std::pow(t, 2) + t) + p2 * (-2.0f * std::pow(t, 3) + 3.0f * std::pow(t, 2)) + r2 * (std::pow(t, 3) - std::pow(t, 2));
    }
    return Ogre::Vector3();
}

Ogre::Vector3 KochanekBartelsSpline::ComputePoint(float t)
{
    if(t >= 0.0f && t <= 1.0f && false == mKeyPoints.empty())
    {
        if(false == mIsComputed)
        {
            ComputeDerivatives();
        }

        size_t pointsCount = mKeyPoints.size();
        size_t intervalsCount = pointsCount - 1;
        float intervalLength = 1.0f / static_cast<float>(intervalsCount);
        float startTime = 0.0f;
        float endTime = startTime + intervalLength;
        size_t i = 0;
        size_t j = 0;
        for(size_t q = 0; q < intervalsCount; ++q)
        {
            if(t >= startTime && t <= endTime)
            {
                i = q;
                j = q + 1;
                t = (t - startTime) / intervalLength;
                break;
            }

            startTime = endTime;
            endTime = startTime + intervalLength;
        }

        assert(i < mKeyPoints.size());
        assert(j < mKeyPoints.size());
        assert(i < mDerivativesA.size());
        assert(j < mDerivativesB.size());

        t = Ease(t, 0.0f, 0.0f);
        Ogre::Vector3 p1 = mKeyPoints[i];
        Ogre::Vector3 p2 = mKeyPoints[j];
        Ogre::Vector3 r1 = mDerivativesA[i];
        Ogre::Vector3 r2 = mDerivativesB[j];
        return p1 * (2.0f * std::pow(t, 3) - 3.0f * std::pow(t, 2) + 1.0f) + r1 * (std::pow(t, 3) - 2.0f * std::pow(t, 2) + t) + p2 * (-2.0f * std::pow(t, 3) + 3.0f * std::pow(t, 2)) + r2 * (std::pow(t, 3) - std::pow(t, 2));
    }
    return Ogre::Vector3();
}

float KochanekBartelsSpline::Ease(float t, float a, float b)
{
    float k;
    float s = a + b;

    if (s == 0.0f)
    {
        return t;
    }
    if (s > 1.0f)
    {
        a = a / s;
        b = b / s;
    }
    k = 1.0f / (2.0f - a - b);
    if (t < a)
    {
        return ((k / a) * t * t);
    }
    else
    {
        if (t < 1.0f - b)
        {
            return (k * (2 * t - a));
        }
        else
        {
            t = 1.0f - t;
            return (1.0f - (k / b) * t * t);
        }
    }
}