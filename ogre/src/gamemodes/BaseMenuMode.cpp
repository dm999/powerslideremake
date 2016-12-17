#include "../pcheader.h"

#include "BaseMenuMode.h"

#include "../gamelogic/RacingGridGeneration.h"

BaseMenuMode::BaseMenuMode(const ModeContext& modeContext) :
    BaseMode(modeContext)
{
    mModeContext.getGameState().getPlayerCar().setCharacterName("frantic");
    mModeContext.getGameState().setRaceParameters("dam", Insane);
    mModeContext.getGameState().setAICount(1);                          //not more than 11

    mModeContext.getGameState().setMultiplayerServerPort(8800);
    mModeContext.getGameState().setMultiplayerPlayersLimits(2);         //for master only
    mModeContext.getGameState().setMultiplayerBroadcastInterval(150);   //in ms; 50 for fast; 150 for slow

    mModeContext.getGameState().setListenerGain(0.0f);

    std::vector<std::string> aiCharacters = RacingGridGeneration().generate(mModeContext.getGameState());
    mModeContext.getGameState().setAICharacters(aiCharacters);
    for(size_t q = 0; q < mModeContext.getGameState().getAICount(); ++q)
    {
        mModeContext.getGameState().getAICar(q).setCharacterName(aiCharacters[q]);
    }
}