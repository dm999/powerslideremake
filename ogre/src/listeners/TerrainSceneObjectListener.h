#ifndef TERRAINSCENEOBJECTLISTENER_H
#define TERRAINSCENEOBJECTLISTENER_H

#include <map>

#include "../includes/OgreInclude.h"
#include "../tools/Tools.h"

class ExclusionBoxesObjectListener : public Ogre::MovableObject::Listener
{
public:

    ExclusionBoxesObjectListener(const std::vector<LightEclusion>& excl);

protected:

    bool isBoxContainsPoint(const Ogre::Vector3& min, const Ogre::Vector3& max, const Ogre::Vector3& point, Ogre::Real margin = 0.0f);
    bool isBoxFalloff(const Ogre::Vector3& min, const Ogre::Vector3& max, const Ogre::Vector3& point, Ogre::Real margin = 0.0f);

    size_t getDE2LightIndex(const std::string& lightName);

    const std::vector<LightEclusion>& mLightExclusions;
    std::vector<Ogre::Light*> mLights;

    std::map<std::string, size_t> mLightNameToDE2Name;
};

//http://www.ogre3d.org/forums/viewtopic.php?f=5&t=67504
class TerrainSceneObjectListener : public ExclusionBoxesObjectListener
{
public:
    TerrainSceneObjectListener(const Ogre::MovableObject *movableObj, 
                                Ogre::SceneManager *sceneManager,
                                const std::vector<LightEclusion>& excl);

    virtual const Ogre::LightList* objectQueryLights(const Ogre::MovableObject*);

private:
    Ogre::LightList mLightsCache;
    const Ogre::MovableObject *mMovableObj;
    Ogre::SceneManager *mSceneMgr;
    Ogre::SceneNode *mParentNode;
    bool mIsLightListCreated;
    bool mIsNeedToRebuildLightList;

    bool checkSphereAndTriangleIntersection(const Ogre::Vector3& sphereCenter, Ogre::Real radius);

    void doCPUVertexColor();

    bool isMeshFullyCoveredByBox(size_t lightIndex);

    OverlapStatus exclusionBoxAndMeshRelation(size_t lightIndex);
    OverlapStatus lightSphereAndMeshRelation(const Ogre::Vector3& lightPosition, Ogre::Real lightRange);

    void adjustVertexColors(size_t lightIndex);
};

#endif

