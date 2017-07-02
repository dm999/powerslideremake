
#include "TerrainLoader.h"

TerrainLoader::TerrainLoader() 
    : remapFriction(16, 0), remapSounds(16, 0), remapSoundsCollision(16, 0),
    latitudeFriction(16, 0.0f), longtitudeFriction(16, 0.0f)
{}

void TerrainLoader::load(GameState& gameState)
{
    Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()) + "/graphst", "terrain.txt");
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        char buf[1025];
        fileToLoad->readLine(buf, 1024);
        for(size_t q = 0; q < 16; ++q) sscanf(buf, " %d", &remapFriction[q]);

        fileToLoad->readLine(buf, 1024);
        for(size_t q = 0; q < 16; ++q) sscanf(buf, " %d", &remapSounds[q]);

        {
            int something;
            fileToLoad->readLine(buf, 1024);
            for(size_t q = 0; q < 16; ++q)
            {
                sscanf(buf, " %d", &something);
                if(something < 0) something = 0;
                remapSoundsCollision[q] = something;
            }
        }

        {
            int something;
            fileToLoad->readLine(buf, 1024);
            for(size_t q = 0; q < 16; ++q) sscanf(buf, " %d", &something);
        }

        {
            //d.polubotko(TODO): usage should be considered
            //??
            //Later lines: For each terrain Rolling Resistance;Longtitudinal grip multiplier,Latitudinal,combined
            //0.00081;0.05,0.1,0.9

            float rollingResistance, longtitudinalGripMultiplier, latitudinalGripMultiplier, combinedGripMultiplier;
            float something, something2;
            for(size_t q = 0; q < 16; ++q)
            {
                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "%f;%f;%f,%f,%f,%f\n", 
                                &rollingResistance, &longtitudinalGripMultiplier, &latitudinalGripMultiplier, &combinedGripMultiplier,
                                &something, &something2);

                latitudeFriction[q] = longtitudinalGripMultiplier + 0.2f;
                longtitudeFriction[q] = rollingResistance * 100.0f;
            }
        }

        fileToLoad->close();
    }
    else {assert(false);}
}