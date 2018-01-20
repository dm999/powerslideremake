
#ifndef NO_MULTIPLAYER

#include "MultiPlayerMode.h"

#include "../InputHandler.h"

#include "../customs/CustomTrayManager.h"

#include "../tools/OgreTools.h"

#include "../ui/UIRace.h"
#include "../ui/UIRaceMulti.h"

#include "../physics/PhysicsVehicle.h"

class MultiplayerAILapFinishController : public LapUtils::Events
{
public:

    MultiplayerAILapFinishController(GameState& gameState, MultiPlayerMode * multiPlayerMode, PSAICar& aiCar) : 
      mGameState(gameState),
      mMultiPlayerMode(multiPlayerMode),
      mAICar(aiCar){}

    void onLapFinished()override;

private:

    GameState& mGameState;

    MultiPlayerMode * mMultiPlayerMode;

    PSAICar& mAICar;
};


void MultiplayerAILapFinishController::onLapFinished()
{
    size_t lap = mAICar.getLapUtils().getCurrentLap() - 1;

    //race finished by AI
    if(lap == mGameState.getLapsCount())
    {
        mMultiPlayerMode->onRaceFinishedByAI(mAICar);
    }
}

MultiPlayerMode::MultiPlayerMode(const ModeContext& modeContext, const CommonIncludes::shared_ptr<MultiplayerController>& controller) :
    BaseRaceMode(modeContext),
    mIsSessionStarted(false),
    mIsSelfFinished(false)
{

    mUIRaceMulti.reset(new UIRaceMulti(modeContext, this));

    assert(controller.get());

    mMultiplayerController = controller;
    if(mMultiplayerController.get())
        mMultiplayerController->setEvents(this);
}

void MultiPlayerMode::initCamera()
{

    BaseRaceMode::initCamera();

    Ogre::int32 aiCamIndex = mLuaManager.ReadScalarInt("Scene.AICamIndex", mModeContext.mPipeline);
    bool isCamToAI = mLuaManager.ReadScalarBool("Scene.IsCamToAI", mModeContext.mPipeline);

    if(isCamToAI)
        mModeContext.mGameState.getMultiplayerCarAI(aiCamIndex).setCameraMan(mCameraMan.get());
}

void MultiPlayerMode::onLapFinished()
{
    size_t lap = mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap() - 1;

    if(lap <= mModeContext.mGameState.getLapsCount())
    {
        std::string time = Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLastLapTime());
        std::string lapS = Conversions::DMToString(lap);
        mUIRace->addMiscPanelText("Lap finished " + Ogre::String(lapS) + ": [" + Ogre::String(time) + "]", mModeContext.mGameState.getSTRPowerslide().getTrackTimeTrialColor(mModeContext.mGameState.getTrackName()));
    }

    //final self lap finished
    if(lap == mModeContext.mGameState.getLapsCount())
    {

        mSelfTotalRaceTime = mModeContext.mGameState.getPlayerCar().getLapUtils().getTotalTime();

        MultiplayerLobbyData multiplayerLobbyData(lobbyDataLapTime, mSelfTotalRaceTime);

        bool success = mMultiplayerController->sendLobbyMessage(multiplayerLobbyData, false, 10);

        if(success)
        {
            std::string totalTime = Tools::SecondsToString(mSelfTotalRaceTime);
            mUIRace->addMiscPanelText("Final lap finished: [" + Ogre::String(totalTime) + "]", mModeContext.mGameState.getSTRPowerslide().getTrackTimeTrialColor(mModeContext.mGameState.getTrackName()));

            //check all other humans as well
            mIsSelfFinished = true;
            checkRaceFinished();
        }
        else
        {
            mUIRace->addMiscPanelText("Error sending finish data!!!", Ogre::ColourValue::Red);
        }
    }
}

void MultiPlayerMode::onRaceFinishedByAI(PSAICar& aiCar)
{
    Ogre::Real aiTotalRaceTime = aiCar.getLapUtils().getTotalTime();

    MultiplayerLobbyData multiplayerLobbyData(lobbyDataLapTimeAI, aiTotalRaceTime);

    bool success = mMultiplayerController->sendLobbyMessage(multiplayerLobbyData, true, 10);
}

void MultiPlayerMode::customInitScene()
{
    if(!mModeContext.mGameState.isMultiplayerMaster())
    {
        mModeContext.mGameState.setAICount(0);
    }
}

void MultiPlayerMode::customClearScene()
{
    //AI
    for(size_t q = 0; q < mModeContext.mGameState.getMultiplayerCountAI(); ++q)
    {
        mModeContext.mGameState.getMultiplayerCarAI(q).clear();
        mModeContext.mGameState.getMultiplayerCarAI(q).deinitSounds();
    }

    //human
    std::vector<std::string> playerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
    for(size_t q = 0; q < playerNames.size(); ++q)
    {
        mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).clear();
        mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).deinitSounds();
    }
}

void MultiPlayerMode::customInitUI()
{
    /*
    mModeContext.mPlatform->initialise(mModeContext.mWindow, mSceneMgr);
    mModeContext.mGUI->initialise();

    MyGUI::PointerManager::getInstance().setVisible(false);
*/
    mUIRaceMulti->load(mModeContext.mTrayMgr, mModeContext.mGameState);
}

void MultiPlayerMode::customClearUI()
{
    /*
    mModeContext.mGUI->shutdown();
    mModeContext.mPlatform->shutdown();
    */
}

void MultiPlayerMode::customProcessCollision(int triIndex)
{
    if(mIsSessionStarted)
    {
#if 0
        //AI
        for(size_t q = 0; q < mModeContext.mGameState.getMultiplayerCountAI(); ++q)
        {
            mModeContext.mGameState.getMultiplayerCarAI(q).processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mWorld.get(), mStaticMeshProcesser, triIndex);
        }

        //human
        std::vector<std::string> playerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
        for(size_t q = 0; q < playerNames.size(); ++q)
        {
            mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mWorld.get(), mStaticMeshProcesser, triIndex);
        }
#endif
    }
}

void MultiPlayerMode::customUnloadResources()
{
    //AI
    for(size_t q = 0; q < mModeContext.mGameState.getMultiplayerCountAI(); ++q)
        mModeContext.mGameState.getMultiplayerCarAI(q).deinitSounds();

    //human
    std::vector<std::string> playerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
    for(size_t q = 0; q < playerNames.size(); ++q)
    {
        mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).deinitSounds();
    }
}

void MultiPlayerMode::customFrameStartedDoProcessFrameBeforePhysics()
{
    if(mIsSessionStarted)
    {
        //AI
        for(size_t q = 0; q < mModeContext.mGameState.getMultiplayerCountAI(); ++q)
        {
            mModeContext.mGameState.getMultiplayerCarAI(q).processFrameBeforePhysics(mStaticMeshProcesser, mModeContext.mGameState.getRaceStarted());

            if(!mModeContext.mGameState.getRaceFinished())
                mModeContext.mGameState.getMultiplayerCarAI(q).getLapUtils().checkCheckPoints(mModeContext.mGameState.getMultiplayerCarAI(q).getModelNode()->getPosition());
        }

        //human
        std::vector<std::string> playerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
        for(size_t q = 0; q < playerNames.size(); ++q)
        {
            if(!mModeContext.mGameState.getRaceFinished())
                mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).getLapUtils().checkCheckPoints(mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).getModelNode()->getPosition());

            mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).processFrameBeforePhysics(mStaticMeshProcesser, mModeContext.mGameState.getRaceStarted());
        }
    }
}

void MultiPlayerMode::customFrameStartedDoProcessFrameAfterPhysics()
{

    if(mMultiplayerController.get())
    {
        mMultiplayerController->receiveData();

        if(mIsSessionStarted)
        {
            Ogre::Vector3 playerPos = mModeContext.mGameState.getPlayerCar().getPhysicsVehicle()->getVehicleSetup().mCarGlobalPos;

            Ogre::Vector3 playerVel = mModeContext.mGameState.getPlayerCar().getLinearImpulse();
            Ogre::Vector3 playerAngVel = mModeContext.mGameState.getPlayerCar().getAngularImpulse();
            Ogre::Quaternion playerRot = mModeContext.mGameState.getPlayerCar().getPhysicsVehicle()->getVehicleSetup().mCarRot;

            MultiplayerSessionData data;
            data.pos = playerPos;
            data.rot = playerRot;
            data.vel = playerVel;
            data.velang = playerAngVel;
            data.isAcc = mModeContext.mGameState.getPlayerCar().getAcceleration();
            data.isBrake = mModeContext.mGameState.getPlayerCar().getBrake();
            data.steering = mModeContext.mGameState.getPlayerCar().getPhysicsVehicle()->getOriginalSteering();
            data.currentLap = mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap();
            data.lapPosition = mModeContext.mGameState.getPlayerCar().getLapUtils().getLapPosition();

            std::vector<MultiplayerSessionData> dataAI;
            for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
            {
                Ogre::Vector3 aiVel = mModeContext.mGameState.getAICar(q).getLinearImpulse();
                Ogre::Vector3 aiAngVel = mModeContext.mGameState.getAICar(q).getAngularImpulse();
                Ogre::Quaternion aiRot = mModeContext.mGameState.getAICar(q).getPhysicsVehicle()->getVehicleSetup().mCarRot;
                Ogre::Vector3 aiPos = mModeContext.mGameState.getAICar(q).getPhysicsVehicle()->getVehicleSetup().mCarGlobalPos;

                MultiplayerSessionData aiData;
                aiData.pos = aiPos;
                aiData.rot = aiRot;
                aiData.vel = aiVel;
                aiData.velang = aiAngVel;

                aiData.isAcc = mModeContext.mGameState.getAICar(q).getAcceleration();
                aiData.isBrake = mModeContext.mGameState.getAICar(q).getBrake();
                aiData.steering = mModeContext.mGameState.getAICar(q).getPhysicsVehicle()->getOriginalSteering();

                aiData.currentLap = mModeContext.mGameState.getAICar(q).getLapUtils().getCurrentLap();
                aiData.lapPosition = mModeContext.mGameState.getAICar(q).getLapUtils().getLapPosition();

                dataAI.push_back(aiData);
            }

            mMultiplayerController->updateSessionData(data, dataAI);
        }
    }
}

void MultiPlayerMode::customFrameRenderingQueuedDoBegining()
{
    if(mMultiplayerController.get())
    {
        if(!mMultiplayerController->getStartHappen())
        {
            mModeContext.mGameState.resetBeforeStartTimer();
        }

        mUIRace->setShowMiscText(true);
        size_t pingSession = mMultiplayerController->getSessionPing();
        size_t ping = mMultiplayerController->getLobbyPing();
        mUIRace->setMiscText(Conversions::DMToString(ping) + "/" + Conversions::DMToString(pingSession), pingToColor(pingSession));
    }
}

void MultiPlayerMode::customFrameRenderingQueuedDoRaceStarted()
{
    //AI
    for(size_t q = 0; q < mModeContext.mGameState.getMultiplayerCountAI(); ++q)
    {
        mModeContext.mGameState.getMultiplayerCarAI(q).raceStarted();
    }

    //human
    std::vector<std::string> playerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
    for(size_t q = 0; q < playerNames.size(); ++q)
    {
        mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).raceStarted();
    }
}

void MultiPlayerMode::customFrameRenderingQueuedDo2DUI()
{
    size_t currentPlayerLap = mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap();
    Ogre::Real currentPlayerLapPos = mModeContext.mGameState.getPlayerCar().getLapUtils().getLapPosition();

    //AI
    for(size_t q = 0; q < mModeContext.mGameState.getMultiplayerCountAI(); ++q)
    {
        mUIRace->setAIDashBoardSkin(mModeContext.mGameState, mModeContext.mGameState.getAICount() + q, mModeContext.mGameState.getMultiplayerCarAI(q).getCharacterName());
        mUIRace->setDashCarPos(mModeContext.mGameState.getAICount() + q, currentPlayerLap, currentPlayerLapPos, mModeContext.mGameState.getMultiplayerCarAI(q).getCurrentLap(), mModeContext.mGameState.getMultiplayerCarAI(q).getLapUtils().getLapPosition());
    }

    //human
    std::vector<std::string> playerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
    for(size_t q = 0; q < playerNames.size(); ++q)
    {
        mUIRace->setAIDashBoardSkin(mModeContext.mGameState, mModeContext.mGameState.getAICount() + mModeContext.mGameState.getMultiplayerCountAI() + q, mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).getCharacterName());
        mUIRace->setDashCarPos(mModeContext.mGameState.getAICount() + mModeContext.mGameState.getMultiplayerCountAI() + q, currentPlayerLap, currentPlayerLapPos, mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).getCurrentLap(), mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).getLapUtils().getLapPosition());
    }
}

void MultiPlayerMode::onPlayerEjected(const std::string& player)
{
    mUIRace->addMiscPanelText("Player ejected: [" + player + "]", Ogre::ColourValue::Red);
}

void MultiPlayerMode::onPlayerJoined(const std::string& player)
{
    mUIRace->addMiscPanelText("Player joined lobby: [" + player + "]", Ogre::ColourValue::Blue);
}

void MultiPlayerMode::onPlayerLeft(const std::string& player)
{
    mUIRace->addMiscPanelText("Player left lobby: [" + player + "]", Ogre::ColourValue::Red);
}

void MultiPlayerMode::onNewHost(const std::string& player)
{
    mUIRace->addMiscPanelText("New host: [" + player + "]", Ogre::ColourValue::Red);
}

void MultiPlayerMode::onRoomClosed(const std::string& player)
{
    mUIRace->addMiscPanelText("Room closed: [" + player + "]", Ogre::ColourValue::Red);
}

void MultiPlayerMode::onLobbyMessage(const std::string& player, const MultiplayerLobbyData& data)
{
    if(data.mDataType == lobbyDataRegular)
    {
        if(!data.mPlayerMessage.empty())
            mUIRace->addMiscPanelText(player + ": " + data.mPlayerMessage, Ogre::ColourValue::Green);
    }

    //check other players finished all laps
    if(data.mDataType == lobbyDataLapTime)
    {
        std::string time = Tools::SecondsToString(data.mRaceTotalTime);
        mUIRace->addMiscPanelText("Race finished by [" + player + "]: " + Ogre::String(time), mModeContext.mGameState.getSTRPowerslide().getTrackTimeTrialColor(mModeContext.mGameState.getTrackName()));

        mIsRaceFinishedByHuman[player] = true;
        mRaceTimeByHuman[player] = data.mRaceTotalTime;
        checkRaceFinished();
    }

    if(data.mDataType == lobbyDataLapTimeAI)
    {
        std::string time = Tools::SecondsToString(data.mRaceTotalTime);
        mUIRace->addMiscPanelText("Race finished by AI: " + Ogre::String(time), mModeContext.mGameState.getSTRPowerslide().getTrackTimeTrialColor(mModeContext.mGameState.getTrackName()));
        mRaceTimeByAI.push_back(data.mRaceTotalTime);
    }
}

void MultiPlayerMode::onPlayerAddedToSession(const std::string& player)
{
    mUIRace->addMiscPanelText("Player added to session: [" + player + "]");
}

void MultiPlayerMode::onPlayerQuitSession(const std::string& player, bool isHost)
{
    //isHost - true if remote player was host (not current host!!!)

    //if(player != mModeContext.mGameState.getMultiplayerUserName())//if not self
    {
        if(!isHost)
            mUIRace->addMiscPanelText("Player quit session: [" + player + "]", Ogre::ColourValue::Red);
        else
            mUIRace->addMiscPanelText("Host player quit session: [" + player + "]", Ogre::ColourValue::Red);
        
        //remove human
        std::vector<std::string>multiplayerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
        if(std::find(multiplayerNames.begin(), multiplayerNames.end(), player) != multiplayerNames.end())
        {
            PSMultiplayerCar& carHuman = mModeContext.mGameState.getMultiplayerCarHuman(player);
            mLapController.removeCar(&carHuman);
            carHuman.removeFromScene(mSceneMgr);
            carHuman.deinitSounds();

            //remove PSMultiplayerCar object
            mModeContext.mGameState.removeMultiplayerCarHuman(player);
        }

        //remove ai
        if(isHost)
        {
            for(size_t q = 0; q < mModeContext.mGameState.getMultiplayerCountAI(); ++q)
            {
                PSMultiplayerCar& carAI = mModeContext.mGameState.getMultiplayerCarAI(q);
                mLapController.removeCar(&carAI);
                carAI.removeFromScene(mSceneMgr);
                carAI.deinitSounds();
            }
            mModeContext.mGameState.setMultiplayerCountAI(0);

            //mRaceTimeByAI.clear();
        }
    }

    if(!mIsRaceFinishedByHuman[player])
    {
        mIsRaceFinishedByHuman.erase(player);
        mRaceTimeByHuman.erase(player);
    }
    checkRaceFinished();
}

void MultiPlayerMode::prepareDataForSession(const MultiplayerSessionStartInfo& sessionStartInfo)
{
    //hack to enable correct steering
    InputType originalInput = mModeContext.mGameState.getInputType();
    mModeContext.mGameState.setInputType(itMouse);

    size_t aiCount = sessionStartInfo.mAISkins.size();

    //playerIndex should start from 0
    mModeContext.mGameState.getPlayerCar().repositionVehicle(mModeContext.mGameState.getInitialVehicleSetup()[aiCount + sessionStartInfo.mPlayerIndex].mTrackPosition);

    mModeContext.mGameState.clearMultiplayerCarsHuman();
    mModeContext.mGameState.setMultiplayerCountAI(0);

    Ogre::int32 aiCamIndex = mLuaManager.ReadScalarInt("Scene.AICamIndex", mModeContext.mPipeline);
    bool isCamToAI = mLuaManager.ReadScalarBool("Scene.IsCamToAI", mModeContext.mPipeline);

    if(sessionStartInfo.mIsHost)
    {
        //only human cars
        for(size_t q = 0; q < sessionStartInfo.mPlayers.size(); ++q)
        {
            if(q != sessionStartInfo.mPlayerIndex)// minus self
            {
                std::string humanCharacter = "frantic";
                std::map<std::string, std::string>::const_iterator foundPlayerSkin = sessionStartInfo.mPlayersSkins.find(sessionStartInfo.mPlayers[q]);
                if(foundPlayerSkin != sessionStartInfo.mPlayersSkins.end())
                {
                    humanCharacter = (*foundPlayerSkin).second;
                }

                mModeContext.mGameState.addMultiplayerCarHuman(sessionStartInfo.mPlayers[q]);

                PSMultiplayerCar& humanCar = mModeContext.mGameState.getMultiplayerCarHuman(sessionStartInfo.mPlayers[q]);

                humanCar.setCharacterName(humanCharacter);

                humanCar.initModel(mModeContext.mPipeline, mModeContext.mGameState, mSceneMgr, mMainNode, &mModelsPool, mWorld.get(), humanCharacter, 
                    mModeContext.mGameState.getInitialVehicleSetup()[q], 
                    false, sessionStartInfo.mPlayers[q], true);
                humanCar.initSounds(mModeContext.mPipeline, mModeContext.mGameState);

                humanCar.repositionVehicle(mModeContext.mGameState.getInitialVehicleSetup()[aiCount + q].mTrackPosition);

                mLapController.addCar(&humanCar);

                mIsRaceFinishedByHuman.insert(std::make_pair(sessionStartInfo.mPlayers[q], false));
                mRaceTimeByHuman.insert(std::make_pair(sessionStartInfo.mPlayers[q], 0.0f));
            }
        }

        //add AI cars laps listener
        for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        {
            PSAICar& aiCar = mModeContext.mGameState.getAICar(q);
            CommonIncludes::shared_ptr<MultiplayerAILapFinishController> humanLapsController(new MultiplayerAILapFinishController(mModeContext.mGameState, this, aiCar));
            mAILapsController.push_back(humanLapsController);
            aiCar.getLapUtils().setEvents(humanLapsController.get());
        }

    }
    else
    {
        //human
        for(size_t q = 0; q < sessionStartInfo.mPlayers.size(); ++q)
        {
            if(q != sessionStartInfo.mPlayerIndex)// minus self
            {
                std::string humanCharacter = "frantic";
                std::map<std::string, std::string>::const_iterator foundPlayerSkin = sessionStartInfo.mPlayersSkins.find(sessionStartInfo.mPlayers[q]);
                if(foundPlayerSkin != sessionStartInfo.mPlayersSkins.end())
                {
                    humanCharacter = (*foundPlayerSkin).second;
                }

                mModeContext.mGameState.addMultiplayerCarHuman(sessionStartInfo.mPlayers[q]);

                PSMultiplayerCar& humanCar = mModeContext.mGameState.getMultiplayerCarHuman(sessionStartInfo.mPlayers[q]);

                humanCar.setCharacterName(humanCharacter);

                humanCar.initModel(mModeContext.mPipeline, mModeContext.mGameState, mSceneMgr, mMainNode, &mModelsPool, mWorld.get(), humanCharacter, 
                    mModeContext.mGameState.getInitialVehicleSetup()[q], 
                    !isCamToAI, sessionStartInfo.mPlayers[q], true);
                humanCar.initSounds(mModeContext.mPipeline, mModeContext.mGameState);

                humanCar.repositionVehicle(mModeContext.mGameState.getInitialVehicleSetup()[aiCount + q].mTrackPosition);

                mLapController.addCar(&humanCar);

                mIsRaceFinishedByHuman.insert(std::make_pair(sessionStartInfo.mPlayers[q], false));
                mRaceTimeByHuman.insert(std::make_pair(sessionStartInfo.mPlayers[q], 0.0f));
            }
        }

        //AI
        mModeContext.mGameState.setMultiplayerCountAI(aiCount);

        for(size_t q = 0; q < aiCount; ++q)
        {
            std::string aiCharacter = "frantic";
            if(sessionStartInfo.mAISkins.size() > q)
            {
                aiCharacter = sessionStartInfo.mAISkins[q];
            }

            mModeContext.mGameState.getMultiplayerCarAI(q).setCharacterName(aiCharacter);

            bool isCam = (q == aiCamIndex);
            if(!isCamToAI)
                isCam = false;

            mModeContext.mGameState.getMultiplayerCarAI(q).initModel(mModeContext.mPipeline, mModeContext.mGameState, mSceneMgr, mMainNode, &mModelsPool, mWorld.get(), aiCharacter, 
                mModeContext.mGameState.getInitialVehicleSetup()[q], 
                isCam, "", false);

            mModeContext.mGameState.getMultiplayerCarAI(q).initSounds(mModeContext.mPipeline, mModeContext.mGameState);

            mModeContext.mGameState.getMultiplayerCarAI(q).repositionVehicle(mModeContext.mGameState.getInitialVehicleSetup()[q].mTrackPosition);

            mLapController.addCar(&mModeContext.mGameState.getMultiplayerCarAI(q));
        }
    }

    //hack to enable correct steering
    mModeContext.mGameState.setInputType(originalInput);

    mIsSessionStarted = true;
}

void MultiPlayerMode::tabPressed()
{
    mUIRaceMulti->switchVisibleMessageWidget();
}

void MultiPlayerMode::onSessionUpdate(const playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost)
{
    const float posDiffMax = 100.0f;

    if(!isHost)
    {
        //AI
        if(mModeContext.mGameState.getMultiplayerCountAI() == aiPlayersSessionData.size())
        {
            for(size_t q = 0; q < aiPlayersSessionData.size(); ++q)
            {
                PSMultiplayerCar& aiCar = mModeContext.mGameState.getMultiplayerCarAI(q);

                if(aiCar.getLastTimeOfUpdate() < aiPlayersSessionData[q].dataUpdateTimestamp)
                {
                    const Ogre::Real carMass = aiCar.getPhysicsVehicle()->getVehicleSetup().mChassisMass;

                    aiCar.setLastTimeOfUpdate(aiPlayersSessionData[q].dataUpdateTimestamp);

                    Ogre::Vector3 aiPos = aiCar.getPhysicsVehicle()->getVehicleSetup().mCarGlobalPos;

                    if(aiPos.distance(aiPlayersSessionData[q].pos) > posDiffMax)
                    {
                        aiCar.repositionVehicle(aiPlayersSessionData[q].pos, aiPlayersSessionData[q].rot);
                        aiCar.setModelImpulse(aiPlayersSessionData[q].vel, aiPlayersSessionData[q].velang);
                    }
                    else
                    {
                        Ogre::Vector3 posDiff = aiPlayersSessionData[q].pos - aiPos;
                        posDiff.z = -posDiff.z;//original data is left hand
                        Ogre::Vector3 velocityAddition = posDiff / (mModeContext.mGameState.getMultiplayerBroadcastInterval() / 1000.0f);

                        Ogre::Vector3 angleDiff = GetAngleBetweenQuaternions(aiCar.getPhysicsVehicle()->getVehicleSetup().mCarRot, aiPlayersSessionData[q].rot);
                        angleDiff.z = -angleDiff.z;//original data is left hand
                        Ogre::Vector3 angularVelocityAddition = angleDiff / (mModeContext.mGameState.getMultiplayerBroadcastInterval() / 1000.0f);

                        //aiCar.setModelImpulse(aiPlayersSessionData[q].vel + velocityAddition * carMass, aiPlayersSessionData[q].velang + angularVelocityAddition * carMass);
                        //aiCar.setModelImpulse(aiPlayersSessionData[q].vel + velocityAddition * carMass, aiPlayersSessionData[q].velang);
                        aiCar.setModelImpulse(aiPlayersSessionData[q].vel + velocityAddition, aiPlayersSessionData[q].velang + angularVelocityAddition * carMass);
                        //aiCar.setModelImpulse(aiPlayersSessionData[q].vel, aiPlayersSessionData[q].velang);
                        //aiCar.setModelImpulse(aiPlayersSessionData[q].vel + aiPlayersSessionData[q].linImpInc, aiPlayersSessionData[q].velang + aiPlayersSessionData[q].angImpInc);
                    }

                    aiCar.setAcceleration(aiPlayersSessionData[q].isAcc);
                    aiCar.setBrake(aiPlayersSessionData[q].isBrake);
                    aiCar.getPhysicsVehicle()->setSteering(aiPlayersSessionData[q].steering);
                    aiCar.setCurrentLap(aiPlayersSessionData[q].currentLap);
                    aiCar.setLapPosition(aiPlayersSessionData[q].lapPosition);
                }
            }
        }


        //human
        for(playerToData::const_iterator i = otherPlayersSessionData.begin(), j = otherPlayersSessionData.end(); 
            i != j; ++i)
        {
            PSMultiplayerCar& humanCar = mModeContext.mGameState.getMultiplayerCarHuman((*i).first);

            if(humanCar.getLastTimeOfUpdate() < (*i).second.dataUpdateTimestamp)
            {
                const Ogre::Real carMass = humanCar.getPhysicsVehicle()->getVehicleSetup().mChassisMass;

                humanCar.setLastTimeOfUpdate((*i).second.dataUpdateTimestamp);

                Ogre::Vector3 humanPos = humanCar.getPhysicsVehicle()->getVehicleSetup().mCarGlobalPos;

                if(humanPos.distance((*i).second.pos) > posDiffMax)
                {
                    humanCar.repositionVehicle((*i).second.pos, (*i).second.rot);
                    humanCar.setModelImpulse((*i).second.vel, (*i).second.velang);
                }
                else
                {
                    Ogre::Vector3 posDiff = (*i).second.pos - humanPos;
                    posDiff.z = -posDiff.z;//original data is left hand
                    Ogre::Vector3 velocityAddition = posDiff / (mModeContext.mGameState.getMultiplayerBroadcastInterval() / 1000.0f);

                    Ogre::Vector3 angleDiff = GetAngleBetweenQuaternions(humanCar.getPhysicsVehicle()->getVehicleSetup().mCarRot, (*i).second.rot);
                    angleDiff.z = -angleDiff.z;//original data is left hand
                    Ogre::Vector3 angularVelocityAddition = angleDiff / (mModeContext.mGameState.getMultiplayerBroadcastInterval() / 1000.0f);

                    //humanCar.setModelImpulse((*i).second.vel + velocityAddition * carMass, (*i).second.velang + angularVelocityAddition * carMass);
                    humanCar.setModelImpulse((*i).second.vel + velocityAddition, (*i).second.velang + angularVelocityAddition * carMass);
                    //humanCar.setModelImpulse((*i).second.vel, (*i).second.velang);
                    //humanCar.setModelImpulse((*i).second.vel + (*i).second.linImpInc, (*i).second.velang + (*i).second.angImpInc);
                }

                humanCar.setAcceleration((*i).second.isAcc);
                humanCar.setBrake((*i).second.isBrake);
                humanCar.getPhysicsVehicle()->setSteering((*i).second.steering);
                humanCar.setCurrentLap((*i).second.currentLap);
                humanCar.setLapPosition((*i).second.lapPosition);
            }
        }
    }
    else
    {
        //human only
        for(playerToData::const_iterator i = otherPlayersSessionData.begin(), j = otherPlayersSessionData.end(); 
            i != j; ++i)
        {
            PSMultiplayerCar& humanCar = mModeContext.mGameState.getMultiplayerCarHuman((*i).first);

            if(humanCar.getLastTimeOfUpdate() < (*i).second.dataUpdateTimestamp)
            {
                const Ogre::Real carMass = humanCar.getPhysicsVehicle()->getVehicleSetup().mChassisMass;

                humanCar.setLastTimeOfUpdate((*i).second.dataUpdateTimestamp);

                Ogre::Vector3 humanPos = humanCar.getPhysicsVehicle()->getVehicleSetup().mCarGlobalPos;

                if(humanPos.distance((*i).second.pos) > posDiffMax)
                {
                    humanCar.repositionVehicle((*i).second.pos, (*i).second.rot);
                    humanCar.setModelImpulse((*i).second.vel, (*i).second.velang);
                }
                else
                {
                    Ogre::Vector3 posDiff = (*i).second.pos - humanPos;
                    posDiff.z = -posDiff.z;//original data is left hand
                    Ogre::Vector3 velocityAddition = posDiff / (mModeContext.mGameState.getMultiplayerBroadcastInterval() / 1000.0f);

                    Ogre::Vector3 angleDiff = GetAngleBetweenQuaternions(humanCar.getPhysicsVehicle()->getVehicleSetup().mCarRot, (*i).second.rot);
                    angleDiff.z = -angleDiff.z;//original data is left hand
                    Ogre::Vector3 angularVelocityAddition = angleDiff / (mModeContext.mGameState.getMultiplayerBroadcastInterval() / 1000.0f);

                    //humanCar.setModelImpulse((*i).second.vel + velocityAddition * carMass, (*i).second.velang + angularVelocityAddition * carMass);
                    humanCar.setModelImpulse((*i).second.vel + velocityAddition, (*i).second.velang + angularVelocityAddition * carMass);
                    //humanCar.setModelImpulse((*i).second.vel, (*i).second.velang);
                    //humanCar.setModelImpulse((*i).second.vel + (*i).second.linImpInc, (*i).second.velang + (*i).second.angImpInc);
                }

                humanCar.setAcceleration((*i).second.isAcc);
                humanCar.setBrake((*i).second.isBrake);
                humanCar.getPhysicsVehicle()->setSteering((*i).second.steering);
                humanCar.setCurrentLap((*i).second.currentLap);
                humanCar.setLapPosition((*i).second.lapPosition);
            }
        }
    }
}

void MultiPlayerMode::onError(const std::string& message)
{
    mUIRace->addMiscPanelText("Error: [" + message + "]");
}

void MultiPlayerMode::checkRaceFinished()
{
    bool allFinished = mIsSelfFinished;

    if(mIsSelfFinished)
    {
        for(std::map<std::string, bool>::const_iterator i = mIsRaceFinishedByHuman.begin(), j = mIsRaceFinishedByHuman.end();
            i != j; ++i)
        {
            if(!(*i).second)
            {
                allFinished = false;
                break;
            }
        }
    }

    if(allFinished && !mModeContext.mGameState.getRaceFinished())
    {
        mModeContext.mGameState.resetAfterFinishTimer();

        Ogre::Real raceTime = mSelfTotalRaceTime;

        std::vector<std::pair<Ogre::Real, bool> > timeToSelf;
        timeToSelf.push_back(std::make_pair(mSelfTotalRaceTime, true));

        //take whole race time as slowest human player
        for(std::map<std::string, Ogre::Real>::const_iterator i = mRaceTimeByHuman.begin(), j = mRaceTimeByHuman.end();
            i != j; ++i)
        {
            Ogre::Real otherRaceTime = (*i).second;
            if(otherRaceTime > raceTime)raceTime = otherRaceTime;

            timeToSelf.push_back(std::make_pair(otherRaceTime, false));
        }

        //add AI times
        for(size_t q = 0; q < mRaceTimeByAI.size(); ++q)
        {
            timeToSelf.push_back(std::make_pair(mRaceTimeByAI[q], false));
        }

        std::sort(timeToSelf.begin(), timeToSelf.end());

        size_t selfPosition = 0;
        for(size_t q = 0; q < timeToSelf.size(); ++q)
        {
            if(timeToSelf[q].second)
            {
                selfPosition = q;
                break;
            }
        }

        std::string totalTime = Tools::SecondsToString(raceTime);
        mUIRace->addMiscPanelText("Race finished: [" + Ogre::String(totalTime) + "]", mModeContext.mGameState.getSTRPowerslide().getTrackTimeTrialColor(mModeContext.mGameState.getTrackName()));

        mModeContext.mGameState.getPlayerCar().setDisableMouse(false);
        mCamera->setPosition(mModeContext.mGameState.getSTRPowerslide().getFinishCameraPos(mModeContext.mGameState.getTrackName()));
        mUIRace->setRearViewMirrorPanelShow(false);
        mUIRace->setVisibleFinishSign(true, selfPosition + 1);

        mModeContext.mGameState.setRaceFinished(true);
    }
}

#endif