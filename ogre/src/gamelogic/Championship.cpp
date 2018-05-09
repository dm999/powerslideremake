#include "Championship.h"

#include "../gamemodes/ModeContext.h"

#include "../tools/Tools.h"

void Championship::init()
{
    mCurrentTrack = 0;
    mUserPoints = 0;
    mCharToPoints.clear();
    mIsFinished = false;
    mIsWinner = false;

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

    //check finished
    {

        AIStrength strength = modeContext.getGameState().getAIStrength();

        if(strength == Easy)
        {
            if(mCurrentTrack >= 3)
                mIsFinished = true;
        }

        if(strength == Medium)
        {
            if(mCurrentTrack >= 5)
                mIsFinished = true;
        }

        if(strength == Hard || strength == Insane)
        {
            if(mCurrentTrack >= 8)
                mIsFinished = true;
        }
    }

    //check winner
    if(mIsFinished)
    {
        finishBoardVec leaderBoard = getLeaderboard();
        if(leaderBoard[0].mIsPlayer)
            mIsWinner = true;
    }
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

std::string Championship::getAwardString(const ModeContext& modeContext) const
{
    std::string res = "";

    const char stringsFruit[][128] = {
        "an apple",         //win novice                    1st fruit
        "a banana",         //win advanced                  1st fruit
        "an orange",        //win expert                    1st fruit
        "a strawberry",     //win insane                    1st fruit
        "a tangelo",        //every first novice            2nd fruit
        "a peach",          //every first advanced          2nd fruit
        "a passionfruit",   //every first expert            2nd fruit
        "cherries",         //every first insane            2nd fruit
        "a papaya",         //win expert with car           3rd fruit
        "a mango",          //win insane with car           3rd fruit
        "a raspberry",      //beat emergent gun on track    3rd and other fruits, single race
        "a brussel sprout"  //lost every                    1st fruit
    };

    const char stringsWinning[][128] = {
        "winning the Novice Championship",
        "winning the Advanced Championship",
        "winning the Expert Championship",
        "winning the Insane Championship",
        "placing first in every race in the Novice Championship",
        "placing first in every race in the Advanced Championship",
        "placing first in every race in the Expert Championship",
        "placing first in every race in the Insane Championship",
        "winning the Expert Championship with %carname%",
        "winning the Insane Championship with %carname%",
        "beating Ratbag's hot time on the %trackname%",
        "coming last in every race..."
    };

    AIStrength strength = modeContext.getGameState().getAIStrength();
    if(modeContext.getGameState().getPlayerData().level <= strength)
    {
        res = std::string("You have been awarded ") + std::string(stringsFruit[strength]) + std::string(" for ") + std::string(stringsWinning[strength]);

        const STRPowerslide& strPowerslide = modeContext.getGameState().getSTRPowerslide();
        Tools::replace(res, "%carname%", STRPowerslide::getCarTitle(strPowerslide.getCarFromCharacter(modeContext.getGameState().getPlayerCar().getCharacterName())));
        Tools::replace(res, "%trackname%", strPowerslide.getTrackTitle(modeContext.getGameState().getTrackNameAsOriginal()));
    }

    return res;
}

std::string Championship::getUnlockedString(const ModeContext& modeContext) const
{
    std::string res = "";

    const char stringsUnlock[][128] = {
        "the Devil's Elbow and Mineshafted tracks",
        "the Freezer, Urban Brawl, Nutopia and Stunt track and the Pickup and Skeeto cars",
        "the Luge track and the Supercar!"
    };

    AIStrength strength = modeContext.getGameState().getAIStrength();
    if(modeContext.getGameState().getPlayerData().level <= strength && strength != Insane)
    {
        res = std::string("This has unlocked ") + std::string(stringsUnlock[strength]);
    }

    return res;
}