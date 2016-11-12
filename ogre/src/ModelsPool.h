#ifndef MODELSPOOL_H
#define MODELSPOOL_H

#include <string>
#include <vector>

#include "OgreInclude.h"

#include "GameState.h"

#include "MshData.h"
#include "loaders/PFLoader.h"

class ModelsPool
{
public:

    ModelsPool();

    ~ModelsPool(){}

    void initModels(Ogre::SceneManager* sceneMgr, const GameState& gameState);

    void getCopyOfVehicle(Ogre::Entity* vehicleModel[5]);

    Ogre::Entity* getArrow(){return mArrowModel;}

private:

    struct vehicleModel
    {
        Ogre::Entity* mVehicleModel[5];// 4 fl, 3 fr, 2 rl, 1 rr, 0 cockpit
    };

    void loadArrow(const PFLoader& pfloader);
    void loadVehicle(const PFLoader& pfloader, vehicleModel& vehicle, const std::string& path, const std::string& fileName);

    Ogre::Entity * createEntityFromMSH(MSHData& source, const std::string& subMaterial, const std::string& entityName) const;

    Ogre::SceneManager* mSceneMgr;

    std::vector<vehicleModel> mVehicles;
    Ogre::Entity* mArrowModel;

    static Ogre::NameGenerator nameGenMeshes;
};

#endif