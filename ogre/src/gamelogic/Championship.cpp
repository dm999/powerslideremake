#include "Championship.h"

#include "../gamemodes/ModeContext.h"

void Championship::init()
{
    mCurrentTrack = 0;
    mUserPoints = 0;
    mCharToPoints.clear();
}

void Championship::trackFinished(const ModeContext& modeContext)
{
    mIsShownLeaderboardAfterFinish = false;
    const finishBoardVec& finishBoard = modeContext.getFinishBoard();

    for(size_t q = 0; q < finishBoard.size(); ++q)
    {
        size_t points = getPointsFromPosition(q);

        if(finishBoard[q].mIsPlayer)
        {
            mUserPoints += points;
            mUserCharacter = finishBoard[q].mCharName;
        }
        else
        {
            std::map<std::string, size_t>::iterator i = mCharToPoints.find(finishBoard[q].mCharName);
            if(i == mCharToPoints.end())
            {
                mCharToPoints.insert(std::make_pair(finishBoard[q].mCharName, points));
            }
            else
            {
                (*i).second += points;
            }
        }
    }

    ++mCurrentTrack;
}

size_t Championship::getPointsFromPosition(size_t position) const
{
    size_t ret = 0;

    switch(position)
    {
        case 0:
            ret = 10;
        break;
        case 1:
            ret = 7;
        break;
        case 2:
            ret = 5;
        break;
        case 3:
            ret = 3;
        break;
        case 4:
            ret = 2;
        break;
        case 5:
            ret = 1;
        break;
    }

    return ret;
}

bool Championship::isFinished(const ModeContext& modeContext) const
{
    bool ret = false;

    AIStrength strength = modeContext.getGameState().getAIStrength();

    if(strength == Easy)
    {
        if(mCurrentTrack >= 3)
            ret = true;
    }

    if(strength == Medium)
    {
        if(mCurrentTrack >= 5)
            ret = true;
    }

    if(strength == Hard || strength == Insane)
    {
        if(mCurrentTrack >= 8)
            ret = true;
    }

    return ret;
}

finishBoardVec Championship::getLeaderboard() const
{
    finishBoardVec ret;

    ret.push_back(finishBoardElement(mUserPoints, true, 0.0f, 0.0f, mUserCharacter));

    for(std::map<std::string, size_t>::const_iterator i = mCharToPoints.begin(); i != mCharToPoints.end(); ++i)
    {
        ret.push_back(finishBoardElement((*i).second, false, 0.0f, 0.0f, (*i).first));
    }

    std::sort(ret.begin(), ret.end(), std::greater<finishBoardElement>());

    return ret;
}