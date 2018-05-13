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
                            bool isAI);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

    std::string getCharacterName() const {return mCharacterName;}
    void setCharacterName(const std::string& character) {mCharacterName = character;}

    void setVisibility(bool isVisible);

    Ogre::SceneNode* getModelNode(){return mModelNode;}
    Ogre::SceneNode* getModelNode() const{return mModelNode;}

    void repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot); // for multiplayer, time trial ghost

protected:

    std::string getCarPath(const GameState& gameState) const;


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

    static Ogre::NameGenerator nameGenMaterials;
    static Ogre::NameGenerator nameGenTextures;
};

#endif