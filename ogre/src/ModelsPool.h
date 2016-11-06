#ifndef MODELSPOOL_H
#define MODELSPOOL_H

#include "OgreInclude.h"

#include "loaders/PFLoader.h"

#include "MshData.h"

class ModelsPool
{
public:

    ModelsPool();

    ~ModelsPool(){}

    void initModels(Ogre::SceneManager* sceneMgr, const PFLoader& pfloader);

    void getCopyOfWarthog(Ogre::Entity* warthogModel[5]);

    Ogre::Entity* getArrow(){return mArrowModel;}

private:

    void loadArrow(const PFLoader& pfloader);
    void loadVehicles(const PFLoader& pfloader);

    Ogre::Entity * createEntityFromMSH(MSHData& source, const std::string& subMaterial, const std::string& entityName) const;

    Ogre::SceneManager* mSceneMgr;

    Ogre::Entity* mWarthogModel[5];// 4 fl, 3 fr, 2 rl, 1 rr, 0 cockpit
    Ogre::Entity* mArrowModel;

    static Ogre::NameGenerator nameGenMeshes;
};

#endif