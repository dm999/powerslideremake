#include <gtest/gtest.h>
#include "tools/PSSpline.h"

namespace{
    bool compareFloat(float a, float b, float tol = 1e-3)
    {
        return (fabs(a - b) < tol);
    }
}

namespace UT
{
    bool PSSpline_ComputePoint()
    {
        std::vector<float> points;
        points.push_back(1.0f);
        points.push_back(2.0f);
        points.push_back(3.0f);
        points.push_back(4.0f);
        points.push_back(5.0f);
        points.push_back(6.0f);
        points.push_back(7.0f);
        points.push_back(8.0f);
        points.push_back(9.0f);
        points.push_back(10.0f);
        points.push_back(11.0f);

        PSSpline spline;
        spline.init(points, 1.0f, 1.0f);

        return 
            compareFloat(spline.getPoint(1.5f), 2.5f)       &&
            compareFloat(spline.getPoint(10.5f), 11.5f)     &&
            compareFloat(spline.getPoint(20.6f), 21.6f)     &&
            compareFloat(spline.getPoint(-20.6f), 1.0f);
    }

    bool PSSpline_ComputePoint2()
    {
        std::vector<float> points;
        points.push_back(1.0f);
        points.push_back(2.0f);
        points.push_back(3.0f);
        points.push_back(4.0f);
        points.push_back(5.0f);
        points.push_back(6.0f);
        points.push_back(7.0f);
        points.push_back(8.0f);
        points.push_back(9.0f);
        points.push_back(10.0f);
        points.push_back(11.0f);

        PSSpline spline;
        spline.init(points, 10.0f, 1.0f);

        return 
            compareFloat(spline.getPoint(0.15f), 2.5f)      &&
            compareFloat(spline.getPoint(1.05f), 11.5f)     &&
            compareFloat(spline.getPoint(2.06f), 21.6f)     &&
            compareFloat(spline.getPoint(-2.06f), 1.0f);
    }

    bool PSSpline_ComputePoint3()
    {
        std::vector<float> points;
        points.push_back(1.0f);
        points.push_back(2.0f);
        points.push_back(3.0f);
        points.push_back(4.0f);
        points.push_back(5.0f);
        points.push_back(6.0f);
        points.push_back(7.0f);
        points.push_back(8.0f);
        points.push_back(9.0f);
        points.push_back(10.0f);
        points.push_back(11.0f);

        PSSpline spline;
        spline.init(points, 1.0f, 10.0f);

        return 
            compareFloat(spline.getPoint(1.5f), 25.0f)      &&
            compareFloat(spline.getPoint(10.5f), 115.0f)     &&
            compareFloat(spline.getPoint(20.6f), 216.0f)     &&
            compareFloat(spline.getPoint(-20.6f), 10.0f);
    }

    bool PSSpline_ComputePointRevsPower()
    {
        std::vector<float> points;
        points.push_back(0.550000f);
        points.push_back(0.625000f);
        points.push_back(0.700000f);
        points.push_back(0.775000f);
        points.push_back(0.850000f);
        points.push_back(0.925000f);
        points.push_back(1.000000f);
        points.push_back(0.882353f);
        points.push_back(0.571330f);
        points.push_back(0.223103f);
        points.push_back(0.001672f);

        PSSpline spline;
        spline.init(points, 0.001000f, 1.0f);

        const float idleRevsStart = 1000.0f;

        return 
            compareFloat(spline.getPoint(1000.0f - idleRevsStart), 0.55f)   &&
            compareFloat(spline.getPoint(7000.0f - idleRevsStart), 1.00f)   &&
            compareFloat(spline.getPoint(10000.0f - idleRevsStart), 0.223103f);
    }
}//UT

TEST(Common_PSSpline, PSSPline_ComputePoint)
{
    EXPECT_TRUE(UT::PSSpline_ComputePoint());
    EXPECT_TRUE(UT::PSSpline_ComputePoint2());
    EXPECT_TRUE(UT::PSSpline_ComputePoint3());
    EXPECT_TRUE(UT::PSSpline_ComputePointRevsPower());
}