#include "PSSpline.h"

#include <cassert>
#include <cmath>

//0041A550

void PSSpline::init(const std::vector<float>& values, float hScale, float vScale)
{
    assert(values.size() == 11);
    mValues = values;
    mVScale = vScale;
    mHScale = hScale;
}

float PSSpline::getPoint(float val)
{
    float res = 0.0f;

    if(!mValues.empty())
    {
        val *= mHScale;

        double integer;
        float frac = static_cast<float>(modf(val, &integer));
        size_t index = static_cast<size_t>(integer);

        if(integer < 0.0)
        {
            index = 0;
            frac = 0.0f;
        }

        if(index < mValues.size() - 1)
        {
            res = ((1.0f - frac) * mValues[index] + frac * mValues[index + 1]) * mVScale;
        }
        else
        {
            res =   (
                        (mValues[mValues.size() - 1] - mValues[mValues.size() - 2]) *
                        ((index - mValues.size() + 1) + frac) + 
                        mValues[mValues.size() - 1]
                    ) * mVScale;

        }
    }

    return res;
}