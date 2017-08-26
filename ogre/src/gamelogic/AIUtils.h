
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

    void performAICorrection(PSAICar* aiCar, bool isGamePaused);

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

    AIWhole mAIWhole;
};

#endif