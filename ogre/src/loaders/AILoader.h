#ifndef AILOADER_H
#define AILOADER_H

#include "PFLoader.h"

#include "../GameState.h"

/**
 * Loads AI trajectories
 */
class AILoader
{
public:
    AILoader();

    void load(GameState& gameState, Ogre::SceneManager* sceneMgr, bool isDebugAI) const;

private:

    mutable Ogre::NameGenerator mNameGenNodes;
};

#endif
