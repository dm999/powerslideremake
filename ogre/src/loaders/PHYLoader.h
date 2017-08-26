#ifndef PHYLOADER_H
#define PHYLOADER_H

#include "PFLoader.h"

#include "../GameState.h"

/**
 * Loads start positions of vehicles
 */
class PHYLoader
{
public:
    PHYLoader(){}

    void load(GameState& gameState) const;
};

#endif
