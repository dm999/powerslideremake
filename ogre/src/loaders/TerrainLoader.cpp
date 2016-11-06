#include "../pcheader.h"

#include "TerrainLoader.h"

TerrainLoader::TerrainLoader() 
    : remapFriction(16, 0), remapSounds(16, 0), remapSoundsCollision(16, 0),
    latitudeFriction(16, 0.0f), longtitudeFriction(16, 0.0f)
{}

void TerrainLoader::load(const PFLoader& pfLoader, GameState& gameState)
{
    FILE * fileToLoad = pfLoader.getFile("data/tracks/" + gameState.getTrackName() + "/graphst", "terrain.txt");
    if(fileToLoad)
    {
        for(size_t q = 0; q < 16; ++q) fscanf(fileToLoad, " %d", &remapFriction[q]);
        fscanf(fileToLoad, "\n");

        for(size_t q = 0; q < 16; ++q) fscanf(fileToLoad, " %d", &remapSounds[q]);
        fscanf(fileToLoad, "\n");

        {
            int something;
            for(size_t q = 0; q < 16; ++q)
            {
                fscanf(fileToLoad, " %d", &something);
                if(something < 0) something = 0;
                remapSoundsCollision[q] = something;
            }
            fscanf(fileToLoad, "\n");
        }

        {
            int something;
            for(size_t q = 0; q < 16; ++q) fscanf(fileToLoad, " %d", &something);
            fscanf(fileToLoad, "\n");
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
                fscanf(fileToLoad, "%f;%f;%f,%f,%f,%f\n", 
                                &rollingResistance, &longtitudinalGripMultiplier, &latitudinalGripMultiplier, &combinedGripMultiplier,
                                &something, &something2);

                latitudeFriction[q] = longtitudinalGripMultiplier + 0.2f;
                longtitudeFriction[q] = rollingResistance * 100.0f;
            }
        }

        fclose(fileToLoad);
    }
    else {assert(false);}
}