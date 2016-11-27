#include "../pcheader.h"

#include "SinglePlayerMode.h"

#include "../InputHandler.h"

#include "../customs/CustomTrayManager.h"

#include "../tools/Conversions.h"

SinglePlayerMode::SinglePlayerMode(const ModeContext& modeContext) :
    BaseRaceMode(modeContext)
{}

void SinglePlayerMode::frameStarted(const Ogre::FrameEvent &evt)
{
    Ogre::Vector3 playerPos = mModeContext.mGameState.getPlayerCar().getModelNode()->getPosition();

    Ogre::Vector3 playerDir = mModeContext.mGameState.getPlayerCar().getForwardAxis();

    mModeContext.mSoundsProcesser.setListenerPos(playerPos);
    if(!mModeContext.mGameState.isGamePaused())
        mModeContext.mSoundsProcesser.setListenerGain(mModeContext.mGameState.getListenerGain());
    else
        mModeContext.mSoundsProcesser.setListenerGain(0.0f);

    if(!mModeContext.mGameState.getRaceFinished())
        mModeContext.mGameState.getPlayerCar().getLapUtils().checkCheckPoints(playerPos);

    mModeContext.mGameState.getArrowNode()->setOrientation(mModeContext.mGameState.getPlayerCar().getLapUtils().getArrowOrientation(playerPos, playerDir));

    if(!mModeContext.mGameState.isGamePaused())
    {
        mModeContext.mGameState.getPlayerCar().processFrameBeforePhysics(evt, mStaticMeshProcesser, mModeContext.mGameState.getRaceStarted());

        for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        {
            if(!mModeContext.mGameState.getRaceFinished())
                mModeContext.mGameState.getAICar(q).getLapUtils().checkCheckPoints(mModeContext.mGameState.getAICar(q).getModelNode()->getPosition());

            if(mModeContext.mGameState.getRaceStarted())
            {
                mModeContext.mGameState.getAICar(q).performAICorrection(mModeContext.mGameState.isGamePaused());
            }
            mModeContext.mGameState.getAICar(q).processFrameBeforePhysics(evt, mStaticMeshProcesser, mModeContext.mGameState.getRaceStarted());
        }
    }

    mLapController.calculateLapPositions();

    if(mModeContext.mGameState.getRaceStarted() && !mModeContext.mGameState.isGamePaused())
        mWorld->stepSimulation(evt.timeSinceLastFrame, 7);

    mModeContext.mGameState.getPlayerCar().processFrameAfterPhysics(evt, mModeContext.mGameState.getRaceStarted());

    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        mModeContext.mGameState.getAICar(q).processFrameAfterPhysics(evt, mModeContext.mGameState.getRaceStarted());
}

void SinglePlayerMode::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if( !mModeContext.mGameState.getRaceStarted() && 
        mModeContext.mGameState.getBeforeStartTimerTime() > mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "ready left start") * 1000.0f && 
        !mModeContext.mGameState.isGamePaused())
    {
        mModeContext.mGraphics2D.setRearViewMirrorPanelShow(false);
        mModeContext.mGraphics2D.hideAllStart();
        mModeContext.mGraphics2D.showBeforeStart1();
        mModeContext.mSoundsProcesser.playBeforeStart1();
    }

    if(!mModeContext.mGameState.getRaceStarted() && 
        mModeContext.mGameState.getBeforeStartTimerTime() > mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "set left start") * 1000.0f && 
        !mModeContext.mGameState.isGamePaused())
    {
        mModeContext.mGraphics2D.hideAllStart();
        mModeContext.mGraphics2D.showBeforeStart2();
        mModeContext.mSoundsProcesser.playBeforeStart2();
    }

    if(!mModeContext.mGameState.getRaceStarted() && 
        mModeContext.mGameState.getBeforeStartTimerTime() > mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left start") * 1000.0f && 
        !mModeContext.mGameState.isGamePaused())
    {
        mModeContext.mGraphics2D.hideAllStart();
        mModeContext.mGraphics2D.showBeforeStart3();
        mModeContext.mSoundsProcesser.playBeforeStart3();

        for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        {
            mModeContext.mGameState.getAICar(q).raceStarted();
        }

        if(mModeContext.mGameState.isMultiplayerEnabled())
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

        mModeContext.mGameState.getPlayerCar().getLapUtils().resetLapTimer();

        mModeContext.mGameState.setRaceStarted(true);
    }

    if(mModeContext.mGameState.getBeforeStartTimerTime() > (mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left start") * 1000.0f + mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left length") * 1000.0f) && !mModeContext.mGameState.isGamePaused())
    {
        mModeContext.mGraphics2D.hideAllStart();

        if(mModeContext.mGameState.getMirrorEnabled() && !mModeContext.mGameState.getRaceFinished())
        {
            mModeContext.mGraphics2D.setRearViewMirrorPanelShow(true);
        }
    }

    mModeContext.mInputHandler->capture(evt, mModeContext.mGameState.getPlayerCar().getModelNode(), mModeContext.mGameState.getGlobalLight(), mModeContext.mGameState.getShadowLight(), true);

    mModeContext.mTrayMgr->frameRenderingQueued(evt);

    mModeContext.mGraphics2D.setShowPausedPanel(mModeContext.mGameState.isGamePaused());

    if (!mModeContext.mTrayMgr->isDialogVisible())
    {
        if(!mModeContext.mGameState.isGamePaused())
            mModeContext.mGraphics2D.setEngineRPM(mModeContext.mGameState.getPlayerCar().getCarEngine().getEngineRPM());
        mModeContext.mGraphics2D.setCarSpeed(mModeContext.mGameState.getPlayerCar().getAlignedVelocity());
        mModeContext.mGraphics2D.setCurrentLap(static_cast<unsigned short>(mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap()), static_cast<unsigned short>(mModeContext.mGameState.getLapsCount()));
        mModeContext.mGraphics2D.setCarGear(static_cast<unsigned char>(mModeContext.mGameState.getPlayerCar().getCarEngine().getCurrentGear()));
        mModeContext.mGraphics2D.setCarPos(static_cast<unsigned char>(mLapController.getTotalPosition(0)), static_cast<unsigned char>(mLapController.getTotalCars()));

        mModeContext.mGraphics2D.hideAIDashboardCars();
        size_t currentPlayerLap = mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap();
        Ogre::Real currentPlayerLapPos = mModeContext.mGameState.getPlayerCar().getLapUtils().getLapPosition();
        mModeContext.mGraphics2D.setPlayerDashBoardSkin(mModeContext.mGameState);
        for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        {
            mModeContext.mGraphics2D.setAIDashBoardSkin(mModeContext.mGameState, q, mModeContext.mGameState.getAICar(q).getCharacterName());
            mModeContext.mGraphics2D.setDashCarPos(q, currentPlayerLap, currentPlayerLapPos, mModeContext.mGameState.getAICar(q).getLapUtils().getCurrentLap(), mModeContext.mGameState.getAICar(q).getLapUtils().getLapPosition());
        }

        if(mModeContext.mGameState.isMultiplayerEnabled())
        {
            //AI
            for(size_t q = 0; q < mModeContext.mGameState.getMultiplayerCountAI(); ++q)
            {
                mModeContext.mGraphics2D.setAIDashBoardSkin(mModeContext.mGameState, mModeContext.mGameState.getAICount() + q, mModeContext.mGameState.getMultiplayerCarAI(q).getCharacterName());
                mModeContext.mGraphics2D.setDashCarPos(mModeContext.mGameState.getAICount() + q, currentPlayerLap, currentPlayerLapPos, mModeContext.mGameState.getMultiplayerCarAI(q).getCurrentLap(), mModeContext.mGameState.getMultiplayerCarAI(q).getLapPosition());
            }

            //human
            std::vector<std::string> playerNames = mModeContext.mGameState.getMultiplayerCarHumanNames();
            for(size_t q = 0; q < playerNames.size(); ++q)
            {
                mModeContext.mGraphics2D.setAIDashBoardSkin(mModeContext.mGameState, mModeContext.mGameState.getAICount() + mModeContext.mGameState.getMultiplayerCountAI() + q, mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).getCharacterName());
                mModeContext.mGraphics2D.setDashCarPos(mModeContext.mGameState.getAICount() + mModeContext.mGameState.getMultiplayerCountAI() + q, currentPlayerLap, currentPlayerLapPos, mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).getCurrentLap(), mModeContext.mGameState.getMultiplayerCarHuman(playerNames[q]).getLapPosition());
            }
        }

        if(mModeContext.mGameState.getRaceStarted())
        {
            if(mModeContext.mGameState.getPlayerCar().getLapUtils().getAfterFinishLinePassTime() < 3.0f && mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap() > 1)
            {
                mModeContext.mGraphics2D.setRaceTime(Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLastLapTime()));
            }
            else
            {
                mModeContext.mGraphics2D.setRaceTime(Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLapTime()));
            }
        }
        else
        {
            mModeContext.mGraphics2D.setRaceTime("00:00:00");
        }


#if SHOW_DETAILS_PANEL
        mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
        mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
        mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
        
        mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getLinearVelocity().length()));
        mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getAlignedVelocity()));
        mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getLateralVelocity()));
#if 0
        mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mModeContext.mGameState.getAICar(0].getLinearVelocity().length()));
        mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mModeContext.mGameState.getAICar(0].getAlignedVelocity()));
        mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mModeContext.mGameState.getAICar(0].getLateralVelocity()));
#endif

        mDetailsPanel->setParamValue(8, mModeContext.mGameState.getPlayerCar().getFrontLWheelColliderString());
        mDetailsPanel->setParamValue(9, mModeContext.mGameState.getPlayerCar().getFrontRWheelColliderString());
        mDetailsPanel->setParamValue(10, mModeContext.mGameState.getPlayerCar().getBackLWheelColliderString());
        mDetailsPanel->setParamValue(11, mModeContext.mGameState.getPlayerCar().getBackRWheelColliderString());
#if 0
        mDetailsPanel->setParamValue(8, mModeContext.mGameState.getAICar(0].getFrontLWheelColliderString());
        mDetailsPanel->setParamValue(9, mModeContext.mGameState.getAICar(0].getFrontRWheelColliderString());
        mDetailsPanel->setParamValue(10, mModeContext.mGameState.getAICar(0].getBackLWheelColliderString());
        mDetailsPanel->setParamValue(11, mModeContext.mGameState.getAICar(0].getBackRWheelColliderString());
#endif

        mDetailsPanel->setParamValue(13, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getPlayerCar().getFrontLWheelColliderIndex())));
        mDetailsPanel->setParamValue(14, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getPlayerCar().getFrontRWheelColliderIndex())));
        mDetailsPanel->setParamValue(15, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getPlayerCar().getBackLWheelColliderIndex())));
        mDetailsPanel->setParamValue(16, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getPlayerCar().getBackRWheelColliderIndex())));
#if 0
        mDetailsPanel->setParamValue(13, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getAICar(0].getFrontLWheelColliderIndex())));
        mDetailsPanel->setParamValue(14, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getAICar(0].getFrontRWheelColliderIndex())));
        mDetailsPanel->setParamValue(15, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getAICar(0].getBackLWheelColliderIndex())));
        mDetailsPanel->setParamValue(16, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getAICar(0].getBackRWheelColliderIndex())));
#endif

        mDetailsPanel->setParamValue(18, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getPlayerCar().getFrontLWheelColliderIndex())));
        mDetailsPanel->setParamValue(19, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getPlayerCar().getFrontRWheelColliderIndex())));
        mDetailsPanel->setParamValue(20, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getPlayerCar().getBackLWheelColliderIndex())));
        mDetailsPanel->setParamValue(21, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getPlayerCar().getBackRWheelColliderIndex())));
#if 0
        mDetailsPanel->setParamValue(18, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getAICar(0].getFrontLWheelColliderIndex())));
        mDetailsPanel->setParamValue(19, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getAICar(0].getFrontRWheelColliderIndex())));
        mDetailsPanel->setParamValue(20, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getAICar(0].getBackLWheelColliderIndex())));
        mDetailsPanel->setParamValue(21, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getAICar(0].getBackRWheelColliderIndex())));
#endif

        mDetailsPanel->setParamValue(23, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLapPosition()));
        mDetailsPanel->setParamValue(24, Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLastLapTime()));
        mDetailsPanel->setParamValue(25, Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLapTime()));
        mDetailsPanel->setParamValue(26, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap()));
        mDetailsPanel->setParamValue(27, Ogre::StringConverter::toString(mLapController.getTotalPosition(0)));
        mDetailsPanel->setParamValue(28, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getCarEngine().getCurrentGear()));
        mDetailsPanel->setParamValue(29, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getCarEngine().getEngineRPM()));
#endif
    }
}