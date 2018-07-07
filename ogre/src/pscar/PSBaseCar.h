
#ifndef PSBASECAR_H
#define PSBASECAR_H

#include "PSBaseVehicle.h"
#include "PSBaseGraphicsVehicle.h"

#include "../loaders/PFLoader.h"
#include "../OriginalSettings.h"

#include "../listeners/PhysicsListener.h"

struct lua_State;

class ModelsPool;
class Physics;
class StaticMeshProcesser;
class SoundSource;

class GameState;

class PSBaseCar : public PSBaseVehicle, public PSBaseGraphicsVehicle, public PhysicsListener
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

    Ogre::Vector3 getLinearVelocitySpeedometer()const;
    Ogre::Vector3 getAngularVelocitySpeedometer()const;
    Ogre::Real getAlignedVelocity()const;
    Ogre::Real getAlignedVelocitySpeedometer()const;
    Ogre::Real getWheelBackLVelocity() const;
    Ogre::Real getWheelBackRVelocity() const;
    Ogre::Real getLateralVelocitySpeedometer()const;
    Ogre::Vector3 getForwardAxis()const;
    Ogre::Vector3 getUpAxis()const;//sound
    Ogre::Vector3 getLinearImpulse()const;//ai & multiplayer
    void setLinearImpulse(const Ogre::Vector3& val);//multiplayer
    Ogre::Vector3 getAngularImpulse()const;//multiplayer
    void setAngularImpulse(const Ogre::Vector3& val);//multiplayer

    void clear()override;

    std::string getFrontLWheelColliderString(){return mWheelFrontLColliderString;}
    std::string getFrontRWheelColliderString(){return mWheelFrontRColliderString;}
    std::string getBackLWheelColliderString(){return mWheelBackLColliderString;}
    std::string getBackRWheelColliderString(){return mWheelBackRColliderString;}

    virtual size_t getCurrentLap() const = 0;
    virtual Ogre::Real getLapPosition() const = 0;

protected:

    void stopSounds();

    float getCarParameter(const std::string& section, const std::string& key, bool isSpline = false) const;
    Ogre::Vector3 getCarArray3Parameter(const std::string& section, const std::string& key) const;
    Ogre::Vector4 getCarArray4Parameter(const std::string& section, const std::string& key) const;
    std::vector<std::string> getCarArrayValueParameter(const std::string& section, const std::string& key) const;
    std::vector<Ogre::Real> convertSplinePoints(const std::vector<std::string>& points) const;




    std::string mWheelFrontLColliderString;
    std::string mWheelFrontRColliderString;
    std::string mWheelBackLColliderString;
    std::string mWheelBackRColliderString;

    //sounds
#ifndef NO_OPENAL
    CommonIncludes::shared_ptr<SoundSource> mEngLow;
    CommonIncludes::shared_ptr<SoundSource> mEngMid;
    CommonIncludes::shared_ptr<SoundSource> mEngHigh;

    float mMasterGain;
#endif

private:

    //param.str
    STRSettings mCarSettings;
    STRSettings mTrackSettings;
    STRSettings mDefaultSettings;

    //graphs.str
    STRSettings mCarSplines;
    STRSettings mTrackSplines;
    STRSettings mDefaultSplines;

};

#endif