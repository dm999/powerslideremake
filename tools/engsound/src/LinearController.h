#ifndef LINEARCONTROLLER_H
#define LINEARCONTROLLER_H

#include <vector>

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

private:
    Tools(){}
    Tools(const Tools&){}

};

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