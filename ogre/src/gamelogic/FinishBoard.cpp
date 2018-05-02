#include "Finishboard.h"

#include "../includes/OgreInclude.h"

#include "../gamemodes/ModeContext.h"
#include "../GameState.h"

#include "LapController.h"

#include "../tools/Randomizer.h"

finishBoardVec FinishBoard::prepareFinishBoard(const ModeContext& modeContext)
{
    finishBoardVec ret;

    LapController lapController = modeContext.getLapController();
    size_t lap = modeContext.getGameState().getPlayerCar().getLapUtils().getCurrentLap() - 1;

    bool isPlayerFinishRace = false;

    if(lap == modeContext.getGameState().getLapsCount())//race finished
    {
        isPlayerFinishRace = true;
    }

    for(size_t q = 0; q < modeContext.getGameState().getAICountInRace(); ++q)
    {
        ret.push_back(finishBoardElement(
            isPlayerFinishRace ? lapController.getTotalPosition(q + 1) : q, 
            false, 
            isPlayerFinishRace ? modeContext.getGameState().getAICar(q).getLapUtils().getTotalTime() : 0.0f,
            modeContext.getGameState().getAICar(q).getLapUtils().getBestLapTime(),
            modeContext.getGameState().getAICar(q).getCharacterName()));
    }

    //player
    ret.push_back(finishBoardElement(
        isPlayerFinishRace ? lapController.getTotalPosition(0) : modeContext.getGameState().getAICountInRace(), 
        true, 
        isPlayerFinishRace ? modeContext.getGameState().getPlayerCar().getLapUtils().getTotalTime() : 0.0f,
        modeContext.getGameState().getPlayerCar().getLapUtils().getBestLapTime(),
        modeContext.getGameState().getPlayerCar().getCharacterName()
        ));

    if(isPlayerFinishRace)
    {
        std::sort(ret.begin(), ret.end());

        finishBoardVec::const_iterator i = std::find_if(ret.begin(), ret.end(), finishBoardElement::findPlayer);
        size_t playerIndexAfterSort = i - ret.begin();
        for(size_t q = playerIndexAfterSort + 1; q < ret.size(); ++q)
        {
            ret[q].mRaceTime = ret[q - 1].mRaceTime + Randomizer::GetInstance().GetRandomFloat(1.0f, 10.0f);
            if(ret[q].mBestLapTime == 0.0f)
            {
                if(modeContext.getGameState().getLapsCount() == 1)
                    ret[q].mBestLapTime = ret[q].mRaceTime;
                else
                    ret[q].mBestLapTime = ret[q - 1].mBestLapTime + Randomizer::GetInstance().GetRandomFloat(1.0f, 2.0f);
            }
        }
    }
    else
    {
        for(size_t q = 0; q < ret.size() - 1; ++q)
        {
            if(ret[q].mBestLapTime == 0.0f)
            {
                const STRRacetimes& raceTimes = modeContext.getGameState().getSTRRacetimes();
                Ogre::Vector2 lapTime = raceTimes.getArray2Value(modeContext.getGameState().getTrackName() + " times", ret[q].mCharName + " lap times");
                
                float normalLapTime = lapTime.x;
                
                if(modeContext.getGameState().getAIStrength() == Easy)
                    normalLapTime = lapTime.y;

                ret[q].mBestLapTime = normalLapTime + Randomizer::GetInstance().GetRandomFloat(normalLapTime / 20.0f, normalLapTime / 10.0f);
                ret[q].mRaceTime = modeContext.getGameState().getLapsCount() * (normalLapTime + Randomizer::GetInstance().GetRandomFloat(normalLapTime / 10.0f, normalLapTime / 5.0f));
            }
            else
            {
                ret[q].mRaceTime = modeContext.getGameState().getLapsCount() * ret[q].mBestLapTime + Randomizer::GetInstance().GetRandomFloat(ret[q].mBestLapTime / 20.0f, ret[q].mBestLapTime / 10.0f);
            }
        }

        std::sort(ret.begin(), ret.end() - 1, finishBoardElement::sortByRaceTime);

        for(size_t q = 0; q < ret.size() - 1; ++q)
            ret[q].mPos = q;
    }

    return ret;
}