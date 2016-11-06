#ifndef VEHICLESCENEOBJECTLISTENER_H
#define VEHICLESCENEOBJECTLISTENER_H

#include "OgreInclude.h"

//http://www.ogre3d.org/forums/viewtopic.php?f=5&t=67504
class VehicleSceneObjectListener : public Ogre::MovableObject::Listener
{
public:
    VehicleSceneObjectListener(const Ogre::MovableObject *movableObj, Ogre::SceneManager *sceneManager);

    virtual const Ogre::LightList* objectQueryLights(const Ogre::MovableObject*);
private:
    Ogre::LightList mLightsCache;
    const Ogre::MovableObject *mMovableObj;
    Ogre::SceneManager *mSceneMgr;
    Ogre::SceneNode *mParentNode;
    Ogre::ulong mLightListUpdated;
};

#endif