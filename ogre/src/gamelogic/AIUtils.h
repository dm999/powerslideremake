
#ifndef AIUTILS_H
#define AIUTILS_H

#include "../includes/OgreInclude.h"

#include "../tools/Tools.h"

class PSAICar;
//class KochanekBartelsSpline;

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
    void clear();

    void performAICorrection(PSAICar* aiCar, bool isRaceStarted, bool isGamePaused);

    void setSpeedCoeff(Ogre::Real speedCoeff){mSpeedCoeff = speedCoeff;}

    void raceStarted();

private:

    bool mIsDebugAI;
    Ogre::SceneNode * mDebugSphereNode;

    Ogre::Real mSpeedCoeff;

    //CommonIncludes::shared_ptr<KochanekBartelsSpline> mSpline;
    Ogre::SimpleSpline mSpline;

    bool mPrevClosestSegmentInited;
    size_t mPrevClosestSegmentIndex;
    std::vector<AIDataSegment> mAIDataSegments;

    Ogre::Vector3 getTowardPoint(const Ogre::Vector3& carPos, Ogre::Vector3& towardDir);
    size_t getClosestSegment(const Ogre::Vector3& carPos, Ogre::Vector3& pointInLineRes);

    //Ogre::Vector3 getTowardPoint(const Ogre::Vector3& carPos, size_t& towardPointIndex)const;

    //void getClosestSegment(const Ogre::Vector3& carPos, size_t& indexPrev, size_t& indexCur, size_t& indexAhead)const;

    Ogre::Timer mTimerAIStuck;
    Ogre::Timer mTimerReverse;
    Ogre::Real mAIDistanceLength;
    Ogre::Vector3 mPrevPos;
    bool mIsReverseEnabled;

    //NN based functions
    void calcFeatures(PSAICar* aiCar);
    float inference();//return steering (-1.0 right, 1.0 left)

    size_t getClosestSplinePoint(const Ogre::Vector3& carPos) const;
    size_t getFracIndex(size_t closestSplineIndex, const Ogre::Vector3& carPos, float & frac) const;

    struct SplineFeatures{
        Ogre::Vector3 out6, out7, out12, out13;
        float out8, out9, out10, out11;

        SplineFeatures() : out12(Ogre::Vector3::ZERO), out13(Ogre::Vector3::ZERO){}
    };

    SplineFeatures getSplineFeatures(
        const Ogre::Vector3& carPos, 
        float frac, size_t fracIndex, 
        float feature3, float feature4) const;

    void mulSlotMatrix(size_t fromRow, size_t toRow);

    AIWhole mAIWhole;
    Ogre::Vector3 mField17;
};

#endif