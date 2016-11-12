#ifndef AILOADER_H
#define AILOADER_H

#include "PFLoader.h"

#include "../GameState.h"

enum AIStrength
{
    Easy = 0,
    Medium = 1,
    Hard = 2,
    Insane = 3
};

/**
 * Loads AI trajectories
 */
class AILoader
{
public:
    AILoader();

    void load(GameState& gameState, Ogre::SceneManager* sceneMgr, bool isDebugAI, AIStrength aiStrength) const;

private:

    mutable Ogre::NameGenerator mNameGenNodes;
};

#endif
