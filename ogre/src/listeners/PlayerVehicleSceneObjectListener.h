#ifndef PLAYERVEHICLESCENEOBJECTLISTENER_H
#define PLAYERVEHICLESCENEOBJECTLISTENER_H

#include "../includes/OgreInclude.h"
#include "../tools/Tools.h"

#include "TerrainSceneObjectListener.h"

//http://www.ogre3d.org/forums/viewtopic.php?f=5&t=67504
class PlayerVehicleSceneObjectListener : public ExclusionBoxesObjectListener
{
public:
    PlayerVehicleSceneObjectListener(const Ogre::MovableObject *movableObj, Ogre::SceneManager *sceneManager, const std::vector<LightEclusion>& excl);

    virtual const Ogre::LightList* objectQueryLights(const Ogre::MovableObject*);
private:

    Ogre::LightList mLightsCache;
    const Ogre::MovableObject *mMovableObj;
    Ogre::SceneManager *mSceneMgr;
    Ogre::SceneNode *mParentNode;
    Ogre::ulong mLightListUpdated;

    void doCPUVertexColor();
};

#endif