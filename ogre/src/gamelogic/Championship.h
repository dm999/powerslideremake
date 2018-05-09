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
    bool isWinner() const {return mIsWinner;}

    size_t getCurrentTrack() const {return mCurrentTrack;}

    finishBoardVec getLeaderboard() const;

    bool getIsShownLeaderboard() const{ return mIsShownLeaderboardAfterFinish;}
    void setShownLeaderboard(){mIsShownLeaderboardAfterFinish = true;}

    std::string getAwardString(const ModeContext& modeContext) const;
    std::string getUnlockedString(const ModeContext& modeContext) const;

private:

    size_t getPointsFromPosition(size_t position) const;

    size_t mCurrentTrack;
    size_t mUserPoints;
    std::string mUserCharacter;
    std::map<std::string, size_t> mCharToPoints;//not includes user

    bool mIsShownLeaderboardAfterFinish;

    bool mIsFinished;
    bool mIsWinner;
};

#endif