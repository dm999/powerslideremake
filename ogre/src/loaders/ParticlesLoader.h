#ifndef PARTICLESLOADER_H
#define PARTICLESLOADER_H

#include "PFLoader.h"

#include "../GameState.h"

/**
 * Particle data loader
 */
class ParticlesLoader
{
public:
    ParticlesLoader(){}

    void load(const PFLoader& pfLoader, GameState& gameState) const;

private:

};

#endif
