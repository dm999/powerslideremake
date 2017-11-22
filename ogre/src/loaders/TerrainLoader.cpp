
#include "TerrainLoader.h"

#include <sstream>

TerrainLoader::TerrainLoader(){}

void TerrainLoader::load(GameState& gameState)
{
    Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()) + "/graphst", "terrain.txt");
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        mTerrainData.clear();
        mTerrainData.resize(16);

        for(size_t q = 0; q < 16; ++q)
        {
            mTerrainData[q].mVelocityIndex = q;
        }

        char buf[1025];
        {
            fileToLoad->readLine(buf, 1024);
            std::istringstream bufStream(buf);
            for(size_t q = 0; q < 16; ++q)
            {
                int tmpVal;
                bufStream >> tmpVal;
                mTerrainData[tmpVal].mUnknownIndex = 15 - q;
            }
        }

        {
            fileToLoad->readLine(buf, 1024);
            std::istringstream bufStream(buf);
            for(size_t q = 0; q < 16; ++q)
            {
                int tmpVal;
                bufStream >> tmpVal;
                mTerrainData[q].mSoundIndex = tmpVal;
            }
        }

        {
            fileToLoad->readLine(buf, 1024);
            std::istringstream bufStream(buf);
            for(size_t q = 0; q < 16; ++q)
            {
                int tmpVal;
                bufStream >> tmpVal;
                mTerrainData[q].mSoundIndex2 = tmpVal;
            }
        }

        {
            fileToLoad->readLine(buf, 1024);
            std::istringstream bufStream(buf);
            for(size_t q = 0; q < 16; ++q)
            {
                int tmpVal;
                bufStream >> tmpVal;
                mTerrainData[q].mSoundIndex3 = tmpVal;
            }
        }

        {

            float rollingResistance, longtitudinalGripMultiplier, latitudinalGripMultiplier, combinedGripMultiplier;
            float something, something2;
            for(size_t q = 0; q < 16; ++q)
            {
                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "%f;%f;%f,%f,%f,%f\n", 
                                &rollingResistance, &longtitudinalGripMultiplier, &latitudinalGripMultiplier, &combinedGripMultiplier,
                                &something, &something2);

                mTerrainData[q].mRollResistance = rollingResistance;
                mTerrainData[q].mLongtitudinalGripMultiplier = longtitudinalGripMultiplier;
                mTerrainData[q].mLatitudinalGripMultiplier = latitudinalGripMultiplier;
                mTerrainData[q].mCombinedMultiplier = combinedGripMultiplier;
                mTerrainData[q].mParameter = something;
                mTerrainData[q].mParameter2 = something2;
            }
        }

        fileToLoad->close();
    }
    else {assert(false);}
}