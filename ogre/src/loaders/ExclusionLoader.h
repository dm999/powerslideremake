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

    void load(const PFLoader& pfLoader, GameState& gameState, const std::string& excludeFile, Ogre::SceneManager* sceneMgr, bool isDebugExclusion) const;

private:

    mutable Ogre::NameGenerator mNameGenNodes;
};

#endif
