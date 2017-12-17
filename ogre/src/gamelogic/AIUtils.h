
#ifndef AIUTILS_H
#define AIUTILS_H

#include "../includes/OgreInclude.h"

#include "../tools/Tools.h"

class PSAICar;
class PhysicsVehicleAI;
class GameState;
struct InitialVehicleSetup;

struct AIDataSegment
{
    Ogre::Vector3 posA, posB;
    Ogre::Vector3 tangentA, tangentB;
    Ogre::Vector3 magicA, magicB;
    Ogre::Real segmentLength;
};

class AIUtils
{
public:

    AIUtils();
    ~AIUtils(){}

    void setAIData(const AIWhole& aiWhole, Ogre::SceneManager* sceneMgr, bool isDebugAI);

    void performAICorrection(PSAICar* aiCar, PhysicsVehicleAI* physicsAICar, const GameState& gameState, const InitialVehicleSetup& initialVehicleSetup);

    void raceStarted();

    const AIWhole& getAIData() const {return mAIWhole;}

private:

    Ogre::Timer mTimerAIStuck;
    Ogre::Timer mTimerReverse;
    Ogre::Real mAIDistanceLength;
    Ogre::Vector3 mPrevPos;
    bool mIsReverseEnabled;

    //NN based functions
    void calcFeatures(PSAICar* aiCar, PhysicsVehicleAI* physicsAICar, const GameState& gameState, const InitialVehicleSetup& initialVehicleSetup);
    void inference(float& steering, float& acceleration);//return steering (-1.0 right, 1.0 left)
    void adjustInferenceResults(float& steering, float& acceleration, float& breaks, bool isMineshafted) const;

    size_t getClosestSplinePoint(const Ogre::Vector3& carPos) const;
    size_t getRelativeClosestSplinePoint(const Ogre::Vector3& carPos);
    size_t getFracIndex(size_t closestSplineIndex, const Ogre::Vector3& carPos, float & frac) const;

    struct SplineFeatures{
        Ogre::Vector3 out6, out7, out12, out13;
        float out8, out9, out10, out11;

        SplineFeatures() : out12(Ogre::Vector3::ZERO), out13(Ogre::Vector3::ZERO){}
    };

    SplineFeatures getSplineFeatures(
        PhysicsVehicleAI* physicsAICar,
        const Ogre::Vector3& carPos, 
        const Ogre::Vector3& impulseLinear, 
        const InitialVehicleSetup& initialVehicleSetup,
        float frac, size_t fracIndex, 
        float feature3, float feature4) const;

    void mulSlotMatrix(size_t fromRow, size_t toRow);

    size_t mPrevClosestSplineIndex;
    bool mIsPrevClosestSplineIndexInited;
    AIWhole mAIWhole;
    Ogre::Vector3 mPrevRot;
};

#endif