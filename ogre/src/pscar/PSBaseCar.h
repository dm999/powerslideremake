
#ifndef PSBASECAR_H
#define PSBASECAR_H

#include "PSBaseVehicle.h"

#include "../loaders/PFLoader.h"
#include "../OriginalSettings.h"

#include "../customs/CustomRigidBody.h"

struct lua_State;

class StaticMeshProcesser;
class ModelsPool;

class OpenALSource;

class GameState;

class PSBaseCar : public PSBaseVehicle
{
public:

    PSBaseCar();
    virtual ~PSBaseCar(){}

    virtual void processFrameBeforePhysics(const Ogre::FrameEvent &evt, StaticMeshProcesser& processer, bool isRaceStarted) = 0;
    virtual void processFrameAfterPhysics(const Ogre::FrameEvent &evt);

    virtual void initModel( lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            OgreBulletDynamics::DynamicsWorld * world,
                            const std::string& characterName,
                            const Ogre::Matrix4& transform,
                            const Ogre::Vector3& initialForcesLinear,
                            bool isAI);

    void repositionVehicle(const Ogre::Matrix4& transform); // for multiplayer
    void repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot); // for multiplayer

    void initSounds(lua_State * pipeline, const GameState& gameState);
    void deinitSounds();

    Ogre::Vector3 getLinearVelocity()const{return mCarChassis->getLinearVelocity();}
    Ogre::Vector3 getAngularVelocity()const{return mCarChassis->getAngularVelocity();}
    Ogre::Real getAlignedVelocity()const;
    Ogre::Real getLateralVelocity()const;
    Ogre::Vector3 getForwardAxis()const;
    Ogre::Vector3 getLinearForce()const;

    Ogre::SceneNode* getModelNode(){return mModelNode;}

    void clear()override;

    std::string getFrontLWheelColliderString(){return mWheelFrontLColliderString;}
    std::string getFrontRWheelColliderString(){return mWheelFrontRColliderString;}
    std::string getBackLWheelColliderString(){return mWheelBackLColliderString;}
    std::string getBackRWheelColliderString(){return mWheelBackRColliderString;}

    unsigned char getFrontLWheelColliderIndex(){return mWheelFrontLColliderIndex;}
    unsigned char getFrontRWheelColliderIndex(){return mWheelFrontRColliderIndex;}
    unsigned char getBackLWheelColliderIndex(){return mWheelBackLColliderIndex;}
    unsigned char getBackRWheelColliderIndex(){return mWheelBackRColliderIndex;}
    unsigned char getChassisColliderIndex(){return mChassisColliderIndex;}

    const CustomRigidBody& getChassis()const{return *mCarChassis;}

    virtual size_t getCurrentLap() const = 0;
    virtual Ogre::Real getLapPosition() const = 0;

    std::string getCharacterName() const {return mCharacterName;}
    void setCharacterName(const std::string& character) {mCharacterName = character;}

    void setVisibility(bool isVisible);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

protected:

    virtual void processSounds(const Ogre::FrameEvent &evt) = 0;

    void stopSounds();

    float getCarParameter(const STRSettings& carSettings, const STRSettings& trackSettings, const STRSettings& defaultSettings, const std::string& section, const std::string& key) const;
    Ogre::Vector4 getCarArray4Parameter(const STRSettings& carSettings, const STRSettings& trackSettings, const STRSettings& defaultSettings, const std::string& section, const std::string& key) const;

    bool mIsAI;

    Ogre::SceneNode* mModelNode;
    Ogre::Entity* mModelEntity[5];

    std::vector<std::vector<Ogre::Vector3> > mSuspensionData;
    std::vector<std::vector<size_t> > mSuspensionIndices;
    std::vector<std::vector<Ogre::Vector3> > mSuspensionPointOriginalPos;
    Ogre::Vector3 mFrontLOriginalPos;
    Ogre::Vector3 mFrontROriginalPos;
    Ogre::Vector3 mBackLOriginalPos;
    Ogre::Vector3 mBackROriginalPos;


    Ogre::Entity * mWheelEntitys[4];
    Ogre::SceneNode *mWheelNodes[4];



    std::string mWheelFrontLColliderString;
    std::string mWheelFrontRColliderString;
    std::string mWheelBackLColliderString;
    std::string mWheelBackRColliderString;

    unsigned char mWheelFrontLColliderIndex;
    unsigned char mWheelFrontRColliderIndex;
    unsigned char mWheelBackLColliderIndex;
    unsigned char mWheelBackRColliderIndex;
    unsigned char mChassisColliderIndex;

    std::string mCharacterName;

    //sounds
#ifndef NO_OPENAL
    CommonIncludes::shared_ptr<OpenALSource> mEngLow;
    CommonIncludes::shared_ptr<OpenALSource> mEngMid;
    CommonIncludes::shared_ptr<OpenALSource> mEngHigh;
#endif

private:

    void initSuspension(const GameState& gameState);

    std::string loadTexture(const GameState& gameState, const std::string& textureName, std::string& carPath);

#if defined(__ANDROID__)
    std::string mTextureName;
#endif

    static Ogre::NameGenerator nameGenMaterials;
    static Ogre::NameGenerator nameGenTextures;

};

#endif