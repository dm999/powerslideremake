
#include "PHYLoader.h"

#include "../tools/Conversions.h"

void PHYLoader::load(GameState& gameState) const
{
    gameState.getTrackPositions().clear();

    for(int q = 0; q < (gameState.getMaxAI() + 1); ++q)
    {
        Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()) + "/record", "car" + Conversions::DMToString(q) + ".phy");
        if(fileToLoad.get() && fileToLoad->isReadable())
        {
            typedef unsigned int DWORD;

            DWORD something;
            fileToLoad->read(&something, 4);

            float rotX[3];
            float rotY[3];
            float rotZ[3];
            float pos[3];

            loadVector(fileToLoad, rotX);   // rotation matrix
            loadVector(fileToLoad, rotY);   // rotation matrix
            loadVector(fileToLoad, rotZ);   // rotation matrix
            loadVector(fileToLoad, pos);    // car position in track grid

            Ogre::Matrix4 transform(
                rotX[0], rotY[0], rotZ[0], pos[0],
                rotX[1], rotY[1], rotZ[1], pos[1],
                rotX[2], rotY[2], rotZ[2], -pos[2],
                0.0f, 0.0f, 0.0f, 1.0f);

            gameState.getTrackPositions().push_back(transform);

            fileToLoad->close();
        }
        else {assert(false && "No PHY file");}
    }
}

void PHYLoader::loadVector(const Ogre::DataStreamPtr& stream, float data[3]) const
{
    stream->read(&data[0], 4);
    stream->read(&data[1], 4);
    stream->read(&data[2], 4);

    //fprintf(ff, "%10.4f %10.4f %10.4f\n", data[0], data[1], data[2]);
}