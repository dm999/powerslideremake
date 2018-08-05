#ifndef PSBASEGRAPHICSVEHICLE_H
#define PSBASEGRAPHICSVEHICLE_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "../physics/InitialVehicleSetup.h"

struct lua_State;

class ModelsPool;
class StaticMeshProcesser;

class GameState;

class PSBaseGraphicsVehicle
{
public:

    PSBaseGraphicsVehicle();
    virtual ~PSBaseGraphicsVehicle(){}

    void initGraphicsModel( lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            const std::string& characterName,
                            const InitialVehicleSetup& initialVehicleSetup,
                            bool isAdvancedLighting);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

    std::string getCharacterName() const {return mCharacterName;}
    void setCharacterName(const std::string& character) {mCharacterName = character;}

    bool getVisibility() const {return mModelEntity[0]->isVisible();}
    void setVisibility(bool isVisible);

    Ogre::SceneNode* getModelNode(){return mModelNode;}
    Ogre::SceneNode* getModelNode() const{return mModelNode;}

    //for time trial ghost
    const Ogre::SceneNode *const* getWheelNodes() const{return mWheelNodes;}
    void repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot,
        const Ogre::Vector3 wheelPos[InitialVehicleSetup::mWheelsAmount], const Ogre::Quaternion wheelRot[InitialVehicleSetup::mWheelsAmount]);

    // for multiplayer
    void repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot);

    std::string getCarPath(const GameState& gameState) const;

protected:


    Ogre::SceneNode* mModelNode;
    Ogre::Entity* mModelEntity[InitialVehicleSetup::mWheelsAmount + 1];//Cockpit, RR, RL, FR, FL

    std::vector<std::vector<Ogre::Vector3> > mSuspensionData;
    std::vector<std::vector<size_t> > mSuspensionIndices;
    std::vector<std::vector<Ogre::Vector3> > mSuspensionPointOriginalPos;


    Ogre::Entity * mWheelEntitys[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
    Ogre::SceneNode *mWheelNodes[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL

private:

    void initSuspension(const GameState& gameState);

    std::string loadTexture(const GameState& gameState, const std::string& textureName);

#if defined(__ANDROID__)
    std::string mTextureName;
#endif

    std::string mCharacterName;

    Ogre::Vector3 mFrontLOriginalPos;
    Ogre::Vector3 mFrontROriginalPos;
    Ogre::Vector3 mBackLOriginalPos;
    Ogre::Vector3 mBackROriginalPos;
    Ogre::Vector3 mCOG;

    static Ogre::NameGenerator nameGenMaterials;
    static Ogre::NameGenerator nameGenTextures;
};

#endif