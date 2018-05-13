#ifndef CHAMPIONSHIP
#define CHAMPIONSHIP

#include <map>
#include <vector>

#include "FinishBoard.h"

class ModeContext;

class Championship
{
public:

    void init();

    void trackFinished(const ModeContext& modeContext, Ogre::Real bestLap);

    bool isFinished() const {return mIsFinished;}
    bool isFruitsAvailable() const {return mIsFruitsAvailable;}
    std::vector<size_t> getAvailableFruits() const {return mAvailableFruits;}

    size_t getCurrentTrack() const {return mCurrentTrack;}

    finishBoardVec getLeaderboard() const;

    bool getIsShownLeaderboard() const{ return mIsShownLeaderboardAfterFinish;}
    void setShownLeaderboard(){mIsShownLeaderboardAfterFinish = true;}

    std::string getAwardString(int index, const ModeContext& modeContext, size_t trackIndex = 0) const;
    std::string getUnlockedString(int index) const;

    static bool checkEmergentGunBeaten(const ModeContext& modeContext, Ogre::Real bestLap, size_t& trackIndex);

    static const size_t mWinnerFruitOffset          = 0;
    static const size_t mEveryWinnerFruitOffset     = mWinnerFruitOffset            + 4;//novice, advanced, expert, insane
    static const size_t mExpertCarFruitOffset       = mEveryWinnerFruitOffset       + 4;//novice, advanced, expert, insane
    static const size_t mInsaneCarFruitOffset       = mExpertCarFruitOffset         + 7;//7 cars
    static const size_t mBeatEmergentGunFruitOffset = mInsaneCarFruitOffset         + 7;//7 cars
    static const size_t mBrusselFruitOffset         = mBeatEmergentGunFruitOffset   + 8;//8 tracks with fruits

private:

    size_t getPointsFromPosition(size_t position) const;

    size_t mCurrentTrack;
    size_t mUserPoints;
    std::string mUserCharacter;
    std::map<std::string, size_t> mCharToPoints;//not includes user

    bool mIsShownLeaderboardAfterFinish;

    bool mIsFinished;
    bool mIsFruitsAvailable;
    std::vector<size_t> mAvailableFruits;
    std::vector<size_t> mAvailableEmergentGunBeatenTracks;
};

#endif