
#include "PHYLoader.h"

#include "../tools/Conversions.h"

void PHYLoader::load(GameState& gameState) const
{
    gameState.getInitialVehicleSetup().clear();

    //The original game ships only mRaceGridCarsMax (12) per-track .phy start
    //records (car0..car11). For a normal race that is enough (11 AI + player).
    //Massacre builds a much larger field in batches of 11 AI + player, so the
    //slots beyond the 12 real records are cloned from the real ones with a
    //cumulative grid-row stagger (mTrackPosition[0][7] += 16.0f), placing each
    //extra batch one row behind the previous on the start grid. Without this the
    //loader would assert at q >= 12 — cloning is what makes >12 cars possible.
    std::vector<InitialVehicleSetup> initialVehicleSetupVec;

    for(int q = 0; q < (GameState::mAIMax + 1); ++q)//+ player
    {
        if(q < static_cast<int>(GameState::mRaceGridCarsMax))
        {
            Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()) + "/record", "car" + Conversions::DMToString(q) + ".phy");
            if(fileToLoad.get() && fileToLoad->isReadable())
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

                for(size_t qq = 0; qq < InitialVehicleSetup::mWheelsAmount; ++qq)
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
            else {assert(false && "No PHY file");}
        }
        else
        {
            //clone an existing record, offset one grid row back per batch so
            //each extra batch lines up behind the previous on the start grid.
            //q % mRaceGridCarsMax picks the source slot; the +16.0f accumulates
            //on the source entry (it is a reference) so batch k sits at +16*k.
            InitialVehicleSetup& initialVehicleSetup = initialVehicleSetupVec[q % GameState::mRaceGridCarsMax];
            initialVehicleSetup.mTrackPosition[0][7] += 16.0f;
            gameState.getInitialVehicleSetup().push_back(initialVehicleSetup);
        }
    }
}
