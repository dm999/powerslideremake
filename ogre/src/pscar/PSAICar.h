
#ifndef PSAICAR
#define PSAICAR

#include "../includes/OgreInclude.h"
#include "PSControllableCar.h"
#include "../gamelogic/AIUtils.h"

class PSAICar : public PSControllableCar
{
public:

    PSAICar();

    virtual ~PSAICar(){}

    virtual void initModel( lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            Physics * world,
                            const std::string& characterName,
                            InitialVehicleSetup& initialVehicleSetup,
                            bool isPossesCamera)override;

    void performAICorrection(const GameState& gameState, PhysicsVehicleAI* physicsAICar, const InitialVehicleSetup& initialVehicleSetup, Ogre::int32 afterStartCounter);
    void setAIData(const AIWhole& aiWhole, Ogre::SceneManager* sceneMgr, bool isDebugAI);
    void raceStarted();

    void setSlotIndex(size_t index){mSlotIndex = index;}
    size_t getSlotIndex() const {return mSlotIndex;}

    const AIWhole& getAIData() const {return mAIUtils.getAIData();}


private:

    AIUtils mAIUtils;

    size_t mSlotIndex;
};

#endif