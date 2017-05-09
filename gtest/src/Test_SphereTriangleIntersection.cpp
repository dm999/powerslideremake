#include <gtest/gtest.h>

#include "tools/Tools.h"

namespace UT
{
    TEST(SphereTriangleIntersection, CheckOne)
    {
        OgreTriangle tri;
        tri.A = Ogre::Vector3(11.0f, 0.0f, 0.0f);
        tri.B = Ogre::Vector3(15.0f, 0.0f, 0.0f);
        tri.C = Ogre::Vector3(12.0f, 10.0f, 0.0f);
        EXPECT_EQ(false, Tools::CheckSphereIntersectionWithTriangle(Ogre::Vector3(0.0f, 0.0f, 0.0f), 10.0f, tri));

        tri.A = Ogre::Vector3(9.5f, 0.0f, 0.0f);
        tri.B = Ogre::Vector3(15.0f, 0.0f, 0.0f);
        tri.C = Ogre::Vector3(6.0f, 10.0f, 0.0f);
        EXPECT_EQ(true, Tools::CheckSphereIntersectionWithTriangle(Ogre::Vector3(0.0f, 0.0f, 0.0f), 10.0f, tri));

        tri.A = Ogre::Vector3(-9.5f, 0.0f, 0.0f);
        tri.B = Ogre::Vector3(0.0f, 9.5f, 0.0f);
        tri.C = Ogre::Vector3(9.5f, 0.0f, 0.0f);
        EXPECT_EQ(true, Tools::CheckSphereIntersectionWithTriangle(Ogre::Vector3(0.0f, 0.0f, 0.0f), 10.0f, tri));

        tri.A = Ogre::Vector3(-90.5f, 0.0f, 0.0f);
        tri.B = Ogre::Vector3(90.5f, 0.0f, 0.0f);
        tri.C = Ogre::Vector3(0.0f, 90.5f, 0.0f);
        EXPECT_EQ(true, Tools::CheckSphereIntersectionWithTriangle(Ogre::Vector3(0.0f, 0.0f, 0.0f), 10.0f, tri));

        tri.A = Ogre::Vector3(-9.5f, 0.0f, 11.0f);
        tri.B = Ogre::Vector3(9.5f, 0.0f, 11.0f);
        tri.C = Ogre::Vector3(0.0f, 9.5f, 11.0f);
        EXPECT_EQ(false, Tools::CheckSphereIntersectionWithTriangle(Ogre::Vector3(0.0f, 0.0f, 0.0f), 10.0f, tri));
    }
}