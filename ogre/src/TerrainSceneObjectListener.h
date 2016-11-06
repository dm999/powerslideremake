#ifndef TERRAINSCENEOBJECTLISTENER_H
#define TERRAINSCENEOBJECTLISTENER_H

#include "OgreInclude.h"
#include "Tools.h"

//http://www.ogre3d.org/forums/viewtopic.php?f=5&t=67504
class TerrainSceneObjectListener : public Ogre::MovableObject::Listener
{
public:
    TerrainSceneObjectListener(const Ogre::MovableObject *movableObj, 
                                Ogre::SceneManager *sceneManager,
                                const std::vector<LightEclusion>& excl);

    virtual const Ogre::LightList* objectQueryLights(const Ogre::MovableObject*);

private:
    Ogre::LightList mLightsCache;
    std::vector<Ogre::Light*> mLights;
    const Ogre::MovableObject *mMovableObj;
    Ogre::SceneManager *mSceneMgr;
    Ogre::SceneNode *mParentNode;
    bool mIsLightListCreated;
    bool mIsNeedToRebuildLightList;

    bool checkSphereAndTriangleIntersection(const Ogre::Vector3& sphereCenter, Ogre::Real radius);
    bool checkSphereAndTriangleIntersectionBullet(const Ogre::Vector3& sphereCenter, Ogre::Real radius);

    void doCPUVertexColor();

    bool isMeshFullyCoveredByBox(size_t lightIndex);

    OverlapStatus exclusionBoxAndMeshRelation(size_t lightIndex);
    OverlapStatus lightSphereAndMeshRelation(const Ogre::Vector3& lightPosition, Ogre::Real lightRange);

    void adjustVertexColors(size_t lightIndex);
    bool isBoxContainsPoint(const Ogre::Vector3& min, const Ogre::Vector3& max, const Ogre::Vector3& point, Ogre::Real margin = 0.0f);
    bool isBoxFalloff(const Ogre::Vector3& min, const Ogre::Vector3& max, const Ogre::Vector3& point, Ogre::Real margin = 0.0f);

    const std::vector<LightEclusion>& mLightExclusions;
};

#endif

