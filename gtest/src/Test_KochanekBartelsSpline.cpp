#include <stdio.h>
#include <gtest/gtest.h>

#include "tools/KochanekBartelsSpline.h"

namespace UT
{
    bool Constructor()
    {
        KochanekBartelsSpline spline;

        const std::vector<Ogre::Vector3> & resPoints = spline.GetPoints();

        return resPoints.size() == 0;
    }

    bool Constructor2()
    {
        std::vector<Ogre::Vector3> points;
        points.push_back(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        points.push_back(Ogre::Vector3(2.0f, 2.0f, 0.0f));
        points.push_back(Ogre::Vector3(4.0f, 0.0f, 0.0f));

        KochanekBartelsSpline spline(points);

        const std::vector<Ogre::Vector3> & resPoints = spline.GetPoints();

        return resPoints.size() == 3;
    }

    bool ComputePoint()
    {
        std::vector<Ogre::Vector3> points;
        points.push_back(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        points.push_back(Ogre::Vector3(2.0f, 2.0f, 0.0f));
        points.push_back(Ogre::Vector3(4.0f, 0.0f, 0.0f));

        KochanekBartelsSpline spline(points);

        float t = 0.0f;
        Ogre::Vector3 resPoint = spline.ComputePoint(t, 0, 1);
        Ogre::Vector3 resPoint2 = spline.ComputePoint(t, 1, 2);

        return resPoint.x == 0.0f && resPoint.y == 0.0f  &&
               resPoint2.x == 2.0f && resPoint2.y == 2.0f;
    }

    bool ComputePoint2()
    {
        std::vector<Ogre::Vector3> points;
        points.push_back(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        points.push_back(Ogre::Vector3(2.0f, 2.0f, 0.0f));
        points.push_back(Ogre::Vector3(4.0f, 0.0f, 0.0f));

        KochanekBartelsSpline spline(points);

        Ogre::Vector3 resPoint = spline.ComputePoint(0.0f);
        Ogre::Vector3 resPoint2 = spline.ComputePoint(0.5f);
        Ogre::Vector3 resPoint3 = spline.ComputePoint(1.0f);

        return resPoint.x == 0.0f && resPoint.y == 0.0f      &&
               resPoint2.x == 2.0f && resPoint2.y == 2.0f    &&
               resPoint3.x == 4.0f && resPoint3.y == 0.0f;
    }

    bool ReplacePoints()
    {
        std::vector<Ogre::Vector3> points;
        points.push_back(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        points.push_back(Ogre::Vector3(2.0f, 2.0f, 0.0f));
        points.push_back(Ogre::Vector3(4.0f, 0.0f, 0.0f));

        KochanekBartelsSpline spline(points);

        points.push_back(Ogre::Vector3(8.0f, 0.0f, 0.0f));

        spline.ReplacePoints(points);

        const std::vector<Ogre::Vector3> & resPoints = spline.GetPoints();

        return resPoints.size() == 4;
    }

    bool ReplacePoints2()
    {
        std::vector<Ogre::Vector3> points;
        points.push_back(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        points.push_back(Ogre::Vector3(2.0f, 2.0f, 0.0f));
        points.push_back(Ogre::Vector3(4.0f, 0.0f, 0.0f));

        std::vector<Ogre::Vector3> points2;
        points2.push_back(Ogre::Vector3(2.0f, 2.0f, 0.0f));
        points2.push_back(Ogre::Vector3(4.0f, 4.0f, 0.0f));
        points2.push_back(Ogre::Vector3(8.0f, 0.0f, 0.0f));

        KochanekBartelsSpline spline(points);


        spline.ReplacePoints(points2);

        Ogre::Vector3 resPoint = spline.ComputePoint(0.0f);
        Ogre::Vector3 resPoint2 = spline.ComputePoint(0.5f);
        Ogre::Vector3 resPoint3 = spline.ComputePoint(1.0f);

        return resPoint.x == 2.0f && resPoint.y == 2.0f      &&
               resPoint2.x == 4.0f && resPoint2.y == 4.0f    &&
               resPoint3.x == 8.0f && resPoint3.y == 0.0f;
    }

} //END UT Namespace

TEST(Common_KochanekBartelsSpline, Constructor)
{
    EXPECT_TRUE(UT::Constructor());
    EXPECT_TRUE(UT::Constructor2());
}

TEST(Common_KochanekBartelsSpline, ComputePoint)
{
    EXPECT_TRUE(UT::ComputePoint());
    EXPECT_TRUE(UT::ComputePoint2());
}

TEST(Common_KochanekBartelsSpline, ReplacePoints)
{
    EXPECT_TRUE(UT::ReplacePoints());
    EXPECT_TRUE(UT::ReplacePoints2());
}