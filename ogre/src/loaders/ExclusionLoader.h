#ifndef EXCLUSIONLOADER_H
#define EXCLUSIONLOADER_H

#include "PFLoader.h"

#include "../GameState.h"

/**
 * Exclusion boxes - approach to volumetrical shadow loader
 */
class ExclusionLoader
{
public:
    ExclusionLoader();

    void load(GameState& gameState, Ogre::SceneManager* sceneMgr, bool isDebugExclusion) const;

private:

    mutable Ogre::NameGenerator mNameGenNodes;
};

#endif
