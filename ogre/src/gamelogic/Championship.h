#ifndef CHAMPIONSHIP
#define CHAMPIONSHIP

#include <map>

class ModeContext;

class Championship
{
public:

    void init();

    void trackFinished(const ModeContext& modeContext);

    bool isFinished(const ModeContext& modeContext) const;

    size_t getCurrentTrack() const {return mCurrentTrack;}

    bool getIsShownLeaderboard() const{ return mIsShownLeaderboardAfterFinish;}
    void setShownLeaderboard(){mIsShownLeaderboardAfterFinish = true;}

private:

    size_t getPointsFromPosition(size_t position) const;

    size_t mCurrentTrack;
    size_t mUserPoints;
    std::string mUserCharacter;
    std::map<std::string, size_t> mCharToPoints;//not includes user

    bool mIsShownLeaderboardAfterFinish;
};

#endif