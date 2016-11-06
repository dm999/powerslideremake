#ifndef LINEARCONTROLLER_H
#define LINEARCONTROLLER_H

#include <vector>

#include "Tools.h"

template <typename T>
class LinearController
{
public:

    void addPoint(const T& controlPoint, const T& val)
    {
        mControlPoints.push_back(controlPoint);
        mValues.push_back(val);
    }

    T getVal(const T& point) const
    {


        T res = static_cast<T>(0);

        if(!mControlPoints.empty())
        {
            if(point <= mControlPoints[0])
            {
                res = mValues[0];
            }
            else if(point >= mControlPoints[mControlPoints.size() - 1])
            {
                res = mValues[mValues.size() - 1];
            }
            else
            {
                for(size_t q = 0; q < mControlPoints.size() - 1; ++q)
                {
                    if(point >= mControlPoints[q] && point < mControlPoints[q + 1])
                    {
                        T distance = mControlPoints[q + 1] - mControlPoints[q];
                        T offset = point - mControlPoints[q];
                        float step = offset / distance;
                        res = Tools::lerp(mValues[q], mValues[q + 1], step);
                        break;
                    }
                }
            }
        }

        return res;
    }

private:
    std::vector<T> mControlPoints;
    std::vector<T> mValues;
};

#endif