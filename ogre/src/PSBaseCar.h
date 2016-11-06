
#ifndef PSBASECAR_H
#define PSBASECAR_H

#include "OgreInclude.h"
#include "OgreBulletInclude.h"
#include "GameCars.h"
#include "CommonIncludes.h"
#include "XMLParsing.h"
#include "loaders/PFLoader.h"
#include "OriginalSettings.h"

struct lua_State;
class CameraMan;

class StaticMeshProcesser;
class CustomRigidBodyWheel;
class ModelsPool;

class OpenALSource;

class GameState;

class PSBaseCar : public XMLParsing
{
public:

    PSBaseCar();
    virtual ~PSBaseCar(){}

    virtual void processFrameBeforePhysics(const Ogre::FrameEvent &evt, StaticMeshProcesser& processer, bool isRaceStarted) = 0;
    virtual void processFrameAfterPhysics(const Ogre::FrameEvent &evt);

    virtual void initModel( lua_State * pipeline, 
                            const PFLoader& pfLoaderData,
                            const PFLoader& pfLoaderStore,
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            CameraMan * cameraMan,
                            ModelsPool* modelsPool,
                            OgreBulletDynamics::DynamicsWorld * world,
                            GameCars gameCar,
                            const Ogre::Matrix4& transform,
                            bool isAI);

    void setModelPositionOnGrid(const Ogre::Matrix4& transform); // for multiplayer
    void getWheelsPositions(Ogre::Vector3& wheel0, Ogre::Vector3& wheel1, Ogre::Vector3& wheel2, Ogre::Vector3& wheel3) const; // for multiplayer

    void initSounds(lua_State * pipeline, const PFLoader& mPFLoaderData);
    void deinitSounds();

    Ogre::Vector3 getLinearVelocity()const{return mCarChassis->getLinearVelocity();}
    Ogre::Vector3 getAngularVelocity()const{return mCarChassis->getAngularVelocity();}
    Ogre::Real getAlignedVelocity()const;
    Ogre::Real getLateralVelocity()const;
    Ogre::Vector3 getForwardAxis()const;

    Ogre::SceneNode* getModelNode(){return mModelNode;}

    void clear();

    std::string getFrontLWheelColliderString(){return mWheelFrontLColliderString;}
    std::string getFrontRWheelColliderString(){return mWheelFrontRColliderString;}
    std::string getBackLWheelColliderString(){return mWheelBackLColliderString;}
    std::string getBackRWheelColliderString(){return mWheelBackRColliderString;}

    unsigned char getFrontLWheelColliderIndex(){return mWheelFrontLColliderIndex;}
    unsigned char getFrontRWheelColliderIndex(){return mWheelFrontRColliderIndex;}
    unsigned char getBackLWheelColliderIndex(){return mWheelBackLColliderIndex;}
    unsigned char getBackRWheelColliderIndex(){return mWheelBackRColliderIndex;}
    unsigned char getChassisColliderIndex(){return mChassisColliderIndex;}

    const OgreBulletDynamics::RigidBody& getChassis()const{return *mCarChassis;}

    virtual size_t getCurrentLap() const = 0;
    virtual Ogre::Real getLapPosition() const = 0;

    GameCars getCarType() const {return mGameCarType;}
    //for multiplayer
    void setCarType(GameCars& car) {mGameCarType = car;}

protected:

    virtual void processSounds(const Ogre::FrameEvent &evt) = 0;

    void stopSounds();

    float getCarParameter(const STRSettings& carSettings, const STRSettings& trackSettings, const STRSettings& defaultSettings, const std::string& section, const std::string& key) const;
    Ogre::Vector4 getCarArray4Parameter(const STRSettings& carSettings, const STRSettings& trackSettings, const STRSettings& defaultSettings, const std::string& section, const std::string& key) const;

    bool mIsAI;

    OgreBulletDynamics::DynamicsWorld * mWorld;

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

    OgreBulletDynamics::RigidBody *mCarChassis;
    CustomRigidBodyWheel *mCarWheelFrontL;
    CustomRigidBodyWheel *mCarWheelFrontR;
    CustomRigidBodyWheel *mCarWheelBackL;
    CustomRigidBodyWheel *mCarWheelBackR;
    //OgreBulletDynamics::VehicleTuning *mTuning;
    //OgreBulletDynamics::VehicleRayCaster *mVehicleRayCaster;
    //OgreBulletDynamics::RaycastVehicle *mVehicle;
    OgreBulletDynamics::SixDofSpring2Constraint * mSixDofSpringFrontL;
    OgreBulletDynamics::SixDofSpring2Constraint * mSixDofSpringFrontR;
    OgreBulletDynamics::SixDofSpring2Constraint * mSixDofSpringBackL;
    OgreBulletDynamics::SixDofSpring2Constraint * mSixDofSpringBackR;

    Ogre::Real mWheelRadiusFront;
    Ogre::Real mWheelRadiusBack;


    std::string mWheelFrontLColliderString;
    std::string mWheelFrontRColliderString;
    std::string mWheelBackLColliderString;
    std::string mWheelBackRColliderString;

    unsigned char mWheelFrontLColliderIndex;
    unsigned char mWheelFrontRColliderIndex;
    unsigned char mWheelBackLColliderIndex;
    unsigned char mWheelBackRColliderIndex;
    unsigned char mChassisColliderIndex;

    static Ogre::NameGenerator nameGenNodes;

    GameCars mGameCarType; // for multiplayer

    //sounds
    CommonIncludes::shared_ptr<OpenALSource> mEngLow;
    CommonIncludes::shared_ptr<OpenALSource> mEngMid;
    CommonIncludes::shared_ptr<OpenALSource> mEngHigh;

private:

    void initSuspension(const PFLoader& pfLoaderData);

    static Ogre::NameGenerator nameGenMaterials;
    static Ogre::NameGenerator nameGenTextures;

};

#endif