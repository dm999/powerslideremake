
#include "PHYLoader.h"

#include "../tools/Conversions.h"

void PHYLoader::load(GameState& gameState) const
{
    gameState.getTrackPositions().clear();
    gameState.getTrackOriginalImpulseLinear().clear();

    for(int q = 0; q < (GameState::mAIMax + 1); ++q)
    {
        Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()) + "/record", "car" + Conversions::DMToString(q) + ".phy");
        if(fileToLoad.get() && fileToLoad->isReadable())
        {
            typedef unsigned int DWORD;

            DWORD something;
            fileToLoad->read(&something, 4);

            Ogre::Vector3 rotX;
            Ogre::Vector3 rotY;
            Ogre::Vector3 rotZ;
            Ogre::Vector3 pos;
            Ogre::Vector3 impulseRot;
            Ogre::Vector3 impulseLinear;
            Ogre::Vector3 impulseRotIncr;
            Ogre::Vector3 impulseLinearIncr;

            fileToLoad->read(&rotX, 4 * 3);
            fileToLoad->read(&rotY, 4 * 3);
            fileToLoad->read(&rotZ, 4 * 3);
            fileToLoad->read(&pos, 4 * 3);
            fileToLoad->read(&impulseRot, 4 * 3);
            fileToLoad->read(&impulseLinear, 4 * 3);
            fileToLoad->read(&impulseRotIncr, 4 * 3);
            fileToLoad->read(&impulseLinearIncr, 4 * 3);

            Ogre::Matrix4 transform(
                rotX.x, rotY.x, rotZ.x, pos.x,
                rotX.y, rotY.y, rotZ.y, pos.y,
                rotX.z, rotY.z, rotZ.z, -pos.z,
                0.0f, 0.0f, 0.0f, 1.0f);

            gameState.getTrackPositions().push_back(transform);
            gameState.getTrackOriginalImpulseLinear().push_back(Ogre::Vector3(impulseLinear.x, impulseLinear.y, -impulseLinear.z));

            fileToLoad->close();
        }
        else {assert(false && "No PHY file");}
    }
}
