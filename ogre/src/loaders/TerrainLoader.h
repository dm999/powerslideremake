#ifndef TERRAINLOADER_H
#define TERRAINLOADER_H

#include <vector>

#include "PFLoader.h"

#include "../GameState.h"

/**
 * Terrain information loader (terrain maps used to extract it)
 */
class TerrainLoader
{
public:
    TerrainLoader();

    void load(GameState& gameState);

    const std::vector<size_t>& getRemapFriction() const {return remapFriction;}
    const std::vector<size_t>& getRemapSounds() const {return remapSounds;}
    const std::vector<size_t>& getRemapSoundsCollision() const {return remapSoundsCollision;}
    const std::vector<float>& getLatitudeFriction() const {return latitudeFriction;}
    const std::vector<float>& getLongtitudeFriction() const {return longtitudeFriction;}

private:

    std::vector<size_t> remapFriction;
    std::vector<size_t> remapSounds;
    std::vector<size_t> remapSoundsCollision;
    std::vector<float> latitudeFriction;
    std::vector<float> longtitudeFriction;
};

#endif
