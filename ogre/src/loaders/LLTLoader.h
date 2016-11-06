#ifndef LLTLOADER_H
#define LLTLOADER_H

#include "PFLoader.h"

#include "../GameState.h"

/**
 * Lookahead lap tracking loader
 */
class LLTLoader
{
public:
    LLTLoader();

    void load(const PFLoader& pfLoader, GameState& gameState, Ogre::SceneManager* sceneMgr, bool isDebugLLT) const;

private:
    mutable Ogre::NameGenerator mNameGenNodes;
};

#endif
