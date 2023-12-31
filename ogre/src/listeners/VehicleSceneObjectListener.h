#ifndef VEHICLESCENEOBJECTLISTENER_H
#define VEHICLESCENEOBJECTLISTENER_H

#include "../includes/OgreInclude.h"

//http://www.ogre3d.org/forums/viewtopic.php?f=5&t=67504
class VehicleSceneObjectListener : public Ogre::MovableObject::Listener
{
public:
    VehicleSceneObjectListener(const Ogre::MovableObject *movableObj, Ogre::SceneManager *sceneManager, bool isAdvancedLighting);

    virtual const Ogre::LightList* objectQueryLights(const Ogre::MovableObject*);
private:
    Ogre::LightList mLightsCache;
    const Ogre::MovableObject *mMovableObj;
    Ogre::SceneManager *mSceneMgr;
    Ogre::SceneNode *mParentNode;
    Ogre::ulong mLightListUpdated;

    bool mIsInited;
    bool mIsAdvancedLighting;
};

#endif