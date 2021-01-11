
#include "PHYLoader.h"

#include "../tools/Conversions.h"

void PHYLoader::load(GameState& gameState) const
{
    gameState.getInitialVehicleSetup().clear();

    std::vector<InitialVehicleSetup> initialVehicleSetupVec;

    for(int q = 0; q < (GameState::mAIMax + 1); ++q)//+ player
    {
        if (q < GameState::mRaceGridCarsMax)
        {
            Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()) + "/record", "car" + Conversions::DMToString(q) + ".phy");
            if (fileToLoad.get() && fileToLoad->isReadable())
            {
                InitialVehicleSetup initialVehicleSetup;

                Ogre::uint32 something;
                fileToLoad->read(&something, 4);

                Ogre::Vector3 rotX;
                Ogre::Vector3 rotY;
                Ogre::Vector3 rotZ;
                Ogre::Vector3 pos;
                Ogre::Vector3 impulseRot;
                Ogre::Vector3 impulseLinear;
                Ogre::Vector3 impulseRotInc;
                Ogre::Vector3 impulseLinearInc;

                fileToLoad->read(&rotX, 4 * 3);
                fileToLoad->read(&rotY, 4 * 3);
                fileToLoad->read(&rotZ, 4 * 3);
                fileToLoad->read(&pos, 4 * 3);
                fileToLoad->read(&impulseRot, 4 * 3);
                fileToLoad->read(&impulseLinear, 4 * 3);
                fileToLoad->read(&impulseRotInc, 4 * 3);
                fileToLoad->read(&impulseLinearInc, 4 * 3);

                Ogre::Real tmp;
                fileToLoad->read(&tmp, 4);
                fileToLoad->read(&tmp, 4);
                Ogre::uint32 gear;
                fileToLoad->read(&gear, 4);
                ++gear;

                for (size_t qq = 0; qq < InitialVehicleSetup::mWheelsAmount; ++qq)
                {
                    Ogre::Vector3 wheelData;
                    fileToLoad->read(&initialVehicleSetup.mSuspensionDataWheel[InitialVehicleSetup::mWheelsAmount - qq - 1], 4 * 3);//FL, FR, RL, RR
                    Ogre::Vector2 wheelDataTmp;
                    fileToLoad->read(&wheelDataTmp, 4 * 2);
                }

                Ogre::Matrix4 transform(
                    rotX.x, rotY.x, rotZ.x, pos.x,
                    rotX.y, rotY.y, rotZ.y, pos.y,
                    rotX.z, rotY.z, rotZ.z, -pos.z,
                    0.0f, 0.0f, 0.0f, 1.0f);

                initialVehicleSetup.mTrackPosition = transform;
                initialVehicleSetup.mInitialImpulseLinear = Ogre::Vector3(impulseLinear.x, impulseLinear.y, impulseLinear.z);
                initialVehicleSetup.mInitialImpulseLinearInc = Ogre::Vector3(impulseLinearInc.x, impulseLinearInc.y, impulseLinearInc.z);
                initialVehicleSetup.mInitialImpulseRot = Ogre::Vector3(impulseRot.x, impulseRot.y, impulseRot.z);
                initialVehicleSetup.mInitialImpulseRotInc = Ogre::Vector3(impulseRotInc.x, impulseRotInc.y, impulseRotInc.z);

                initialVehicleSetupVec.push_back(initialVehicleSetup);
                gameState.getInitialVehicleSetup().push_back(initialVehicleSetup);

                fileToLoad->close();
            }
            else { assert(false && "No PHY file"); }
        }
        else
        {
            InitialVehicleSetup& initialVehicleSetup = initialVehicleSetupVec[q % GameState::mRaceGridCarsMax];
            //initialVehicleSetupLast.mTrackPosition[0][3] += 3.0f;
            initialVehicleSetup.mTrackPosition[0][7] += 16.0f;
            //initialVehicleSetupLast.mTrackPosition[0][11] -= 10.0f;
            gameState.getInitialVehicleSetup().push_back(initialVehicleSetup);
        }
    }
}
