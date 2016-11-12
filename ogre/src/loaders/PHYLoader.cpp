#include "../pcheader.h"

#include "PHYLoader.h"

#include "../Conversions.h"

void PHYLoader::load(GameState& gameState) const
{
    gameState.getTrackPositions().clear();

    for(int q = 0; q < (gameState.getMaxAI() + 1); ++q)
    {
        FILE * fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getTrackName() + "/record", "car" + Conversions::DMToString(q) + ".phy");
        if(fileToLoad)
        {
            typedef unsigned int DWORD;

            DWORD something;
            fread(&something, 4, 1, fileToLoad);

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

            fclose(fileToLoad);
        }
        else {assert(false && "No PHY file");}
    }
}

void PHYLoader::loadVector(FILE * f, float data[3]) const
{
    fread(&data[0], 4, 1, f);
    fread(&data[1], 4, 1, f);
    fread(&data[2], 4, 1, f);

    //fprintf(ff, "%10.4f %10.4f %10.4f\n", data[0], data[1], data[2]);
}