#include <gtest/gtest.h>

#include "OgreTools.h"

namespace UT
{
    TEST(LineAndPoint, CheckLine)
    {
        Ogre::Vector3 point(12.0f, 10.0f, 0.0f);

        Ogre::Vector3 A(0.0f, 0.0f, 0.0f);
        Ogre::Vector3 B(10.0f, 0.0f, 0.0f);

        Ogre::Vector3 res = ProjectPointOnLine(point, A, B);

        EXPECT_EQ(res.x, 10.0f);
        EXPECT_EQ(res.y, 0.0f);
        EXPECT_EQ(res.z, 0.0f);
    }

    TEST(LineAndPoint, CheckLine2)
    {
        Ogre::Vector3 point(-12.0f, 10.0f, 0.0f);

        Ogre::Vector3 A(0.0f, 0.0f, 0.0f);
        Ogre::Vector3 B(10.0f, 0.0f, 0.0f);

        Ogre::Vector3 res = ProjectPointOnLine(point, A, B);

        EXPECT_EQ(res.x, 0.0f);
        EXPECT_EQ(res.y, 0.0f);
        EXPECT_EQ(res.z, 0.0f);
    }

    TEST(LineAndPoint, CheckLine3)
    {
        Ogre::Vector3 point(12.0f, 10.0f, 0.0f);

        Ogre::Vector3 A(0.0f, 0.0f, 0.0f);
        Ogre::Vector3 B(10.0f, 0.0f, 0.0f);

        Ogre::Vector3 res = NearestPoint(A, B, point);

        EXPECT_EQ(res.x, 12.0f);
        EXPECT_EQ(res.y, 0.0f);
        EXPECT_EQ(res.z, 0.0f);
    }

    TEST(LineAndPoint, CheckLine4)
    {
        Ogre::Vector3 point(-12.0f, 10.0f, 0.0f);

        Ogre::Vector3 A(0.0f, 0.0f, 0.0f);
        Ogre::Vector3 B(10.0f, 0.0f, 0.0f);

        Ogre::Vector3 res = NearestPoint(A, B, point);

        EXPECT_EQ(res.x, -12.0f);
        EXPECT_EQ(res.y, 0.0f);
        EXPECT_EQ(res.z, 0.0f);
    }
}