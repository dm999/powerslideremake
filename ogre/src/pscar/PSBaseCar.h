
#ifndef PSBASECAR_H
#define PSBASECAR_H

#include "PSBaseVehicle.h"

#include "../loaders/PFLoader.h"
#include "../OriginalSettings.h"

#include "../listeners/PhysicsListener.h"

struct lua_State;

class ModelsPool;
class Physics;
class StaticMeshProcesser;
class OpenALSource;

class GameState;

class PSBaseCar : public PSBaseVehicle, public PhysicsListener
{
public:

    PSBaseCar();
    virtual ~PSBaseCar(){}

    virtual void processFrameBeforePhysics(const StaticMeshProcesser& processer, bool isRaceStarted) = 0;

    virtual void initModel( lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            Physics * world,
                            const std::string& characterName,
                            InitialVehicleSetup& initialVehicleSetup,
                            bool isAI);

    //PhysicsListener
    void timeStepAfter(Physics * physics)override;

    void repositionVehicle(const Ogre::Matrix4& transform); // for multiplayer
    void repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot); // for multiplayer

    void initSounds(lua_State * pipeline, const GameState& gameState);
    void deinitSounds();

    Ogre::Vector3 getLinearVelocity()const;
    Ogre::Vector3 getAngularVelocity()const;
    Ogre::Real getAlignedVelocity()const;
    Ogre::Real getAlignedVelocitySpeedometer()const;
    Ogre::Real getWheelBackLVelocity() const;
    Ogre::Real getWheelBackRVelocity() const;
    Ogre::Real getLateralVelocity()const;
    Ogre::Vector3 getForwardAxis()const;
    Ogre::Vector3 getLinearImpulse()const;

    Ogre::SceneNode* getModelNode(){return mModelNode;}
    Ogre::SceneNode* getModelNode() const{return mModelNode;}

    void clear()override;

    std::string getFrontLWheelColliderString(){return mWheelFrontLColliderString;}
    std::string getFrontRWheelColliderString(){return mWheelFrontRColliderString;}
    std::string getBackLWheelColliderString(){return mWheelBackLColliderString;}
    std::string getBackRWheelColliderString(){return mWheelBackRColliderString;}

    virtual size_t getCurrentLap() const = 0;
    virtual Ogre::Real getLapPosition() const = 0;

    std::string getCharacterName() const {return mCharacterName;}
    void setCharacterName(const std::string& character) {mCharacterName = character;}

    void setVisibility(bool isVisible);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

protected:

    void stopSounds();

    float getCarParameter(const std::string& section, const std::string& key, bool isSpline = false) const;
    Ogre::Vector3 getCarArray3Parameter(const std::string& section, const std::string& key) const;
    Ogre::Vector4 getCarArray4Parameter(const std::string& section, const std::string& key) const;
    std::vector<std::string> getCarArrayValueParameter(const std::string& section, const std::string& key) const;
    std::vector<Ogre::Real> convertSplinePoints(const std::vector<std::string>& points) const;


    Ogre::SceneNode* mModelNode;
    Ogre::Entity* mModelEntity[InitialVehicleSetup::mWheelsAmount + 1];//Cockpit, RR, RL, FR, FL

    std::vector<std::vector<Ogre::Vector3> > mSuspensionData;
    std::vector<std::vector<size_t> > mSuspensionIndices;
    std::vector<std::vector<Ogre::Vector3> > mSuspensionPointOriginalPos;
    Ogre::Vector3 mFrontLOriginalPos;
    Ogre::Vector3 mFrontROriginalPos;
    Ogre::Vector3 mBackLOriginalPos;
    Ogre::Vector3 mBackROriginalPos;


    Ogre::Entity * mWheelEntitys[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
    Ogre::SceneNode *mWheelNodes[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL



    std::string mWheelFrontLColliderString;
    std::string mWheelFrontRColliderString;
    std::string mWheelBackLColliderString;
    std::string mWheelBackRColliderString;

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

    //param.str
    STRSettings mCarSettings;
    STRSettings mTrackSettings;
    STRSettings mDefaultSettings;

    //graphs.str
    STRSettings mCarSplines;
    STRSettings mTrackSplines;
    STRSettings mDefaultSplines;

    static Ogre::NameGenerator nameGenMaterials;
    static Ogre::NameGenerator nameGenTextures;

};

#endif