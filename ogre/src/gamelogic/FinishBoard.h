#ifndef FINISHBOARD
#define FINISHBOARD

#include <vector>

#include "../includes/OgreInclude.h"
#include "../GameState.h"

class ModeContext;

struct finishBoardElement
{
    size_t mPos;
    bool mIsPlayer;
    float mRaceTime;
    float mBestLapTime;
    std::string mCharName;

    finishBoardElement(size_t pos, bool isPlayer, float raceTime, float bestLapTime, const std::string& charName)
        : mPos(pos), mIsPlayer(isPlayer), mRaceTime(raceTime), mBestLapTime(bestLapTime), mCharName(charName){}

    bool operator< (const finishBoardElement& other)const{return mPos < other.mPos;}

    static bool sortByRaceTime(const finishBoardElement& a, const finishBoardElement& b){return a.mRaceTime < b.mRaceTime;}
    static bool findPlayer(const finishBoardElement& other){return other.mIsPlayer;}
};

typedef std::vector<finishBoardElement> finishBoardVec;

class FinishBoard
{
public:

    static finishBoardVec prepareFinishBoard(const ModeContext& modeContext);
};

#endif