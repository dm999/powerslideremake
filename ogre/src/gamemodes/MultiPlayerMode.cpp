#include "../pcheader.h"

#include "MultiPlayerMode.h"

#include "../InputHandler.h"

#include "../customs/CustomTrayManager.h"

#include "../ui/UIRace.h"

MultiPlayerMode::MultiPlayerMode(const ModeContext& modeContext, const CommonIncludes::shared_ptr<MultiplayerController>& controller) :
    BaseRaceMode(modeContext),
    mIsSessionStarted(false)
{
    assert(controller.get());

    mMultiplayerController = controller;
    if(mMultiplayerController.get())
        mMultiplayerController->setEvents(this);
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

void MultiPlayerMode::customProcessCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex)
{
    if(mIsSessionStarted)
    {
        //AI
        for(size_t q = 0; q < mModeContext.mGameState.getMultiplayerCountAI(); ++q)
        {
            mModeContext.mGameState.getMultiplayerCarAI(q).processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);
        }

        //human
        std::vector<std::string> playerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
        for(size_t q = 0; q < playerNames.size(); ++q)
        {
            mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);
        }
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

void MultiPlayerMode::customFrameStartedDoProcessFrameBeforePhysics(const Ogre::FrameEvent &evt)
{
    if(mIsSessionStarted)
    {
        //AI
        for(size_t q = 0; q < mModeContext.mGameState.getMultiplayerCountAI(); ++q)
        {
            mModeContext.mGameState.getMultiplayerCarAI(q).processFrameBeforePhysics(evt, mStaticMeshProcesser, mModeContext.mGameState.getRaceStarted());
        }

        //human
        std::vector<std::string> playerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
        for(size_t q = 0; q < playerNames.size(); ++q)
        {
            mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).processFrameBeforePhysics(evt, mStaticMeshProcesser, mModeContext.mGameState.getRaceStarted());
        }
    }
}

void MultiPlayerMode::customFrameStartedDoProcessFrameAfterPhysics(const Ogre::FrameEvent &evt)
{
    if(mIsSessionStarted)
    {
        //AI
        for(size_t q = 0; q < mModeContext.mGameState.getMultiplayerCountAI(); ++q)
            mModeContext.mGameState.getMultiplayerCarAI(q).processFrameAfterPhysics(evt, mModeContext.mGameState.getRaceStarted());

        //human
        std::vector<std::string> playerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
        for(size_t q = 0; q < playerNames.size(); ++q)
        {
            mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).processFrameAfterPhysics(evt, mModeContext.mGameState.getRaceStarted());
        }
    }


    if(mMultiplayerController.get())
    {
        mMultiplayerController->receiveData();

        if(mIsSessionStarted)
        {

            Ogre::Vector3 playerPos = mModeContext.mGameState.getPlayerCar().getModelNode()->getPosition();

            Ogre::Vector3 playerVel = mModeContext.mGameState.getPlayerCar().getLinearVelocity();
            Ogre::Vector3 playerAngVel = mModeContext.mGameState.getPlayerCar().getAngularVelocity();
            Ogre::Quaternion playerRot = mModeContext.mGameState.getPlayerCar().getModelNode()->getOrientation();

            Ogre::Vector3 wheel0;
            Ogre::Vector3 wheel1;
            Ogre::Vector3 wheel2;
            Ogre::Vector3 wheel3;
            mModeContext.mGameState.getPlayerCar().getWheelsPositions(wheel0, wheel1, wheel2, wheel3);
            
            MultiplayerSessionData data;
            data.pos = playerPos;
            data.rot = playerRot;
            data.vel = playerVel;
            data.velang = playerAngVel;
            data.wheelpos[0] = wheel0;
            data.wheelpos[1] = wheel1;
            data.wheelpos[2] = wheel2;
            data.wheelpos[3] = wheel3;
            data.isAcc = mModeContext.mGameState.getPlayerCar().getAcceleration();
            data.isBrake = mModeContext.mGameState.getPlayerCar().getBrake();
            data.isLeft = mModeContext.mGameState.getPlayerCar().getSteerLeft();
            data.isRight = mModeContext.mGameState.getPlayerCar().getSteerRight();
            data.currentLap = mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap();
            data.lapPosition = mModeContext.mGameState.getPlayerCar().getLapUtils().getLapPosition();

            std::vector<MultiplayerSessionData> dataAI;
            for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
            {
                Ogre::Vector3 aiVel = mModeContext.mGameState.getAICar(q).getLinearVelocity();
                Ogre::Vector3 aiAngVel = mModeContext.mGameState.getAICar(q).getAngularVelocity();
                Ogre::Quaternion aiRot = mModeContext.mGameState.getAICar(q).getModelNode()->getOrientation();
                Ogre::Vector3 aiPos = mModeContext.mGameState.getAICar(q).getModelNode()->getPosition();

                Ogre::Vector3 aiWheel0;
                Ogre::Vector3 aiWheel1;
                Ogre::Vector3 aiWheel2;
                Ogre::Vector3 aiWheel3;
                mModeContext.mGameState.getAICar(q).getWheelsPositions(aiWheel0, aiWheel1, aiWheel2, aiWheel3);

                MultiplayerSessionData aiData;
                aiData.pos = aiPos;
                aiData.rot = aiRot;
                aiData.vel = aiVel;
                aiData.velang = aiAngVel;

                aiData.wheelpos[0] = aiWheel0;
                aiData.wheelpos[1] = aiWheel1;
                aiData.wheelpos[2] = aiWheel2;
                aiData.wheelpos[3] = aiWheel3;

                aiData.isAcc = mModeContext.mGameState.getAICar(q).getAcceleration();
                aiData.isBrake = mModeContext.mGameState.getAICar(q).getBrake();
                aiData.isLeft = mModeContext.mGameState.getAICar(q).getSteerLeft();
                aiData.isRight = mModeContext.mGameState.getAICar(q).getSteerRight();

                aiData.currentLap = mModeContext.mGameState.getAICar(q).getLapUtils().getCurrentLap();
                aiData.lapPosition = mModeContext.mGameState.getAICar(q).getLapUtils().getLapPosition();

                dataAI.push_back(aiData);
            }

            mMultiplayerController->updateSessionData(data, dataAI, evt.timeSinceLastFrame);
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
        mUIRace->setMiscText(Conversions::DMToString(mMultiplayerController->getSessionPing()));
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
        mUIRace->setDashCarPos(mModeContext.mGameState.getAICount() + q, currentPlayerLap, currentPlayerLapPos, mModeContext.mGameState.getMultiplayerCarAI(q).getCurrentLap(), mModeContext.mGameState.getMultiplayerCarAI(q).getLapPosition());
    }

    //human
    std::vector<std::string> playerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
    for(size_t q = 0; q < playerNames.size(); ++q)
    {
        mUIRace->setAIDashBoardSkin(mModeContext.mGameState, mModeContext.mGameState.getAICount() + mModeContext.mGameState.getMultiplayerCountAI() + q, mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).getCharacterName());
        mUIRace->setDashCarPos(mModeContext.mGameState.getAICount() + mModeContext.mGameState.getMultiplayerCountAI() + q, currentPlayerLap, currentPlayerLapPos, mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).getCurrentLap(), mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).getLapPosition());
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
            }
            mModeContext.mGameState.setMultiplayerCountAI(0);
        }
    }
}

void MultiPlayerMode::prepareDataForSession(const MultiplayerSessionStartInfo& sessionStartInfo)
{
    //playerIndex should start from 0
    mModeContext.mGameState.getPlayerCar().setModelPositionOnGrid(mModeContext.mGameState.getTrackPositions()[sessionStartInfo.mAIAmount + sessionStartInfo.mPlayerIndex]);

    mModeContext.mGameState.clearMultiplayerCarsHuman();
    mModeContext.mGameState.setMultiplayerCountAI(0);

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

                humanCar.initModel(mModeContext.mPipeline, mModeContext.mGameState, mSceneMgr, mMainNode, mCameraMan.get(), &mModelsPool, mWorld.get(), humanCharacter, mModeContext.mGameState.getTrackPositions()[q], false);
                humanCar.initSounds(mModeContext.mPipeline, mModeContext.mGameState);

                humanCar.setModelPositionOnGrid(mModeContext.mGameState.getTrackPositions()[sessionStartInfo.mAIAmount + q]);

                mLapController.addCar(&humanCar);
            }
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

                humanCar.initModel(mModeContext.mPipeline, mModeContext.mGameState, mSceneMgr, mMainNode, mCameraMan.get(), &mModelsPool, mWorld.get(), humanCharacter, mModeContext.mGameState.getTrackPositions()[q], false);
                humanCar.initSounds(mModeContext.mPipeline, mModeContext.mGameState);

                humanCar.setModelPositionOnGrid(mModeContext.mGameState.getTrackPositions()[sessionStartInfo.mAIAmount + q]);

                mLapController.addCar(&humanCar);
            }
        }

        //AI
        mModeContext.mGameState.setMultiplayerCountAI(sessionStartInfo.mAIAmount);

        for(size_t q = 0; q < sessionStartInfo.mAIAmount; ++q)
        {
            std::string aiCharacter = "frantic";
            if(sessionStartInfo.mAISkins.size() > q)
            {
                aiCharacter = sessionStartInfo.mAISkins[q];
            }

            mModeContext.mGameState.getMultiplayerCarAI(q).setCharacterName(aiCharacter);

            mModeContext.mGameState.getMultiplayerCarAI(q).initModel(mModeContext.mPipeline, mModeContext.mGameState, mSceneMgr, mMainNode, mCameraMan.get(), &mModelsPool, mWorld.get(), aiCharacter, mModeContext.mGameState.getTrackPositions()[q], false);

            mModeContext.mGameState.getMultiplayerCarAI(q).initSounds(mModeContext.mPipeline, mModeContext.mGameState);
            mModeContext.mGameState.getMultiplayerCarAI(q).setModelPositionOnGrid(mModeContext.mGameState.getTrackPositions()[q]);

            mLapController.addCar(&mModeContext.mGameState.getMultiplayerCarAI(q));
        }
    }

    mIsSessionStarted = true;
}

void MultiPlayerMode::onSessionUpdate(const playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost)
{
    const float posDiffMax = 30.0f;

    if(!isHost)
    {
        //AI
        if(mModeContext.mGameState.getMultiplayerCountAI() == aiPlayersSessionData.size())
        {
            for(size_t q = 0; q < aiPlayersSessionData.size(); ++q)
            {
                Ogre::Vector3 aiPos = mModeContext.mGameState.getMultiplayerCarAI(q).getModelNode()->getPosition();
                if(aiPos.distance(aiPlayersSessionData[q].pos) > posDiffMax)
                {
                    mModeContext.mGameState.getMultiplayerCarAI(q).setModelPosition(aiPlayersSessionData[q].pos, aiPlayersSessionData[q].rot, aiPlayersSessionData[q].wheelpos[0], aiPlayersSessionData[q].wheelpos[1], aiPlayersSessionData[q].wheelpos[2], aiPlayersSessionData[q].wheelpos[3]);
                }
                else
                {
                    mModeContext.mGameState.getMultiplayerCarAI(q).setModelRotationOnly(aiPlayersSessionData[q].rot);
                }

                mModeContext.mGameState.getMultiplayerCarAI(q).setModelVelocity(aiPlayersSessionData[q].vel, aiPlayersSessionData[q].velang);
                mModeContext.mGameState.getMultiplayerCarAI(q).setAcceleration(aiPlayersSessionData[q].isAcc);
                mModeContext.mGameState.getMultiplayerCarAI(q).setBrake(aiPlayersSessionData[q].isBrake);
                mModeContext.mGameState.getMultiplayerCarAI(q).setSteerLeft(aiPlayersSessionData[q].isLeft);
                mModeContext.mGameState.getMultiplayerCarAI(q).setSteerRight(aiPlayersSessionData[q].isRight);
                mModeContext.mGameState.getMultiplayerCarAI(q).setCurrentLap(aiPlayersSessionData[q].currentLap);
                mModeContext.mGameState.getMultiplayerCarAI(q).setLapPosition(aiPlayersSessionData[q].lapPosition);
            }
        }


        //human
        for(playerToData::const_iterator i = otherPlayersSessionData.begin(), j = otherPlayersSessionData.end(); 
            i != j; ++i)
        {
            PSMultiplayerCar& humanCar = mModeContext.mGameState.getMultiplayerCarHuman((*i).first);
            Ogre::Vector3 humanPos = humanCar.getModelNode()->getPosition();
            if(humanPos.distance((*i).second.pos) > posDiffMax)
            {
                humanCar.setModelPosition((*i).second.pos, (*i).second.rot, (*i).second.wheelpos[0], (*i).second.wheelpos[1], (*i).second.wheelpos[2], (*i).second.wheelpos[3]);
            }
            else
            {
                humanCar.setModelRotationOnly((*i).second.rot);
            }

            humanCar.setModelVelocity((*i).second.vel, (*i).second.velang);
            humanCar.setAcceleration((*i).second.isAcc);
            humanCar.setBrake((*i).second.isBrake);
            humanCar.setSteerLeft((*i).second.isLeft);
            humanCar.setSteerRight((*i).second.isRight);
            humanCar.setCurrentLap((*i).second.currentLap);
            humanCar.setLapPosition((*i).second.lapPosition);
        }
    }
    else
    {
        //human only
        for(playerToData::const_iterator i = otherPlayersSessionData.begin(), j = otherPlayersSessionData.end(); 
            i != j; ++i)
        {
            PSMultiplayerCar& humanCar = mModeContext.mGameState.getMultiplayerCarHuman((*i).first);
            Ogre::Vector3 humanPos = humanCar.getModelNode()->getPosition();
            if(humanPos.distance((*i).second.pos) > posDiffMax)
            {
                humanCar.setModelPosition((*i).second.pos, (*i).second.rot, (*i).second.wheelpos[0], (*i).second.wheelpos[1], (*i).second.wheelpos[2], (*i).second.wheelpos[3]);
            }
            else
            {
                humanCar.setModelRotationOnly((*i).second.rot);
            }

            humanCar.setModelVelocity((*i).second.vel, (*i).second.velang);
            humanCar.setAcceleration((*i).second.isAcc);
            humanCar.setBrake((*i).second.isBrake);
            humanCar.setSteerLeft((*i).second.isLeft);
            humanCar.setSteerRight((*i).second.isRight);
            humanCar.setCurrentLap((*i).second.currentLap);
            humanCar.setLapPosition((*i).second.lapPosition);
        }
    }
}

void MultiPlayerMode::onError(const std::string& message)
{
    mUIRace->addMiscPanelText("Error: [" + message + "]");
}