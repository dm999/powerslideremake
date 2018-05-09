#ifndef CHAMPIONSHIP
#define CHAMPIONSHIP

#include <map>

#include "FinishBoard.h"

class ModeContext;

class Championship
{
public:

    void init();

    void trackFinished(const ModeContext& modeContext);

    bool isFinished() const {return mIsFinished;}
    bool isFirstFruitAvailable() const {return mIsFirstFruitAvailable;}
    bool isSecondFruitAvailable() const {return mIsSecondFruitAvailable;}

    size_t getCurrentTrack() const {return mCurrentTrack;}

    finishBoardVec getLeaderboard() const;

    bool getIsShownLeaderboard() const{ return mIsShownLeaderboardAfterFinish;}
    void setShownLeaderboard(){mIsShownLeaderboardAfterFinish = true;}

    std::string getAwardString(int index, const ModeContext& modeContext) const;
    std::string getUnlockedString(int index) const;

    static const int mWinnerFruitOffset             = 0;
    static const int mEveryWinnerFruitOffset        = mWinnerFruitOffset            + 4;//novice, advanced, expert, insane
    static const int mExpertCarFruitOffset          = mEveryWinnerFruitOffset       + 4;//novice, advanced, expert, insane
    static const int mInsaneCarFruitOffset          = mExpertCarFruitOffset         + 7;//7 cars
    static const int mBeatEmergentGunFruitOffset    = mInsaneCarFruitOffset         + 7;//7 cars
    static const int mBrusselFruitOffset            = mBeatEmergentGunFruitOffset   + 8;//8 tracks

private:

    size_t getPointsFromPosition(size_t position) const;

    size_t mCurrentTrack;
    size_t mUserPoints;
    std::string mUserCharacter;
    std::map<std::string, size_t> mCharToPoints;//not includes user

    bool mIsShownLeaderboardAfterFinish;

    bool mIsFinished;
    bool mIsFirstFruitAvailable;
    bool mIsSecondFruitAvailable;
};

#endif