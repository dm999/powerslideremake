#ifndef TERRAINLOADER_H
#define TERRAINLOADER_H

#include <vector>

#include "PFLoader.h"

#include "../GameState.h"

#include "../physics/TerrainData.h"

/**
 * Terrain information loader (terrain maps used to extract it)
 */
class TerrainLoader
{
public:
    TerrainLoader();

    void load(GameState& gameState);

    const std::vector<TerrainData>& getTerrainData() const {return mTerrainData;}

private:

    std::vector<TerrainData> mTerrainData;
};

#endif
