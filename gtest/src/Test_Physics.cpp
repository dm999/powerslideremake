#include <gtest/gtest.h>
#include "physics/PhysicsVehicle.h"

namespace{
    bool compareFloat(float a, float b, float tol = 1e-3)
    {
        return (fabs(a - b) < tol);
    }
}
#if 0
namespace UT
{
    bool Physics_Tangent()
    {
        Ogre::Vector3 tangent = PhysicsVehicle::findTangent(Ogre::Vector3(-0.0172738f, 0.99983734f, -0.0051829759f), Ogre::Vector3(0.0f, -6.7494035f, 0.0f));

        return 
            compareFloat(tangent.x, -0.11656889f)   &&
            compareFloat(tangent.y, -0.0021953583f) &&
            compareFloat(tangent.z, -0.034976307f);
    }
}//UT

TEST(Common_Physics, Physics_Tangent)
{
    EXPECT_TRUE(UT::Physics_Tangent());
}

#endif