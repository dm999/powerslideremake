
#include "UIMainMenuLabels.h"

#include "../tools/Conversions.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../gamelogic/TrialGhost.h"
#include "../tools/SectionsFile.h"

#include "../BaseApp.h"

namespace{
#if !defined(__ANDROID__)
    const Ogre::Real buttonSize = 12.0f;
    const Ogre::Real buttonLeftAdj = 0.0f;
    const Ogre::Real buttonTopAdj = 0.0f;
#else
    const Ogre::Real buttonSize = 18.0f;
    const Ogre::Real buttonLeftAdj = -3.0f;
    const Ogre::Real buttonTopAdj = -3.0f;
#endif
}

UIMainMenuLabels::UIMainMenuLabels(const ModeContext& modeContext, const GameMode gameMode) : 
    UIMainMenuBackground(modeContext, gameMode),
    mIsViewByDescription(false),
    mIsBioByDescription(false)
{}

void UIMainMenuLabels::onButtonReleased(UIButton * button)
{
    if(button == mShadowVal)
    {
        if(mShadowVal->getChecked())
        {
            mModeContext.getGameState().setCastShadows(true);
        }
        else
        {
            mModeContext.getGameState().setCastShadows(false);
        }

        mModeContext.getGameState().savePlayerData();
    }

    if(button == mVSyncVal)
    {
        if(mVSyncVal->getChecked())
        {
            mModeContext.getRenderWindow()->setVSyncEnabled(true);
            mModeContext.getRenderWindow()->setVSyncInterval(1);
        }
        else
        {
            mModeContext.getRenderWindow()->setVSyncEnabled(false);
        }

        mModeContext.getGameState().savePlayerData();
    }

    if(button == mFulscreenVal)
    {
        if(mFulscreenVal->getChecked())
        {
            Ogre::RenderSystem * rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
            Ogre::ConfigOptionMap& configOpts = rs->getConfigOptions();
            Ogre::ConfigOption& videoMode = configOpts["Video Mode"];
            unsigned int curWidth, curHeight;
            sscanf(videoMode.currentValue.c_str(), "%d x %d", &curWidth, &curHeight);

            mModeContext.getRenderWindow()->setFullscreen(true, curWidth, curHeight);
            mOptionGraphicsLabel_Resolution_Val->setCaption(videoMode.currentValue);
        }
        else
        {
            unsigned int width, height, colourDepth;
            int left, top;
            mModeContext.getRenderWindow()->getMetrics(width, height, colourDepth, left, top);
            mModeContext.getRenderWindow()->setFullscreen(false, width, height);

            Ogre::RenderSystem * rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
            Ogre::ConfigOptionMap configOpts = rs->getConfigOptions();
            Ogre::ConfigOption videoMode = configOpts["Video Mode"];
            mOptionGraphicsLabel_Resolution_Val->setCaption(videoMode.currentValue);
        }

        mModeContext.getGameState().savePlayerData();

        mModeContext.getGameModeSwitcher()->recreateMenu();
    }

    if(button == mInputTypeValLeft || button == mInputTypeValRight)
    {
#if !defined(__ANDROID__)
        InputType input = mModeContext.getGameState().getInputType();

        if(input == itKeyboard)
        {
            mModeContext.getGameState().setInputType(itMouse);
            mOptionInputLabel_Type->setCaption("Mouse");
        }
        if(input == itMouse)
        {
            mModeContext.getGameState().setInputType(itKeyboard);
            mOptionInputLabel_Type->setCaption("Keyboard");
        }

        mModeContext.getGameState().savePlayerData();
#endif
    }

    if(button == mSoundVolumeFXValLeft)
    {
        size_t gain = mModeContext.getGameState().getSoundsGain() * 9.0f;
        if(gain > 0)
            --gain;
        mModeContext.getGameState().setSoundsGain(gain / 9.0f);
        mOptionSoundLabel_VolumeFX_Val->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getSoundsGain() * 9.0f)));

        mModeContext.getGameState().savePlayerData();

#ifndef NO_OPENAL
    mModeContext.getSoundsProcesser().setVolume(mModeContext.getGameState().getSoundsGain());
#endif
    }

    if(button == mSoundVolumeFXValRight)
    {
        size_t gain = mModeContext.getGameState().getSoundsGain() * 9.0f;
        if(gain < 9)
            ++gain;
        mModeContext.getGameState().setSoundsGain(gain / 9.0f);
        mOptionSoundLabel_VolumeFX_Val->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getSoundsGain() * 9.0f)));

        mModeContext.getGameState().savePlayerData();

#ifndef NO_OPENAL
    mModeContext.getSoundsProcesser().setVolume(mModeContext.getGameState().getSoundsGain());
#endif
    }

    if(button == mSoundVolumeMusicValLeft)
    {
        size_t gain = mModeContext.getGameState().getMusicGain() * 9.0f;
        if(gain > 0)
            --gain;
        mModeContext.getGameState().setMusicGain(gain / 9.0f);
        mOptionSoundLabel_VolumeMusic_Val->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getMusicGain() * 9.0f)));

        mModeContext.getGameState().savePlayerData();

#ifndef NO_OPENAL
    mModeContext.getMusicProcessor().setVolume(mModeContext.getGameState().getMusicGain());
#endif
    }

    if(button == mSoundVolumeMusicValRight)
    {
        size_t gain = mModeContext.getGameState().getMusicGain() * 9.0f;
        if(gain < 9)
            ++gain;
        mModeContext.getGameState().setMusicGain(gain / 9.0f);
        mOptionSoundLabel_VolumeMusic_Val->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getMusicGain() * 9.0f)));

        mModeContext.getGameState().savePlayerData();

#ifndef NO_OPENAL
    mModeContext.getMusicProcessor().setVolume(mModeContext.getGameState().getMusicGain());
#endif
    }

    if(button == mOpponentsValLeft)
    {
        size_t aiCount = mModeContext.getGameState().getAICount();
        --aiCount;
        if(aiCount >= GameState::mAIMin)
        {
            mModeContext.getGameState().setAICount(aiCount);
            mModeContext.getGameState().setAICountInRace(aiCount);
            mOptionRaceLabel_Opponents_Val->setCaption(Conversions::DMToString(mModeContext.getGameState().getAICountInRace()));
        }

        mModeContext.getGameState().savePlayerData();
    }

    if(button == mOpponentsValRight)
    {
        size_t aiCount = mModeContext.getGameState().getAICount();
        ++aiCount;
        if(aiCount <= GameState::mAIMax)
        {
            mModeContext.getGameState().setAICount(aiCount);
            mModeContext.getGameState().setAICountInRace(aiCount);
            mOptionRaceLabel_Opponents_Val->setCaption(Conversions::DMToString(mModeContext.getGameState().getAICountInRace()));
        }

        mModeContext.getGameState().savePlayerData();
    }

    if(button == mMirrorVal)
    {
        if(mMirrorVal->getChecked())
        {
            mModeContext.getGameState().setMirrorEnabled(true);
        }
        else
        {
            mModeContext.getGameState().setMirrorEnabled(false);
        }

        mModeContext.getGameState().savePlayerData();
    }

    if(button == mGhostVal)
    {
        if(mGhostVal->getChecked())
        {
            mModeContext.getGameState().setGhostEnabled(true);
        }
        else
        {
            mModeContext.getGameState().setGhostEnabled(false);
        }

        mModeContext.getGameState().savePlayerData();
    }


    if(button == mHighScoresTrackLeft)
    {
        const STRHiscores& strHiscores = mModeContext.getGameState().getSTRHiscores();
        std::vector<std::string> hiscoresTracks = strHiscores.getArrayValue("", "hiscore tracks");
        if(mHighScoreTrackIndex > 0)
            --mHighScoreTrackIndex;

        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

        std::string availTrack = availTracks[mHighScoreTrackIndex];
        if(mHighScoreTrackIndex == 8)
            availTrack = availTracks[mHighScoreTrackIndex + 1];

        mOptionHighScoresTrackLabel->setCaption(strPowerslide.getTrackTitle(availTrack));

        fillHighScoreTable();
        fillReplayStat();
    }

    if(button == mHighScoresTrackRight)
    {
        const size_t amountOfHighScoreTracks = 8;

        const STRHiscores& strHiscores = mModeContext.getGameState().getSTRHiscores();
        std::vector<std::string> hiscoresTracks = strHiscores.getArrayValue("", "hiscore tracks");
        if(mHighScoreTrackIndex < amountOfHighScoreTracks)
            ++mHighScoreTrackIndex;

        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

        std::string availTrack = availTracks[mHighScoreTrackIndex];
        if(mHighScoreTrackIndex == 8)
            availTrack = availTracks[mHighScoreTrackIndex + 1];

        mOptionHighScoresTrackLabel->setCaption(strPowerslide.getTrackTitle(availTrack));

        fillHighScoreTable();
        fillReplayStat();
    }

#ifndef NO_OPENAL
    mModeContext.getSoundsProcesser().playUIDown();
#endif
}

void UIMainMenuLabels::onButtonOver(UIButton * button)
{
#ifndef NO_OPENAL
    mModeContext.getSoundsProcesser().playUIOver();
#endif
}

void UIMainMenuLabels::onLabelReleased(UILabel * label)
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();

    if(label == mModeSingle)
    {
        switchState(State_SingleType);
    }

    if(label == mModeMulti)
    {
        switchState(State_Multi);
    }

    if(label == mModeSingleTypeRace)
    {
        mGameModeSelected = ModeMenu;
        switchState(State_Difficulty);
    }

    if(label == mModeSingleTypeChampionship)
    {
        mGameModeSelected = ModeMenuChampionship;
        switchState(State_Difficulty);
        selectMode();
    }

    if(label == mModeSingleTypeTimetrial)
    {
        mGameModeSelected = ModeMenuTimetrial;
        switchState(State_Track);
        selectMode();
    }

    if(label == mModeSingleDifficultyNovice)
    {
        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), Easy, mModeContext.getGameState().getLapsCount());

        if(mGameModeSelected == ModeMenu || mGameModeSelected == ModeMenuTimetrial)
        {
            switchState(State_Track);
        }

        if(mGameModeSelected == ModeMenuChampionship)
        {
            switchState(State_Car);
        }
    }

    if(label == mModeSingleDifficultyAdvanced)
    {
        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), Medium, mModeContext.getGameState().getLapsCount());
        
        if(mGameModeSelected == ModeMenu || mGameModeSelected == ModeMenuTimetrial)
        {
            switchState(State_Track);
        }

        if(mGameModeSelected == ModeMenuChampionship)
        {
            switchState(State_Car);
        }
    }

    if(label == mModeSingleDifficultyExpert)
    {
        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), Hard, mModeContext.getGameState().getLapsCount());
        
        if(mGameModeSelected == ModeMenu || mGameModeSelected == ModeMenuTimetrial)
        {
            switchState(State_Track);
        }

        if(mGameModeSelected == ModeMenuChampionship)
        {
            switchState(State_Car);
        }
    }

    if(label == mModeSingleDifficultyInsane)
    {
        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), Insane, mModeContext.getGameState().getLapsCount());
        
        if(mGameModeSelected == ModeMenu || mGameModeSelected == ModeMenuTimetrial)
        {
            switchState(State_Track);
        }

        if(mGameModeSelected == ModeMenuChampionship)
        {
            switchState(State_Car);
        }
    }

    for(size_t q = 0; q < mTracksLabels.size(); ++q)
    {
        if(label == mTracksLabels[q])
        {
            std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

            mModeContext.getGameState().setRaceParameters(availTracks[q], mModeContext.getGameState().getAIStrength());

            selectTrack(mModeContext.getGameState().getTrackNameAsOriginal());

            mModeContext.getGameState().savePlayerData();

            switchState(State_Car);
        }
    }

    for(size_t q = 0; q < mCarsLabels.size(); ++q)
    {
        if(label == mCarsLabels[q])
        {
            std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");

            std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(availCars[q]);
            mModeContext.getGameState().getPlayerCar().setCharacterName(availChars[0]);

            std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
            characterCar = strPowerslide.getBaseCarFromCar(characterCar);
            selectCar(characterCar);

            for(size_t q = 0; q < 7; ++q)
            {
                if(q < availChars.size())
                    mCharactersLabels[q]->getTextArea()->setCaption(STRPowerslide::getCharacterTitle(availChars[q]));
                else
                    mCharactersLabels[q]->getTextArea()->setCaption("");
            }

            switchState(State_Character);
        }
    }

    for(size_t q = 0; q < mCharactersLabels.size(); ++q)
    {
        if(label == mCharactersLabels[q])
        {
            std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
            characterCar = strPowerslide.getBaseCarFromCar(characterCar);

            std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

            mModeContext.getGameState().getPlayerCar().setCharacterName(availChars[q]);

            mModeContext.getGameState().savePlayerData();

            if(mGameModeSelected == ModeMenuTimetrial)
            {
                mModeContext.getGameState().setAICountInRace(0);
                startRace();
            }
            else
            {
                mModeContext.getGameState().setAICountInRace(mModeContext.getGameState().getAICount());
                switchState(State_StartingGrid);
            }
        }
    }

    for(size_t q = 0; q < mOptionLabels.size(); ++q)
    {
        if(label == mOptionLabels[q])
        {
            setWindowTitle("Options: " + mOptionLabels[q]->getTextArea()->getCaption());
            switchState(static_cast<SinglePlayerMenuStates>(State_Options + q));
        }
    }


    if(label == mGameExitYesLabel)
    {
        mModeContext.getBaseApp()->setShutdown(false);
    }

    if(label == mGameExitNoLabel)
    {
        if(mGameModeSelected == ModeMenu || mGameModeSelected == ModeMenuTimetrial)
        {
            switchState(State_SingleMulti);
            selectMode();
        }

        if(mGameModeSelected == ModeMenuChampionship)
        {
            switchState(State_Podium);
        }
    }


    if(label == mModeMultiCreateRoom)
    {
        switchState(State_MultiCreateRoom);
    }

    if(label == mModeMultiListOfRooms)
    {
        connectToServer();
    }

    if(label == mModeMultiJoinRoom)
    {
        switchState(State_MultiJoinRoom);
    }


#ifndef NO_OPENAL
    mModeContext.getSoundsProcesser().playUIDown();
#endif
}

void UIMainMenuLabels::onLabelOver(UILabel * label)
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();

    for(size_t q = 0; q < mTracksLabels.size(); ++q)
    {
        if(label == mTracksLabels[q])
        {
            setTrackLogo(q);
            setTrackBestTime(q);
            setTrackDescription(q);
        }
    }

    for(size_t q = 0; q < mCarsLabels.size(); ++q)
    {
        if(label == mCarsLabels[q])
        {
            setCarLogo(q);
        }
    }

    for(size_t q = 0; q < mCharactersLabels.size(); ++q)
    {
        if(label == mCharactersLabels[q])
        {
            std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");

            for (size_t w = 0; w < availableCharacters.size(); ++w)
            {
                if(STRPowerslide::getCharacterTitle(availableCharacters[w]) == mCharactersLabels[q]->getTextArea()->getCaption().asUTF8())
                {
                    setCharacterLogo(w);
                    setBioDescription(w);
                }
            }
        }
    }

#ifndef NO_OPENAL
    mModeContext.getSoundsProcesser().playUIOver();
#endif
}

void UIMainMenuLabels::createLabels(const Ogre::Matrix4& screenAdaptionRelative)
{
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f; 

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 10.0f, 0.0f, 0.0f);
        mWindowTitle = createTextArea("MainWindowTitle", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mWindowTitle->setCaption("Game Mode");
        mWindowTitle->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mWindowTitle->setSpaceWidth(9.0f);
        mWindowTitle->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mWindowTitle->setFontName("SdkTrays/Caption");
        mWindowTitle->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mWindowTitle);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(430.0f, 10.0f, 0.0f, 0.0f);
        mWindowTitleTrophies = createTextArea("MainWindowTitleTrophies", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mWindowTitleTrophies->setCaption("Trophies");
        mWindowTitleTrophies->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mWindowTitleTrophies->setSpaceWidth(9.0f);
        mWindowTitleTrophies->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mWindowTitleTrophies->setFontName("SdkTrays/Caption");
        mWindowTitleTrophies->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mWindowTitleTrophies);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 100.0f, 0.0f, 0.0f);
        mModeSingle = mUILabelsManager.add();
        mModeSingle->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingle->getTextArea()->setCaption("Single Player");
        mModeSingle->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mModeSingle->getTextArea()->setSpaceWidth(9.0f);
        mModeSingle->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mModeSingle->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingle->getTextArea()->setFontName("SdkTrays/Caption");
        mModeSingle->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingle->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingle->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 135.0f, 0.0f, 0.0f);
        mModeMulti = mUILabelsManager.add();
        mModeMulti->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMulti->getTextArea()->setCaption("Multi Player");
        mModeMulti->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mModeMulti->getTextArea()->setSpaceWidth(9.0f);
        mModeMulti->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mModeMulti->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMulti->getTextArea()->setFontName("SdkTrays/Caption");
        mModeMulti->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeMulti->setLabelOnAction(this);
        getMainBackground()->addChild(mModeMulti->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(24.0f, 200.0f, 0.0f, 0.0f);
        mModeMultiIP = createTextArea("MainWindowMultiIP", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiIP->setCaption("Server IP:");
        mModeMultiIP->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiIP->setSpaceWidth(9.0f);
        mModeMultiIP->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiIP->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiIP->setFontName("SdkTrays/Caption");
        mModeMultiIP->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mModeMultiIP);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 350.0f, 0.0f, 0.0f);
        mModeMultiListOfRooms = mUILabelsManager.add();
        mModeMultiListOfRooms->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiListOfRooms->getTextArea()->setCaption("Get List of Rooms");
        mModeMultiListOfRooms->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiListOfRooms->getTextArea()->setSpaceWidth(9.0f);
        mModeMultiListOfRooms->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiListOfRooms->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiListOfRooms->getTextArea()->setFontName("SdkTrays/Caption");
        mModeMultiListOfRooms->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeMultiListOfRooms->setLabelOnAction(this);
        getMainBackground()->addChild(mModeMultiListOfRooms->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(24.0f, 260.0f, 0.0f, 0.0f);
        mModeMultiUserName = createTextArea("MainWindowMultiUserName", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiUserName->setCaption("User Name:");
        mModeMultiUserName->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiUserName->setSpaceWidth(9.0f);
        mModeMultiUserName->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiUserName->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiUserName->setFontName("SdkTrays/Caption");
        mModeMultiUserName->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mModeMultiUserName);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(24.0f, 310.0f, 0.0f, 0.0f);
        mModeMultiRoomName = createTextArea("MainWindowMultiRoomName", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiRoomName->setCaption("New Room Name:");
        mModeMultiRoomName->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiRoomName->setSpaceWidth(9.0f);
        mModeMultiRoomName->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiRoomName->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiRoomName->setFontName("SdkTrays/Caption");
        mModeMultiRoomName->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mModeMultiRoomName);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(24.0f, 350.0f, 0.0f, 0.0f);
        mModeMultiCreateRoom = mUILabelsManager.add();
        mModeMultiCreateRoom->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiCreateRoom->getTextArea()->setCaption("Create Room");
        mModeMultiCreateRoom->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiCreateRoom->getTextArea()->setSpaceWidth(9.0f);
        mModeMultiCreateRoom->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiCreateRoom->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiCreateRoom->getTextArea()->setFontName("SdkTrays/Caption");
        mModeMultiCreateRoom->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeMultiCreateRoom->setLabelOnAction(this);
        getMainBackground()->addChild(mModeMultiCreateRoom->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(460.0f, 350.0f, 0.0f, 0.0f);
        mModeMultiJoinRoom = mUILabelsManager.add();
        mModeMultiJoinRoom->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiJoinRoom->getTextArea()->setCaption("Join Room");
        mModeMultiJoinRoom->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiJoinRoom->getTextArea()->setSpaceWidth(9.0f);
        mModeMultiJoinRoom->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiJoinRoom->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiJoinRoom->getTextArea()->setFontName("SdkTrays/Caption");
        mModeMultiJoinRoom->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeMultiJoinRoom->setLabelOnAction(this);
        getMainBackground()->addChild(mModeMultiJoinRoom->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 175.0f, 0.0f, 0.0f);
        mModeSingleTypeRace = mUILabelsManager.add();
        mModeSingleTypeRace->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleTypeRace->getTextArea()->setCaption("Single Race");
        mModeSingleTypeRace->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeRace->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleTypeRace->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeRace->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleTypeRace->getTextArea()->setFontName("SdkTrays/Caption");
        mModeSingleTypeRace->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleTypeRace->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleTypeRace->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 200.0f, 0.0f, 0.0f);
        mModeSingleTypeChampionship = mUILabelsManager.add();
        mModeSingleTypeChampionship->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleTypeChampionship->getTextArea()->setCaption("Championship");
        mModeSingleTypeChampionship->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeChampionship->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleTypeChampionship->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeChampionship->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleTypeChampionship->getTextArea()->setFontName("SdkTrays/Caption");
        mModeSingleTypeChampionship->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleTypeChampionship->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleTypeChampionship->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 225.0f, 0.0f, 0.0f);
        mModeSingleTypeTimetrial = mUILabelsManager.add();
        mModeSingleTypeTimetrial->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleTypeTimetrial->getTextArea()->setCaption("Time Trial");
        mModeSingleTypeTimetrial->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeTimetrial->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleTypeTimetrial->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeTimetrial->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleTypeTimetrial->getTextArea()->setFontName("SdkTrays/Caption");
        mModeSingleTypeTimetrial->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleTypeTimetrial->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleTypeTimetrial->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 193.0f, 0.0f, 0.0f);
        mModeSingleDifficultyNovice = mUILabelsManager.add();
        mModeSingleDifficultyNovice->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleDifficultyNovice->getTextArea()->setCaption("Novice");
        mModeSingleDifficultyNovice->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyNovice->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleDifficultyNovice->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyNovice->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyNovice->getTextArea()->setFontName("SdkTrays/Caption");
        mModeSingleDifficultyNovice->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleDifficultyNovice->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleDifficultyNovice->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 213.0f, 0.0f, 0.0f);
        mModeSingleDifficultyAdvanced = mUILabelsManager.add();
        mModeSingleDifficultyAdvanced->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleDifficultyAdvanced->getTextArea()->setCaption("Advanced");
        mModeSingleDifficultyAdvanced->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyAdvanced->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleDifficultyAdvanced->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyAdvanced->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyAdvanced->getTextArea()->setFontName("SdkTrays/Caption");
        mModeSingleDifficultyAdvanced->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleDifficultyAdvanced->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleDifficultyAdvanced->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 233.0f, 0.0f, 0.0f);
        mModeSingleDifficultyExpert = mUILabelsManager.add();
        mModeSingleDifficultyExpert->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleDifficultyExpert->getTextArea()->setCaption("Expert");
        mModeSingleDifficultyExpert->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyExpert->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleDifficultyExpert->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyExpert->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyExpert->getTextArea()->setFontName("SdkTrays/Caption");
        mModeSingleDifficultyExpert->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleDifficultyExpert->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleDifficultyExpert->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 253.0f, 0.0f, 0.0f);
        mModeSingleDifficultyInsane = mUILabelsManager.add();
        mModeSingleDifficultyInsane->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleDifficultyInsane->getTextArea()->setCaption("Insane");
        mModeSingleDifficultyInsane->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyInsane->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleDifficultyInsane->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyInsane->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyInsane->getTextArea()->setFontName("SdkTrays/Caption");
        mModeSingleDifficultyInsane->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleDifficultyInsane->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleDifficultyInsane->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(25.0f, 20.0f, 0.0f, 0.0f);
        mSingleTrackDescription = createTextArea("MainWindowSingleTrackDescription", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mSingleTrackDescription->setCaption("");
        mSingleTrackDescription->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackDescription->setSpaceWidth(9.0f);
        mSingleTrackDescription->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackDescription->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mSingleTrackDescription->setFontName("SdkTrays/Caption");
        mSingleTrackDescription->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mSingleTrackDescription);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 340.0f, 0.0f, 0.0f);
        mSingleTrackBestTime = createTextArea("MainWindowSingleTrackBestTime", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mSingleTrackBestTime->setCaption("");
        mSingleTrackBestTime->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackBestTime->setSpaceWidth(9.0f);
        mSingleTrackBestTime->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackBestTime->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mSingleTrackBestTime->setFontName("SdkTrays/Caption");
        mSingleTrackBestTime->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mSingleTrackBestTime);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 360.0f, 0.0f, 0.0f);
        mSingleTrackViewBy = createTextArea("MainWindowSingleTrackViewBy", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mSingleTrackViewBy->setCaption("View By");
        mSingleTrackViewBy->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackViewBy->setSpaceWidth(9.0f);
        mSingleTrackViewBy->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackViewBy->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mSingleTrackViewBy->setFontName("SdkTrays/Caption");
        mSingleTrackViewBy->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mSingleTrackViewBy);
    }

    {
        Ogre::Real viewBySize = getTextWidth(mSingleTrackViewBy->getCaption(), mSingleTrackViewBy);
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(0.0f, 360.0f, 0.0f, 0.0f);
        mSingleTrackViewBySelection = createTextArea("MainWindowSingleTrackViewBySelection", 0.0f, 0.0f, mSingleTrackViewBy->getLeft() + viewBySize + 9.0f, textBoxPos.y); 
        mSingleTrackViewBySelection->setCaption("Image");
        mSingleTrackViewBySelection->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackViewBySelection->setSpaceWidth(9.0f);
        mSingleTrackViewBySelection->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackViewBySelection->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mSingleTrackViewBySelection->setFontName("SdkTrays/Caption");
        mSingleTrackViewBySelection->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mSingleTrackViewBySelection);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(25.0f, 20.0f, 0.0f, 0.0f);
        mSingleBioDescription = createTextArea("MainWindowSingleBioDescription", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mSingleBioDescription->setCaption("");
        mSingleBioDescription->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioDescription->setSpaceWidth(9.0f);
        mSingleBioDescription->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioDescription->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mSingleBioDescription->setFontName("SdkTrays/Caption");
        mSingleBioDescription->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mSingleBioDescription);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 360.0f, 0.0f, 0.0f);
        mSingleBioViewBy = createTextArea("MainWindowSingleBioViewBy", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mSingleBioViewBy->setCaption("View By");
        mSingleBioViewBy->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioViewBy->setSpaceWidth(9.0f);
        mSingleBioViewBy->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioViewBy->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mSingleBioViewBy->setFontName("SdkTrays/Caption");
        mSingleBioViewBy->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mSingleBioViewBy);
    }

    {
        Ogre::Real viewBySize = getTextWidth(mSingleBioViewBy->getCaption(), mSingleBioViewBy);
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(0.0f, 360.0f, 0.0f, 0.0f);
        mSingleBioViewBySelection = createTextArea("MainWindowSingleBioViewBySelection", 0.0f, 0.0f, mSingleBioViewBy->getLeft() + viewBySize + 9.0f, textBoxPos.y); 
        mSingleBioViewBySelection->setCaption("Bio");
        mSingleBioViewBySelection->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioViewBySelection->setSpaceWidth(9.0f);
        mSingleBioViewBySelection->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioViewBySelection->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mSingleBioViewBySelection->setFontName("SdkTrays/Caption");
        mSingleBioViewBySelection->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mSingleBioViewBySelection);
    }

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
    std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");

    mTracksLabels.clear();

    for(size_t q = 0; q < availTracks.size(); ++q)
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(333.0f, 56.0f + q * 30.0f, 0.0f, 0.0f);
        mTracksLabels.push_back(mUILabelsManager.add());
        mTracksLabels[q]->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mTracksLabels[q]->getTextArea()->setCaption(strPowerslide.getTrackTitle(availTracks[q]));
        mTracksLabels[q]->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mTracksLabels[q]->getTextArea()->setSpaceWidth(9.0f);
        mTracksLabels[q]->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mTracksLabels[q]->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mTracksLabels[q]->getTextArea()->setFontName("SdkTrays/Caption");
        mTracksLabels[q]->getTextArea()->setColour(UILabel::mInactiveLabel);
        mTracksLabels[q]->setLabelOnAction(this);
        getMainBackground()->addChild(mTracksLabels[q]->getTextArea());
    }

    mCarsLabels.clear();

    for(size_t q = 0; q < availCars.size(); ++q)
    {
        Ogre::Vector4 pos(333.0f, 64.0f + q * 25.0f, 0.0f, 0.0f);
            if(q > 2)
                pos.y += 15;
            if(q > 5)
                pos.y += 15;
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * pos;
        mCarsLabels.push_back(mUILabelsManager.add());
        mCarsLabels[q]->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mCarsLabels[q]->getTextArea()->setCaption(STRPowerslide::getCarTitle(availCars[q]));
        mCarsLabels[q]->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mCarsLabels[q]->getTextArea()->setSpaceWidth(9.0f);
        mCarsLabels[q]->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mCarsLabels[q]->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mCarsLabels[q]->getTextArea()->setFontName("SdkTrays/Caption");
        mCarsLabels[q]->getTextArea()->setColour(UILabel::mInactiveLabel);
        mCarsLabels[q]->setLabelOnAction(this);
        getMainBackground()->addChild(mCarsLabels[q]->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 223.0f, 0.0f, 0.0f);
        mCarParam_Mass = createTextArea("MainWindowSingleCarParamMass", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mCarParam_Mass->setCaption("Mass:");
        mCarParam_Mass->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_Mass->setSpaceWidth(9.0f);
        mCarParam_Mass->setHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_Mass->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mCarParam_Mass->setFontName("SdkTrays/Caption");
        mCarParam_Mass->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mCarParam_Mass);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 243.0f, 0.0f, 0.0f);
        mCarParam_Power = createTextArea("MainWindowSingleCarParamPower", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mCarParam_Power->setCaption("Power:");
        mCarParam_Power->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_Power->setSpaceWidth(9.0f);
        mCarParam_Power->setHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_Power->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mCarParam_Power->setFontName("SdkTrays/Caption");
        mCarParam_Power->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mCarParam_Power);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 263.0f, 0.0f, 0.0f);
        mCarParam_Acceleration = createTextArea("MainWindowSingleCarParamAccel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mCarParam_Acceleration->setCaption("Acceleration:");
        mCarParam_Acceleration->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_Acceleration->setSpaceWidth(9.0f);
        mCarParam_Acceleration->setHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_Acceleration->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mCarParam_Acceleration->setFontName("SdkTrays/Caption");
        mCarParam_Acceleration->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mCarParam_Acceleration);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 281.0f, 0.0f, 0.0f);
        mCarParam_Traction = createTextArea("MainWindowSingleCarParamTraction", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mCarParam_Traction->setCaption("Traction:");
        mCarParam_Traction->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_Traction->setSpaceWidth(9.0f);
        mCarParam_Traction->setHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_Traction->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mCarParam_Traction->setFontName("SdkTrays/Caption");
        mCarParam_Traction->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mCarParam_Traction);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 300.0f, 0.0f, 0.0f);
        mCarParam_TopSpeed = createTextArea("MainWindowSingleCarParamTopSpeed", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mCarParam_TopSpeed->setCaption("Top Speed:");
        mCarParam_TopSpeed->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_TopSpeed->setSpaceWidth(9.0f);
        mCarParam_TopSpeed->setHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_TopSpeed->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mCarParam_TopSpeed->setFontName("SdkTrays/Caption");
        mCarParam_TopSpeed->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mCarParam_TopSpeed);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 319.0f, 0.0f, 0.0f);
        mCarParam_AeroDynamics = createTextArea("MainWindowSingleCarParamAero", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mCarParam_AeroDynamics->setCaption("AeroDynamics:");
        mCarParam_AeroDynamics->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_AeroDynamics->setSpaceWidth(9.0f);
        mCarParam_AeroDynamics->setHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_AeroDynamics->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mCarParam_AeroDynamics->setFontName("SdkTrays/Caption");
        mCarParam_AeroDynamics->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mCarParam_AeroDynamics);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 338.0f, 0.0f, 0.0f);
        mCarParam_Stability = createTextArea("MainWindowSingleCarParamStability", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mCarParam_Stability->setCaption("Stability:");
        mCarParam_Stability->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_Stability->setSpaceWidth(9.0f);
        mCarParam_Stability->setHeight(20.0f * viewportHeight / 1024.0f);
        mCarParam_Stability->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mCarParam_Stability->setFontName("SdkTrays/Caption");
        mCarParam_Stability->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mCarParam_Stability);
    }

    mCharactersLabels.clear();

    for(size_t q = 0; q < 7; ++q)//maximum available is 7
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(333.0f, 64.0f + q * 25.0f, 0.0f, 0.0f);;
        mCharactersLabels.push_back(mUILabelsManager.add());
        mCharactersLabels[q]->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mCharactersLabels[q]->getTextArea()->setCaption("");
        mCharactersLabels[q]->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mCharactersLabels[q]->getTextArea()->setSpaceWidth(9.0f);
        mCharactersLabels[q]->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mCharactersLabels[q]->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mCharactersLabels[q]->getTextArea()->setFontName("SdkTrays/Caption");
        mCharactersLabels[q]->getTextArea()->setColour(UILabel::mInactiveLabel);
        mCharactersLabels[q]->setLabelOnAction(this);
        getMainBackground()->addChild(mCharactersLabels[q]->getTextArea());
    }

    mOptionLabels.clear();

    for(size_t q = 0; q < 7; ++q)
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(333.0f, 64.0f + q * 25.0f, 0.0f, 0.0f);;
        mOptionLabels.push_back(mUILabelsManager.add("mOptionLabels"));
        mOptionLabels[q]->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionLabels[q]->getTextArea()->setCaption("");
        mOptionLabels[q]->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mOptionLabels[q]->getTextArea()->setSpaceWidth(9.0f);
        mOptionLabels[q]->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mOptionLabels[q]->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionLabels[q]->getTextArea()->setFontName("SdkTrays/Caption");
        mOptionLabels[q]->getTextArea()->setColour(UILabel::mInactiveLabel);
        mOptionLabels[q]->setLabelOnAction(this);
        getMainBackground()->addChild(mOptionLabels[q]->getTextArea());
    }
    mOptionLabels[0]->getTextArea()->setCaption("Graphics");
    mOptionLabels[1]->getTextArea()->setCaption("Input Devices");
    mOptionLabels[2]->getTextArea()->setCaption("Sound");
    mOptionLabels[3]->getTextArea()->setCaption("Race Options");
    mOptionLabels[4]->getTextArea()->setCaption("High Scores");
    mOptionLabels[5]->getTextArea()->setCaption("Change Name");
    mOptionLabels[6]->getTextArea()->setCaption("Trophies");

    const size_t maxValStr = 20;

    //Options Graphics Vendor
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 62.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Vendor = createTextArea("MainWindowOptionGraphicsVendorLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionGraphicsLabel_Vendor->setCaption("Vendor");
        mOptionGraphicsLabel_Vendor->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Vendor->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Vendor->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Vendor->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionGraphicsLabel_Vendor->setFontName("SdkTrays/Caption");
        mOptionGraphicsLabel_Vendor->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionGraphicsLabel_Vendor);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 62.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Vendor_Val = createTextArea("MainWindowOptionGraphicsVendorValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        Ogre::String vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        if(vendor.length() > maxValStr)
        {
            vendor = vendor.substr(0, maxValStr);
            vendor += "...";
        }
        mOptionGraphicsLabel_Vendor_Val->setCaption(vendor);
        mOptionGraphicsLabel_Vendor_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Vendor_Val->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Vendor_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Vendor_Val->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionGraphicsLabel_Vendor_Val->setFontName("SdkTrays/Caption");
        mOptionGraphicsLabel_Vendor_Val->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionGraphicsLabel_Vendor_Val);
    }

    //Options Graphics Renderer
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 82.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Renderer = createTextArea("MainWindowOptionGraphicsRendrereLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionGraphicsLabel_Renderer->setCaption("Renderer");
        mOptionGraphicsLabel_Renderer->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Renderer->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Renderer->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Renderer->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionGraphicsLabel_Renderer->setFontName("SdkTrays/Caption");
        mOptionGraphicsLabel_Renderer->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionGraphicsLabel_Renderer);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 82.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Renderer_Val = createTextArea("MainWindowOptionGraphicsRendererValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        Ogre::String renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        if(renderer.length() > maxValStr)
        {
            renderer = renderer.substr(0, maxValStr);
            renderer += "...";
        }
        mOptionGraphicsLabel_Renderer_Val->setCaption(renderer);
        mOptionGraphicsLabel_Renderer_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Renderer_Val->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Renderer_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Renderer_Val->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionGraphicsLabel_Renderer_Val->setFontName("SdkTrays/Caption");
        mOptionGraphicsLabel_Renderer_Val->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionGraphicsLabel_Renderer_Val);
    }

    //Options Graphics Shadow
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 102.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Shadow = createTextArea("MainWindowOptionGraphicsShadowLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionGraphicsLabel_Shadow->setCaption("Shadows");
        mOptionGraphicsLabel_Shadow->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Shadow->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Shadow->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Shadow->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionGraphicsLabel_Shadow->setFontName("SdkTrays/Caption");
        mOptionGraphicsLabel_Shadow->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionGraphicsLabel_Shadow);
    }
    {
        mShadowVal = mUIButtonTicksManager.add("mGraphics");
        mShadowVal->loadBackground("OriginalButtonTick");
        mShadowVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 102.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getGameState().isCastShadows(), true);
        mShadowVal->setButtonOnAction(this);
    }

    //Options Graphics VSync
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 122.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_VSync = createTextArea("MainWindowOptionGraphicsVsyncLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionGraphicsLabel_VSync->setCaption("VSync");
        mOptionGraphicsLabel_VSync->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_VSync->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_VSync->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_VSync->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionGraphicsLabel_VSync->setFontName("SdkTrays/Caption");
        mOptionGraphicsLabel_VSync->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionGraphicsLabel_VSync);
    }
    {
        bool isActive = true;
#if defined(__ANDROID__)
        isActive = false;
#endif
        mVSyncVal = mUIButtonTicksManager.add("mGraphics");
        mVSyncVal->loadBackground("OriginalButtonTick");
        mVSyncVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 122.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getRenderWindow()->isVSyncEnabled(), isActive);
        mVSyncVal->setButtonOnAction(this);
    }

    //Options Graphics Fullscreen
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 142.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Fulscreen = createTextArea("MainWindowOptionGraphicsFullscreenLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionGraphicsLabel_Fulscreen->setCaption("Fullscreen");
        mOptionGraphicsLabel_Fulscreen->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Fulscreen->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Fulscreen->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Fulscreen->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionGraphicsLabel_Fulscreen->setFontName("SdkTrays/Caption");
        mOptionGraphicsLabel_Fulscreen->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionGraphicsLabel_Fulscreen);
    }
    {
        bool isActive = true;
#if defined(__ANDROID__)
        isActive = false;
#endif
        mFulscreenVal = mUIButtonTicksManager.add("mGraphics");
        mFulscreenVal->loadBackground("OriginalButtonTick");
        mFulscreenVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 142.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getRenderWindow()->isFullScreen(), isActive);
        mFulscreenVal->setButtonOnAction(this);
    }

    //Options Graphics Resolution
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 162.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Resolution = createTextArea("MainWindowOptionGraphicsResolutionLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionGraphicsLabel_Resolution->setCaption("Resolution");
        mOptionGraphicsLabel_Resolution->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Resolution->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Resolution->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Resolution->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionGraphicsLabel_Resolution->setFontName("SdkTrays/Caption");
        mOptionGraphicsLabel_Resolution->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionGraphicsLabel_Resolution);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 162.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Resolution_Val = createTextArea("MainWindowOptionGraphicsResolutionValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        Ogre::RenderSystem * rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
        Ogre::ConfigOptionMap configOpts = rs->getConfigOptions();
        Ogre::ConfigOption videoMode = configOpts["Video Mode"];
        mOptionGraphicsLabel_Resolution_Val->setCaption(videoMode.currentValue);
        mOptionGraphicsLabel_Resolution_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Resolution_Val->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Resolution_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Resolution_Val->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionGraphicsLabel_Resolution_Val->setFontName("SdkTrays/Caption");
        mOptionGraphicsLabel_Resolution_Val->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mOptionGraphicsLabel_Resolution_Val);
    }
    {
        Ogre::Real viewBySize = getTextWidth(mOptionGraphicsLabel_Resolution_Val->getCaption(), mOptionGraphicsLabel_Resolution_Val);
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(0.0f, 162.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Resolution_Apply = createTextArea("MainWindowOptionGraphicsResolutionApplyLabel", 0.0f, 0.0f, mOptionGraphicsLabel_Resolution_Val->getLeft() + viewBySize + 9.0f, textBoxPos.y); 
        mOptionGraphicsLabel_Resolution_Apply->setCaption("apply");
        mOptionGraphicsLabel_Resolution_Apply->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Resolution_Apply->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Resolution_Apply->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Resolution_Apply->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionGraphicsLabel_Resolution_Apply->setFontName("SdkTrays/Caption");
        mOptionGraphicsLabel_Resolution_Apply->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mOptionGraphicsLabel_Resolution_Apply);
    }


    //Options Input Type
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 62.0f, 0.0f, 0.0f);;
        mOptionInputLabel_Type = createTextArea("MainWindowInputTypeLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
#if !defined(__ANDROID__)
        if(mModeContext.getGameState().getInputType() == itKeyboard)
            mOptionInputLabel_Type->setCaption("Keyboard");
        if(mModeContext.getGameState().getInputType() == itMouse)
            mOptionInputLabel_Type->setCaption("Mouse");
#else
        mOptionInputLabel_Type->setCaption("Touchscreen");
#endif
        mOptionInputLabel_Type->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionInputLabel_Type->setSpaceWidth(9.0f);
        mOptionInputLabel_Type->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionInputLabel_Type->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionInputLabel_Type->setFontName("SdkTrays/Caption");
        mOptionInputLabel_Type->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionInputLabel_Type);
    }
    {
        mInputTypeValLeft = mUIButtonsManager.add("mInputType");
        mInputTypeValLeft->loadBackground("OriginalButtonDown");
        mInputTypeValLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mInputTypeValLeft->setButtonOnAction(this);

        mInputTypeValRight = mUIButtonsManager.add("mInputType");
        mInputTypeValRight->loadBackground("OriginalButtonUp");
        mInputTypeValRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(280.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mInputTypeValRight->setButtonOnAction(this);
    }

    //Options Sound
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 62.0f, 0.0f, 0.0f);;
        mOptionSoundLabel_VolumeFX = createTextArea("MainWindowSoundVolumeFXLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionSoundLabel_VolumeFX->setCaption("SoundFX");
        mOptionSoundLabel_VolumeFX->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeFX->setSpaceWidth(9.0f);
        mOptionSoundLabel_VolumeFX->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeFX->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionSoundLabel_VolumeFX->setFontName("SdkTrays/Caption");
        mOptionSoundLabel_VolumeFX->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionSoundLabel_VolumeFX);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 62.0f, 0.0f, 0.0f);;
        mOptionSoundLabel_VolumeFX_Val = createTextArea("MainWindowSoundVolumeFXValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionSoundLabel_VolumeFX_Val->setCaption("");
        mOptionSoundLabel_VolumeFX_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeFX_Val->setSpaceWidth(9.0f);
        mOptionSoundLabel_VolumeFX_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeFX_Val->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionSoundLabel_VolumeFX_Val->setFontName("SdkTrays/Caption");
        mOptionSoundLabel_VolumeFX_Val->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionSoundLabel_VolumeFX_Val);
    }
    {
        mSoundVolumeFXValLeft = mUIButtonsManager.add("mSoundVolume");
        mSoundVolumeFXValLeft->setBackgroundMaterial(mInputTypeValLeft->getMaterialName());
        mSoundVolumeFXValLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mSoundVolumeFXValLeft->setButtonOnAction(this);

        mSoundVolumeFXValRight = mUIButtonsManager.add("mSoundVolume");
        mSoundVolumeFXValRight->setBackgroundMaterial(mInputTypeValRight->getMaterialName());
        mSoundVolumeFXValRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(280.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mSoundVolumeFXValRight->setButtonOnAction(this);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 82.0f, 0.0f, 0.0f);;
        mOptionSoundLabel_VolumeMusic = createTextArea("MainWindowSoundVolumeMusicLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionSoundLabel_VolumeMusic->setCaption("Music");
        mOptionSoundLabel_VolumeMusic->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeMusic->setSpaceWidth(9.0f);
        mOptionSoundLabel_VolumeMusic->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeMusic->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionSoundLabel_VolumeMusic->setFontName("SdkTrays/Caption");
        mOptionSoundLabel_VolumeMusic->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionSoundLabel_VolumeMusic);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 82.0f, 0.0f, 0.0f);;
        mOptionSoundLabel_VolumeMusic_Val = createTextArea("MainWindowSoundVolumeMusicValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionSoundLabel_VolumeMusic_Val->setCaption("");
        mOptionSoundLabel_VolumeMusic_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeMusic_Val->setSpaceWidth(9.0f);
        mOptionSoundLabel_VolumeMusic_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeMusic_Val->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionSoundLabel_VolumeMusic_Val->setFontName("SdkTrays/Caption");
        mOptionSoundLabel_VolumeMusic_Val->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionSoundLabel_VolumeMusic_Val);
    }
    {
        mSoundVolumeMusicValLeft = mUIButtonsManager.add("mSoundVolume");
        mSoundVolumeMusicValLeft->setBackgroundMaterial(mInputTypeValLeft->getMaterialName());
        mSoundVolumeMusicValLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f + buttonLeftAdj, 82.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mSoundVolumeMusicValLeft->setButtonOnAction(this);

        mSoundVolumeMusicValRight = mUIButtonsManager.add("mSoundVolume");
        mSoundVolumeMusicValRight->setBackgroundMaterial(mInputTypeValRight->getMaterialName());
        mSoundVolumeMusicValRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(280.0f + buttonLeftAdj, 82.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mSoundVolumeMusicValRight->setButtonOnAction(this);
    }


    //Options Race Opponents
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 62.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Opponents = createTextArea("MainWindowOptionRaceOpponentsLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionRaceLabel_Opponents->setCaption("Number of Opponents");
        mOptionRaceLabel_Opponents->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Opponents->setSpaceWidth(9.0f);
        mOptionRaceLabel_Opponents->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Opponents->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionRaceLabel_Opponents->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Opponents->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionRaceLabel_Opponents);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 62.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Opponents_Val = createTextArea("MainWindowOptionRaceOpponentsValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionRaceLabel_Opponents_Val->setCaption(Conversions::DMToString(mModeContext.getGameState().getAICount()));
        mOptionRaceLabel_Opponents_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Opponents_Val->setSpaceWidth(9.0f);
        mOptionRaceLabel_Opponents_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Opponents_Val->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionRaceLabel_Opponents_Val->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Opponents_Val->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mOptionRaceLabel_Opponents_Val);
    }
    {
        mOpponentsValLeft = mUIButtonsManager.add("mOpponents");
        //mOpponentsValLeft->loadBackground("OriginalButtonDown");
        mOpponentsValLeft->setBackgroundMaterial(mInputTypeValLeft->getMaterialName());
        mOpponentsValLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mOpponentsValLeft->setButtonOnAction(this);

        mOpponentsValRight = mUIButtonsManager.add("mOpponents");
        //mOpponentsValRight->loadBackground("OriginalButtonUp");
        mOpponentsValRight->setBackgroundMaterial(mInputTypeValRight->getMaterialName());
        mOpponentsValRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(280.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mOpponentsValRight->setButtonOnAction(this);
    }

    //Options Race Transmission
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 82.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Transmission = createTextArea("MainWindowOptionRaceTransmissionLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionRaceLabel_Transmission->setCaption("Transmission");
        mOptionRaceLabel_Transmission->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Transmission->setSpaceWidth(9.0f);
        mOptionRaceLabel_Transmission->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Transmission->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionRaceLabel_Transmission->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Transmission->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionRaceLabel_Transmission);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 82.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Transmission_Val = createTextArea("MainWindowOptionRaceTransmissionValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        if(mModeContext.getGameState().getTransmissionType() == trAuto)
            mOptionRaceLabel_Transmission_Val->setCaption("Auto");
        else
            mOptionRaceLabel_Transmission_Val->setCaption("Manual");
        mOptionRaceLabel_Transmission_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Transmission_Val->setSpaceWidth(9.0f);
        mOptionRaceLabel_Transmission_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Transmission_Val->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionRaceLabel_Transmission_Val->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Transmission_Val->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mOptionRaceLabel_Transmission_Val);
    }
    {
        //mTransmissionValLeft.setBackgroundMaterial(mOpponentsValLeft.getMaterialName());
        //mTransmissionValLeft.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f, 82.0f, 12.0f, 12.0f));
        //mTransmissionValLeft.setButtonOnAction(this);
    }

    //Options Race KMPH
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 102.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_KMPH = createTextArea("MainWindowOptionRaceKMPHLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionRaceLabel_KMPH->setCaption("Speed Display");
        mOptionRaceLabel_KMPH->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_KMPH->setSpaceWidth(9.0f);
        mOptionRaceLabel_KMPH->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_KMPH->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionRaceLabel_KMPH->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_KMPH->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionRaceLabel_KMPH);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 102.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_KMPH_Val = createTextArea("MainWindowOptionRaceKMPHValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        if(mModeContext.getGameState().isKMPh())
            mOptionRaceLabel_KMPH_Val->setCaption("Kph");
        else
            mOptionRaceLabel_KMPH_Val->setCaption("Mph");
        mOptionRaceLabel_KMPH_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_KMPH_Val->setSpaceWidth(9.0f);
        mOptionRaceLabel_KMPH_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_KMPH_Val->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionRaceLabel_KMPH_Val->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_KMPH_Val->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mOptionRaceLabel_KMPH_Val);
    }

    //Options Race Mirror
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 122.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Mirror = createTextArea("MainWindowOptionRaceMirrorLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionRaceLabel_Mirror->setCaption("Rearview Mirror");
        mOptionRaceLabel_Mirror->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Mirror->setSpaceWidth(9.0f);
        mOptionRaceLabel_Mirror->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Mirror->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionRaceLabel_Mirror->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Mirror->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionRaceLabel_Mirror);
    }
    {
        mMirrorVal = mUIButtonTicksManager.add("mRace");
        mMirrorVal->loadBackground("OriginalButtonTick");
        mMirrorVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 122.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getGameState().getMirrorEnabled(), true);
        mMirrorVal->setButtonOnAction(this);
    }

    //Options Race Ghost
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 142.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Ghost = createTextArea("MainWindowOptionRaceGhostLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionRaceLabel_Ghost->setCaption("Timetrial Ghost");
        mOptionRaceLabel_Ghost->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Ghost->setSpaceWidth(9.0f);
        mOptionRaceLabel_Ghost->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Ghost->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionRaceLabel_Ghost->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Ghost->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionRaceLabel_Ghost);
    }
    {
        mGhostVal = mUIButtonTicksManager.add("mRace");
        mGhostVal->loadBackground("OriginalButtonTick");
        mGhostVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 142.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getGameState().getGhostEnabled(), true);
        mGhostVal->setButtonOnAction(this);
    }

    //Options Highscores
    {
        mHighScoresTrackLeft = mUIButtonsManager.add();
        mHighScoresTrackLeft->setBackgroundMaterial(mInputTypeValLeft->getMaterialName());
        mHighScoresTrackLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(100.0f + buttonLeftAdj, 32.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mHighScoresTrackLeft->setButtonOnAction(this);

        mHighScoresTrackRight = mUIButtonsManager.add();
        mHighScoresTrackRight->setBackgroundMaterial(mInputTypeValRight->getMaterialName());
        mHighScoresTrackRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(180.0f + buttonLeftAdj, 32.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mHighScoresTrackRight->setButtonOnAction(this);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(128.0f, 32.0f, 0.0f, 0.0f);
        mOptionHighScoresButtonLabel = createTextArea("MainWindowOptionHighscoresButtonLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionHighScoresButtonLabel->setCaption("TRACK");
        mOptionHighScoresButtonLabel->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionHighScoresButtonLabel->setSpaceWidth(9.0f);
        mOptionHighScoresButtonLabel->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionHighScoresButtonLabel->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionHighScoresButtonLabel->setFontName("SdkTrays/Caption");
        mOptionHighScoresButtonLabel->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionHighScoresButtonLabel);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(211.0f, 32.0f, 0.0f, 0.0f);
        mOptionHighScoresTrackLabel = createTextArea("MainWindowOptionHighscoresTrackLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionHighScoresTrackLabel->setCaption("SandBlaster");
        mOptionHighScoresTrackLabel->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionHighScoresTrackLabel->setSpaceWidth(9.0f);
        mOptionHighScoresTrackLabel->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionHighScoresTrackLabel->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionHighScoresTrackLabel->setFontName("SdkTrays/Caption");
        mOptionHighScoresTrackLabel->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionHighScoresTrackLabel);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(15.0f, 62.0f, 0.0f, 0.0f);
        mOptionHighScoresReplayLabel = createTextArea("MainWindowOptionHighscoresReplayLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionHighScoresReplayLabel->setCaption("");
        mOptionHighScoresReplayLabel->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionHighScoresReplayLabel->setSpaceWidth(9.0f);
        mOptionHighScoresReplayLabel->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionHighScoresReplayLabel->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionHighScoresReplayLabel->setFontName("SdkTrays/Caption");
        mOptionHighScoresReplayLabel->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionHighScoresReplayLabel);
    }
    {
        const Ogre::Real left = 250.0f;
        const Ogre::Real top = 55.0f;
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(left, top, left + 52.0f, top + 20.0f);
        mOptionHighScoresReplayIcon = createPanel("MainWindowHiscoreReplayIcon", pos, "Test/car0_0s.bmp");
        mOptionHighScoresReplayIcon->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        getMainBackground()->addChild(mOptionHighScoresReplayIcon);
    }
    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(15.0f, 20.0f * q + 90.0f, 0.0f, 0.0f);
            mOptionHighScoresTable1Label[q] = createTextArea("MainWindowHiscoreTable1_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mOptionHighScoresTable1Label[q]->setCaption(Conversions::DMToString(q + 1));
            mOptionHighScoresTable1Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mOptionHighScoresTable1Label[q]->setSpaceWidth(9.0f);
            mOptionHighScoresTable1Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mOptionHighScoresTable1Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mOptionHighScoresTable1Label[q]->setFontName("SdkTrays/Caption");
            mOptionHighScoresTable1Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mOptionHighScoresTable1Label[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(40.0f, 20.0f * q + 90.0f, 0.0f, 0.0f);
            mOptionHighScoresTable2Label[q] = createTextArea("MainWindowHiscoreTable2_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mOptionHighScoresTable2Label[q]->setCaption("");
            mOptionHighScoresTable2Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mOptionHighScoresTable2Label[q]->setSpaceWidth(9.0f);
            mOptionHighScoresTable2Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mOptionHighScoresTable2Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mOptionHighScoresTable2Label[q]->setFontName("SdkTrays/Caption");
            mOptionHighScoresTable2Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mOptionHighScoresTable2Label[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(190.0f, 20.0f * q + 90.0f, 0.0f, 0.0f);
            mOptionHighScoresTable3Label[q] = createTextArea("MainWindowHiscoreTable3_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mOptionHighScoresTable3Label[q]->setCaption("");
            mOptionHighScoresTable3Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mOptionHighScoresTable3Label[q]->setSpaceWidth(9.0f);
            mOptionHighScoresTable3Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mOptionHighScoresTable3Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mOptionHighScoresTable3Label[q]->setFontName("SdkTrays/Caption");
            mOptionHighScoresTable3Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mOptionHighScoresTable3Label[q]);
        }

        {
            const Ogre::Real left = 250.0f;
            const Ogre::Real top = 85.0f;
            Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(left, 20.0f * q + top, left + 52.0f, 20.0f * q + top + 20.0f);
            mOptionHighScoresTable4Icon[q] = createPanel("MainWindowHiscoreTable4Icon_" + Conversions::DMToString(q), pos, "Test/car0_0s.bmp");
            mOptionHighScoresTable4Icon[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            getMainBackground()->addChild(mOptionHighScoresTable4Icon[q]);
        }
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(100.0f, 330.0f, 0.0f, 0.0f);
        mOptionHighScoresEmergentGunLabel = createTextArea("MainWindowOptionHighscoresEmergentGunLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionHighScoresEmergentGunLabel->setCaption("");
        mOptionHighScoresEmergentGunLabel->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mOptionHighScoresEmergentGunLabel->setSpaceWidth(9.0f);
        mOptionHighScoresEmergentGunLabel->setHeight(16.0f * viewportHeight / 1024.0f);
        mOptionHighScoresEmergentGunLabel->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionHighScoresEmergentGunLabel->setFontName("SdkTrays/Caption");
        mOptionHighScoresEmergentGunLabel->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionHighScoresEmergentGunLabel);
    }


    //Options Name
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 62.0f, 0.0f, 0.0f);;
        mOptionNameLabel = createTextArea("MainWindowOptionNameLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionNameLabel->setCaption("Player Name");
        mOptionNameLabel->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionNameLabel->setSpaceWidth(9.0f);
        mOptionNameLabel->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionNameLabel->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionNameLabel->setFontName("SdkTrays/Caption");
        mOptionNameLabel->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionNameLabel);
    }
    //Options Name Save
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(325.0f, 260.0f, 0.0f, 0.0f);;
        mOptionNameLabel_Save = createTextArea("MainWindowOptionNameSaveLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionNameLabel_Save->setCaption("Save");
        mOptionNameLabel_Save->setCharHeight(56.0f * viewportHeight / 1024.0f);
        mOptionNameLabel_Save->setSpaceWidth(9.0f);
        mOptionNameLabel_Save->setHeight(56.0f * viewportHeight / 1024.0f);
        mOptionNameLabel_Save->setAlignment(Ogre::TextAreaOverlayElement::Center);
        mOptionNameLabel_Save->setFontName("SdkTrays/Caption");
        mOptionNameLabel_Save->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mOptionNameLabel_Save);
    }

    //Options Version
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(630.0f, 360.0f, 0.0f, 0.0f);
        mOptionVersionLabel = createTextArea("MainWindowOptionVersionLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionVersionLabel->setCaption(mModeContext.getGameState().getVersion());
        mOptionVersionLabel->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionVersionLabel->setSpaceWidth(9.0f);
        mOptionVersionLabel->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionVersionLabel->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionVersionLabel->setFontName("SdkTrays/Caption");
        mOptionVersionLabel->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionVersionLabel);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(310.0f, 361.0f, 0.0f, 0.0f);
        mStartingGridTimeLabel = createTextArea("MainWindowTimer", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mStartingGridTimeLabel->setCaption("5");
        mStartingGridTimeLabel->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mStartingGridTimeLabel->setSpaceWidth(9.0f);
        mStartingGridTimeLabel->setHeight(26.0f * viewportHeight / 1024.0f);
        mStartingGridTimeLabel->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mStartingGridTimeLabel->setFontName("SdkTrays/Caption");
        mStartingGridTimeLabel->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mStartingGridTimeLabel);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 198.0f, 0.0f, 0.0f);
        mExitLabel = createTextArea("MainWindowExitGame", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mExitLabel->setCaption("Exit the game?");
        mExitLabel->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mExitLabel->setSpaceWidth(9.0f);
        mExitLabel->setHeight(46.0f * viewportHeight / 1024.0f);
        mExitLabel->setAlignment(Ogre::TextAreaOverlayElement::Center);
        mExitLabel->setFontName("SdkTrays/Caption");
        mExitLabel->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mExitLabel);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(270.0f, 252.0f, 0.0f, 0.0f);
        mGameExitYesLabel = mUILabelsManager.add("mGameExit");
        mGameExitYesLabel->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mGameExitYesLabel->getTextArea()->setCaption("YES");
        mGameExitYesLabel->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitYesLabel->getTextArea()->setSpaceWidth(9.0f);
        mGameExitYesLabel->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitYesLabel->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mGameExitYesLabel->getTextArea()->setFontName("SdkTrays/Caption");
        mGameExitYesLabel->getTextArea()->setColour(UILabel::mInactiveLabel);
        mGameExitYesLabel->setLabelOnAction(this);
        getMainBackground()->addChild(mGameExitYesLabel->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(334.0f, 252.0f, 0.0f, 0.0f);
        mGameExitNoLabel = mUILabelsManager.add("mGameExit");
        mGameExitNoLabel->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mGameExitNoLabel->getTextArea()->setCaption("NO");
        mGameExitNoLabel->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitNoLabel->getTextArea()->setSpaceWidth(9.0f);
        mGameExitNoLabel->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitNoLabel->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mGameExitNoLabel->getTextArea()->setFontName("SdkTrays/Caption");
        mGameExitNoLabel->getTextArea()->setColour(UILabel::mInactiveLabel);
        mGameExitNoLabel->setLabelOnAction(this);
        getMainBackground()->addChild(mGameExitNoLabel->getTextArea());
    }

    for(size_t q = 0, u = 0, l = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        Ogre::Vector4 textBoxPos;
        if(q % 2 == 0)
        {
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(u * 100.0f + 1.0f, 59.0f, 0.0f, 0.0f);
            ++u;
        }
        else
        {
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(l * 100.0f + 1.0f + 40.0f, 209.0f, 0.0f, 0.0f);
            ++l;
        }
        textBoxPos.y -= 11.0f * viewportHeight / 1024.0f;
        mRaceGridCharactersLabel[q] = createTextArea("MainWindowStartingGrid_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mRaceGridCharactersLabel[q]->setCaption(Conversions::DMToString(q + 1));
        mRaceGridCharactersLabel[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mRaceGridCharactersLabel[q]->setSpaceWidth(9.0f);
        mRaceGridCharactersLabel[q]->setHeight(26.0f * viewportHeight / 1024.0f);
        mRaceGridCharactersLabel[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mRaceGridCharactersLabel[q]->setFontName("SdkTrays/Caption");
        mRaceGridCharactersLabel[q]->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mRaceGridCharactersLabel[q]);
    }

    for(size_t q = 0; q < mPodiumCharacters; ++q)
    {
        Ogre::Vector4 textBoxPos;

        if(q == 0)
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(115.0f, 181.0f, 0.0f, 0.0f);
        if(q == 1)
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(231.0f, 281.0f, 0.0f, 0.0f);
        if(q == 2)
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(141.0f, 332.0f, 0.0f, 0.0f);

        mPodiumCharactersFirstSecondThirdLabel[q] = createTextArea("MainWindowPodium123_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mPodiumCharactersFirstSecondThirdLabel[q]->setCaption("");
        mPodiumCharactersFirstSecondThirdLabel[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mPodiumCharactersFirstSecondThirdLabel[q]->setSpaceWidth(9.0f);
        mPodiumCharactersFirstSecondThirdLabel[q]->setHeight(26.0f * viewportHeight / 1024.0f);
        mPodiumCharactersFirstSecondThirdLabel[q]->setAlignment(Ogre::TextAreaOverlayElement::Center);
        mPodiumCharactersFirstSecondThirdLabel[q]->setFontName("SdkTrays/Caption");
        mPodiumCharactersFirstSecondThirdLabel[q]->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mPodiumCharactersFirstSecondThirdLabel[q]);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(335.0f, 57.0f, 0.0f, 0.0f);
        mPodiumTableTitle1Label = createTextArea("MainWindowTableTitle1", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mPodiumTableTitle1Label->setCaption("Pos");
        mPodiumTableTitle1Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle1Label->setSpaceWidth(9.0f);
        mPodiumTableTitle1Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle1Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mPodiumTableTitle1Label->setFontName("SdkTrays/Caption");
        mPodiumTableTitle1Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mPodiumTableTitle1Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(360.0f, 57.0f, 0.0f, 0.0f);
        mPodiumTableTitle2Label = createTextArea("MainWindowTableTitle2", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mPodiumTableTitle2Label->setCaption("Name");
        mPodiumTableTitle2Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle2Label->setSpaceWidth(9.0f);
        mPodiumTableTitle2Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle2Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mPodiumTableTitle2Label->setFontName("SdkTrays/Caption");
        mPodiumTableTitle2Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mPodiumTableTitle2Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(490.0f, 57.0f, 0.0f, 0.0f);
        mPodiumTableTitle3Label = createTextArea("MainWindowTableTitle3", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mPodiumTableTitle3Label->setCaption("Race Time");
        mPodiumTableTitle3Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle3Label->setSpaceWidth(9.0f);
        mPodiumTableTitle3Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle3Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mPodiumTableTitle3Label->setFontName("SdkTrays/Caption");
        mPodiumTableTitle3Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mPodiumTableTitle3Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(560.0f, 57.0f, 0.0f, 0.0f);
        mPodiumTableTitle4Label = createTextArea("MainWindowTableTitle4", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mPodiumTableTitle4Label->setCaption("Best Lap");
        mPodiumTableTitle4Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle4Label->setSpaceWidth(9.0f);
        mPodiumTableTitle4Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle4Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mPodiumTableTitle4Label->setFontName("SdkTrays/Caption");
        mPodiumTableTitle4Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mPodiumTableTitle4Label);
    }


    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(335.0f, 57.0f, 0.0f, 0.0f);
        mLeaderboardTableTitle1Label = createTextArea("MainWindowTableLeaderboardTitle1", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mLeaderboardTableTitle1Label->setCaption("Rank");
        mLeaderboardTableTitle1Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle1Label->setSpaceWidth(9.0f);
        mLeaderboardTableTitle1Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle1Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mLeaderboardTableTitle1Label->setFontName("SdkTrays/Caption");
        mLeaderboardTableTitle1Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mLeaderboardTableTitle1Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(360.0f, 57.0f, 0.0f, 0.0f);
        mLeaderboardTableTitle2Label = createTextArea("MainWindowTableLeaderboardTitle2", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mLeaderboardTableTitle2Label->setCaption("Name");
        mLeaderboardTableTitle2Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle2Label->setSpaceWidth(9.0f);
        mLeaderboardTableTitle2Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle2Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mLeaderboardTableTitle2Label->setFontName("SdkTrays/Caption");
        mLeaderboardTableTitle2Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mLeaderboardTableTitle2Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(490.0f, 57.0f, 0.0f, 0.0f);
        mLeaderboardTableTitle3Label = createTextArea("MainWindowTableLeaderboardTitle3", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mLeaderboardTableTitle3Label->setCaption("Vehicle");
        mLeaderboardTableTitle3Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle3Label->setSpaceWidth(9.0f);
        mLeaderboardTableTitle3Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle3Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mLeaderboardTableTitle3Label->setFontName("SdkTrays/Caption");
        mLeaderboardTableTitle3Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mLeaderboardTableTitle3Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(560.0f, 57.0f, 0.0f, 0.0f);
        mLeaderboardTableTitle4Label = createTextArea("MainWindowTableLeaderboardTitle4", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mLeaderboardTableTitle4Label->setCaption("Points");
        mLeaderboardTableTitle4Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle4Label->setSpaceWidth(9.0f);
        mLeaderboardTableTitle4Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle4Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mLeaderboardTableTitle4Label->setFontName("SdkTrays/Caption");
        mLeaderboardTableTitle4Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mLeaderboardTableTitle4Label);
    }

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(335.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mPodiumTable1Label[q] = createTextArea("MainWindowTable1_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mPodiumTable1Label[q]->setCaption(Conversions::DMToString(q + 1));
            mPodiumTable1Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable1Label[q]->setSpaceWidth(9.0f);
            mPodiumTable1Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable1Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mPodiumTable1Label[q]->setFontName("SdkTrays/Caption");
            mPodiumTable1Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mPodiumTable1Label[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(360.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mPodiumTable2Label[q] = createTextArea("MainWindowTable2_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mPodiumTable2Label[q]->setCaption("");
            mPodiumTable2Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable2Label[q]->setSpaceWidth(9.0f);
            mPodiumTable2Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable2Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mPodiumTable2Label[q]->setFontName("SdkTrays/Caption");
            mPodiumTable2Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mPodiumTable2Label[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(490.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mPodiumTable3Label[q] = createTextArea("MainWindowTable3_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mPodiumTable3Label[q]->setCaption("");
            mPodiumTable3Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable3Label[q]->setSpaceWidth(9.0f);
            mPodiumTable3Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable3Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mPodiumTable3Label[q]->setFontName("SdkTrays/Caption");
            mPodiumTable3Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mPodiumTable3Label[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(560.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mPodiumTable4Label[q] = createTextArea("MainWindowTable4_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mPodiumTable4Label[q]->setCaption("");
            mPodiumTable4Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable4Label[q]->setSpaceWidth(9.0f);
            mPodiumTable4Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable4Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mPodiumTable4Label[q]->setFontName("SdkTrays/Caption");
            mPodiumTable4Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mPodiumTable4Label[q]);
        }


        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(335.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mLeaderboardTable1Label[q] = createTextArea("MainWindowLeaderTable1_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mLeaderboardTable1Label[q]->setCaption(Conversions::DMToString(q + 1));
            mLeaderboardTable1Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable1Label[q]->setSpaceWidth(9.0f);
            mLeaderboardTable1Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable1Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mLeaderboardTable1Label[q]->setFontName("SdkTrays/Caption");
            mLeaderboardTable1Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mLeaderboardTable1Label[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(360.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mLeaderboardTable2Label[q] = createTextArea("MainWindowLeaderTable2_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mLeaderboardTable2Label[q]->setCaption("");
            mLeaderboardTable2Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable2Label[q]->setSpaceWidth(9.0f);
            mLeaderboardTable2Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable2Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mLeaderboardTable2Label[q]->setFontName("SdkTrays/Caption");
            mLeaderboardTable2Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mLeaderboardTable2Label[q]);
        }

        {
            const Ogre::Real left = 490.0f;
            const Ogre::Real top = 80.0f;
            Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(left, 22.0f * q + top, left + 52.0f, 22.0f * q + top + 18.0f);
            mLeaderboardTable3Icon[q] = createPanel("MainWindowLeaderTable3Icon_" + Conversions::DMToString(q), pos, "Test/car0_0s.bmp");
            mLeaderboardTable3Icon[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            getMainBackground()->addChild(mLeaderboardTable3Icon[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(560.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mLeaderboardTable4Label[q] = createTextArea("MainWindowLeaderTable4_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mLeaderboardTable4Label[q]->setCaption("");
            mLeaderboardTable4Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable4Label[q]->setSpaceWidth(9.0f);
            mLeaderboardTable4Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable4Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mLeaderboardTable4Label[q]->setFontName("SdkTrays/Caption");
            mLeaderboardTable4Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mLeaderboardTable4Label[q]);
        }
    }

    //championship results
    for(size_t q = 0; q < mChampionshipResultsLabelsAmountMax; ++q)
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 40.0f + q * 20.0f, 0.0f, 0.0f);
        mChampionshipResultsLabel[q] = createTextArea("MainWindowChampionshipResults" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mChampionshipResultsLabel[q]->setCaption("");
        mChampionshipResultsLabel[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mChampionshipResultsLabel[q]->setSpaceWidth(9.0f);
        mChampionshipResultsLabel[q]->setHeight(26.0f * viewportHeight / 1024.0f);
        mChampionshipResultsLabel[q]->setAlignment(Ogre::TextAreaOverlayElement::Center);
        mChampionshipResultsLabel[q]->setFontName("SdkTrays/Caption");
        mChampionshipResultsLabel[q]->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mChampionshipResultsLabel[q]);
    }



    //Track description, bio`s
    if(mModeContext.getGameState().isPatchDataInited())
    {
        mTrackDesc.clear();
        mCharacterDesc.clear();

        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

        for(size_t q = 0; q < availTracks.size(); ++q)
        {
            std::string desc = getTextFileContent("data/tracks/" + strPowerslide.getBaseDir(availTracks[q]), "english.blah");
            if(!desc.empty())
            {
                mTrackDesc.insert(std::make_pair(availTracks[q], desc));
            }
        }

        std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");

        for(size_t q = 0; q < availableCharacters.size(); ++q)
        {
            std::string folder = strPowerslide.getCharacterFolderForBio(availableCharacters[q]);
            if(!folder.empty())
            {
                std::string desc = getTextFileContent("data/gameshell/bios/" + folder, "english.blah");
                if(!desc.empty())
                {
                    mCharacterDesc.insert(std::make_pair(availableCharacters[q], desc));
                }
            }
        }
    }
}

void UIMainMenuLabels::mousePressed(const Ogre::Vector2& pos)
{
    UIBaseMenu::mousePressed(pos);

    mUIButtonsManager.mousePressed(pos);
    mUIButtonTicksManager.mousePressed(pos);
    mUILabelsManager.mousePressed(pos);
}

void UIMainMenuLabels::mouseReleased(const Ogre::Vector2& pos)
{
    UIBaseMenu::mouseReleased(pos);

    AIStrength gameLevel = mModeContext.getGameState().getPlayerData().level;

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();

    if(mSingleTrackViewBySelection->isVisible() && OgreBites::Widget::isCursorOver(mSingleTrackViewBySelection, pos, 0))
    {
        mIsViewByDescription = !mIsViewByDescription;

        if(mIsViewByDescription)
        {
            mSingleTrackViewBySelection->setCaption("Description");
            hideBackgroundTrack();
            mSingleTrackDescription->show();
        }
        else
        {
            mSingleTrackViewBySelection->setCaption("Image");
            showBackgroundTrack();
            mSingleTrackDescription->hide();
        }


        return;
    }

    if(mSingleBioViewBySelection->isVisible() && OgreBites::Widget::isCursorOver(mSingleBioViewBySelection, pos, 0))
    {
        mIsBioByDescription = !mIsBioByDescription;

        if(mIsBioByDescription)
        {
            mSingleBioViewBySelection->setCaption("Description");
            hideBackgroundCharacter();
            mSingleBioDescription->show();
        }
        else
        {
            mSingleBioViewBySelection->setCaption("Bio");
            showBackgroundCharacter();
            mSingleBioDescription->hide();
        }


        return;
    }

    if(mOptionGraphicsLabel_Resolution_Val->isVisible() && OgreBites::Widget::isCursorOver(mOptionGraphicsLabel_Resolution_Val, pos, 0))
    {
            Ogre::RenderSystem * rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
            Ogre::ConfigOptionMap configOpts = rs->getConfigOptions();
            Ogre::ConfigOption videoMode = configOpts["Video Mode"];
            size_t curVal = 0;
            for(size_t q = 0; q < videoMode.possibleValues.size(); ++q)
            {
                if(mOptionGraphicsLabel_Resolution_Val->getCaption() == videoMode.possibleValues[q])
                {
                    curVal = q;
                    break;
                }
            }
            ++curVal;
            if(curVal >= videoMode.possibleValues.size()) curVal = 0;
            mOptionGraphicsLabel_Resolution_Val->setCaption(videoMode.possibleValues[curVal]);

            Ogre::Real viewBySize = getTextWidth(mOptionGraphicsLabel_Resolution_Val->getCaption(), mOptionGraphicsLabel_Resolution_Val);
            mOptionGraphicsLabel_Resolution_Apply->setLeft(mOptionGraphicsLabel_Resolution_Val->getLeft() + viewBySize + 9.0f);
    }

    if(mOptionGraphicsLabel_Resolution_Apply->isVisible() && OgreBites::Widget::isCursorOver(mOptionGraphicsLabel_Resolution_Apply, pos, 0))
    {
        Ogre::RenderSystem * rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
        Ogre::ConfigOptionMap& configOpts = rs->getConfigOptions();
        Ogre::ConfigOption& videoMode = configOpts["Video Mode"];
        unsigned int desiredWidth, desiredHeight;
        std::string desiredRes = mOptionGraphicsLabel_Resolution_Val->getCaption();
        videoMode.currentValue = desiredRes;
        sscanf(desiredRes.c_str(), "%d x %d", &desiredWidth, &desiredHeight);
        if(mModeContext.getRenderWindow()->isFullScreen())
        {
            mModeContext.getRenderWindow()->setFullscreen(true, desiredWidth, desiredHeight);
        }
        else
        {
            mModeContext.getRenderWindow()->setFullscreen(false, desiredWidth, desiredHeight);
            mModeContext.getRenderWindow()->resize(desiredWidth, desiredHeight);
        }

        mModeContext.getGameState().savePlayerData();

        mModeContext.getGameModeSwitcher()->recreateMenu();
    }

    if(mOptionRaceLabel_Opponents_Val->isVisible() && OgreBites::Widget::isCursorOver(mOptionRaceLabel_Opponents_Val, pos, 0))
    {
        size_t aiCount = mModeContext.getGameState().getAICount();
        ++aiCount;
        if(aiCount > GameState::mAIMax) aiCount = GameState::mAIMin;

        mModeContext.getGameState().setAICount(aiCount);
        mModeContext.getGameState().setAICountInRace(aiCount);
        mOptionRaceLabel_Opponents_Val->setCaption(Conversions::DMToString(mModeContext.getGameState().getAICountInRace()));
    }

    if(mOptionRaceLabel_Transmission_Val->isVisible() && OgreBites::Widget::isCursorOver(mOptionRaceLabel_Transmission_Val, pos, 0))
    {
        if(mModeContext.getGameState().getTransmissionType() == trAuto)
        {
            mModeContext.getGameState().setTransmissionType(trManual);
            mOptionRaceLabel_Transmission_Val->setCaption("Manual");
        }
        else
        {
            mModeContext.getGameState().setTransmissionType(trAuto);
            mOptionRaceLabel_Transmission_Val->setCaption("Auto");
        }

        mModeContext.getGameState().savePlayerData();
    }

    if(mOptionRaceLabel_KMPH_Val->isVisible() && OgreBites::Widget::isCursorOver(mOptionRaceLabel_KMPH_Val, pos, 0))
    {
        if(mModeContext.getGameState().isKMPh())
        {
            mModeContext.getGameState().setKMPh(false);
            mOptionRaceLabel_KMPH_Val->setCaption("Mph");
        }
        else
        {
            mModeContext.getGameState().setKMPh(true);
            mOptionRaceLabel_KMPH_Val->setCaption("Kph");
        }

        mModeContext.getGameState().savePlayerData();
    }

    if(mOptionNameLabel_Save->isVisible() && OgreBites::Widget::isCursorOver(mOptionNameLabel_Save, pos, 0))
    {
        onNameChange();
    }

    mUIButtonsManager.mouseReleased(pos);
    mUIButtonTicksManager.mouseReleased(pos);
    mUILabelsManager.mouseReleased(pos);
}

void UIMainMenuLabels::mouseMoved(const Ogre::Vector2& pos)
{
    UIMainMenuBackground::mouseMoved(pos);

    checkCursorOverLabel(pos, mSingleTrackViewBySelection);
    checkCursorOverLabel(pos, mSingleBioViewBySelection);

    checkCursorOverLabel(pos, mOptionGraphicsLabel_Resolution_Val);
    checkCursorOverLabel(pos, mOptionGraphicsLabel_Resolution_Apply);
    checkCursorOverLabel(pos, mOptionRaceLabel_Opponents_Val);
    checkCursorOverLabel(pos, mOptionRaceLabel_Transmission_Val);
    checkCursorOverLabel(pos, mOptionRaceLabel_KMPH_Val);
    checkCursorOverLabel(pos, mOptionNameLabel_Save);

    mUIButtonsManager.mouseMoved(pos);
    mUIButtonTicksManager.mouseMoved(pos);
    mUILabelsManager.mouseMoved(pos);
}

void UIMainMenuLabels::destroy(CustomTrayManager* trayMgr)
{
    UIBase::destroy(trayMgr);

    mUIButtonsManager.destroy(trayMgr);
    mUIButtonTicksManager.destroy(trayMgr);
    mUILabelsManager.destroy(trayMgr);
}

void UIMainMenuLabels::setWindowTitle(const std::string& title)
{
    mWindowTitle->setCaption(title);
}

void UIMainMenuLabels::showModeSingleMulti()
{
    mModeSingle->show();
    mModeMulti->show();
}

void UIMainMenuLabels::showModeSingleType()
{
    mModeSingleTypeRace->show();
    mModeSingleTypeChampionship->show();
    mModeSingleTypeTimetrial->show();
}

void UIMainMenuLabels::showModeDifficulty()
{
    AIStrength gameLevel = mModeContext.getGameState().getPlayerData().level;

    mModeSingleDifficultyAdvanced->setActive(true);
    mModeSingleDifficultyExpert->setActive(true);
    mModeSingleDifficultyInsane->setActive(true);

    if(gameLevel < Medium) mModeSingleDifficultyAdvanced->setActive(false);
    if(gameLevel < Hard) mModeSingleDifficultyExpert->setActive(false);
    if(gameLevel < Insane) mModeSingleDifficultyInsane->setActive(false);

    mModeSingleDifficultyNovice->show();
    mModeSingleDifficultyAdvanced->show();
    mModeSingleDifficultyExpert->show();
    mModeSingleDifficultyInsane->show();
}

void UIMainMenuLabels::showTrackLabels()
{
    AIStrength gameLevel = mModeContext.getGameState().getPlayerData().level;

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    for(size_t q = 0; q < mTracksLabels.size(); ++q)
    {
        int difficultyAvailable = strPowerslide.getIntValue(availTracks[q] + " parameters", "difficulty available", 0);

        if(difficultyAvailable <= gameLevel)
        {
            mTracksLabels[q]->setActive(true);
            mTracksLabels[q]->show();
        }

        if(difficultyAvailable == (gameLevel + 1))
        {
            mTracksLabels[q]->setActive(false);
            mTracksLabels[q]->show();
        }
    }

    if(mIsViewByDescription)
    {
        mSingleTrackViewBySelection->setCaption("Description");
        hideBackgroundTrack();
        mSingleTrackDescription->show();
    }
    else
    {
        mSingleTrackViewBySelection->setCaption("Image");
        showBackgroundTrack();
        mSingleTrackDescription->hide();
    }

    mSingleTrackBestTime->show();

    if(mModeContext.getGameState().isPatchDataInited())
    {
        mSingleTrackViewBy->show();
        mSingleTrackViewBySelection->show();
    }
}

void UIMainMenuLabels::showCarLabels()
{
    AIStrength gameLevel = mModeContext.getGameState().getPlayerData().level;

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");

    for(size_t q = 0; q < mCarsLabels.size(); ++q)
    {
        int difficultyAvailable = strPowerslide.getIntValue(availCars[q] + " parameters", "difficulty available", 0);

        if(difficultyAvailable <= gameLevel)
        {
            mCarsLabels[q]->show();
        }
    }

    mCarParam_Mass->show();
    mCarParam_Power->show();
    mCarParam_Acceleration->show();
    mCarParam_Traction->show();
    mCarParam_TopSpeed->show();
    mCarParam_AeroDynamics->show();
    mCarParam_Stability->show();
}

void UIMainMenuLabels::showCharacterLabels()
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::string character = mModeContext.getGameState().getPlayerCar().getCharacterName();
    std::string characterCar = strPowerslide.getCarFromCharacter(character);
    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
    characterCar = strPowerslide.getBaseCarFromCar(characterCar);
    std::vector<std::string> availCharsForCar = strPowerslide.getCharactersByBaseCar(characterCar);

    if(mIsBioByDescription)
    {
        hideBackgroundCharacter();
        mSingleBioDescription->show();
        mSingleBioViewBySelection->setCaption("Description");
    }
    else
    {
        showBackgroundCharacter();
        mSingleBioDescription->hide();
        mSingleBioViewBySelection->setCaption("Bio");
    }

    AIStrength gameLevel = mModeContext.getGameState().getPlayerData().level;

    for(size_t q = 0; q < mCharactersLabels.size(); ++q)
    {
        if(mCharactersLabels[q]->getTextArea()->getCaption() != "")
        {
            if(availCharsForCar.size() > q)
            {
                int difficultyAvailable = strPowerslide.getIntValue(availCharsForCar[q] + " parameters", "difficulty available", 0);

                if(difficultyAvailable <= gameLevel)
                {
                    mCharactersLabels[q]->setActive(true);
                    mCharactersLabels[q]->show();
                }

                if(difficultyAvailable == (gameLevel + 1))
                {
                    mCharactersLabels[q]->setActive(false);
                    mCharactersLabels[q]->show();
                }
            }
        }
    }
    for (size_t q = 0; q < availableCharacters.size(); ++q)
    {
        if(STRPowerslide::getCharacterTitle(availableCharacters[q]) == mCharactersLabels[0]->getTextArea()->getCaption().asUTF8())
            setCharacterLogo(q);
    }

    if(mModeContext.getGameState().isPatchDataInited())
    {
        mSingleBioViewBy->show();
        mSingleBioViewBySelection->show();
    }

}

void UIMainMenuLabels::showOptionLabels()
{
    mUILabelsManager.show("mOptionLabels");

    mOptionVersionLabel->show();

#ifdef NO_OPENAL
    mOptionLabels[2]->hide();
#endif
}

void UIMainMenuLabels::showOptionGraphicsLabels()
{
    mOptionGraphicsLabel_Vendor->show();
    mOptionGraphicsLabel_Vendor_Val->show();

    mOptionGraphicsLabel_Renderer->show();
    mOptionGraphicsLabel_Renderer_Val->show();

    mOptionGraphicsLabel_Shadow->show();

    mOptionGraphicsLabel_VSync->show();

    mOptionGraphicsLabel_Fulscreen->show();

    Ogre::RenderSystem * rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
    Ogre::ConfigOptionMap configOpts = rs->getConfigOptions();
    Ogre::ConfigOption videoMode = configOpts["Video Mode"];
    mOptionGraphicsLabel_Resolution_Val->setCaption(videoMode.currentValue);
    mOptionGraphicsLabel_Resolution->show();
    mOptionGraphicsLabel_Resolution_Val->show();
    mOptionGraphicsLabel_Resolution_Apply->show();

    mUIButtonTicksManager.show("mGraphics");
}

void UIMainMenuLabels::showOptionInputLabels()
{
    mOptionInputLabel_Type->show();

    mUIButtonsManager.show("mInputType");
}

void UIMainMenuLabels::showOptionSoundLabels()
{
    mOptionSoundLabel_VolumeFX_Val->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getSoundsGain() * 9.0f)));

    mOptionSoundLabel_VolumeFX->show();
    mOptionSoundLabel_VolumeFX_Val->show();

    mOptionSoundLabel_VolumeMusic_Val->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getMusicGain() * 9.0f)));

    mOptionSoundLabel_VolumeMusic->show();
    mOptionSoundLabel_VolumeMusic_Val->show();

    mUIButtonsManager.show("mSoundVolume");
}

void UIMainMenuLabels::showOptionRaceLabels()
{
    mOptionRaceLabel_Opponents->show();
    mOptionRaceLabel_Opponents_Val->show();
    mOptionRaceLabel_Transmission->show();
    mOptionRaceLabel_Transmission_Val->show();
    mOptionRaceLabel_KMPH->show();
    mOptionRaceLabel_KMPH_Val->show();
    mOptionRaceLabel_Mirror->show();
    mOptionRaceLabel_Ghost->show();

    mUIButtonsManager.show("mOpponents");
    mUIButtonTicksManager.show("mRace");
}

void UIMainMenuLabels::showOptionHiscoreLabels()
{
    mHighScoreTrackIndex = 0;

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    mOptionHighScoresTrackLabel->setCaption(strPowerslide.getTrackTitle(availTracks[mHighScoreTrackIndex]));

    fillHighScoreTable();
    fillReplayStat();

    mHighScoresTrackLeft->show();
    mHighScoresTrackRight->show();
    mOptionHighScoresButtonLabel->show();
    mOptionHighScoresTrackLabel->show();
    mOptionHighScoresEmergentGunLabel->show();

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        mOptionHighScoresTable1Label[q]->show();
        mOptionHighScoresTable2Label[q]->show();
        mOptionHighScoresTable3Label[q]->show();
        mOptionHighScoresTable4Icon[q]->show();
    }
}

void UIMainMenuLabels::showOptionNameLabels()
{
    //mOptionNameLabel->show();
#if defined(__ANDROID__)
    //mOptionNameLabel_Save->show();
#endif
}

void UIMainMenuLabels::showExitLabels(const std::string& title)
{
    mExitLabel->setCaption(title);
    mExitLabel->show();
    mUILabelsManager.show("mGameExit");
}

void UIMainMenuLabels::showRaceGridCharactersLabels()
{
    for(size_t q = 0; q < mModeContext.getGameState().getAICountInRace() + 1; ++q)
    {
        mRaceGridCharactersLabel[q]->show();
    }
}

void UIMainMenuLabels::showPodiumLabels(const finishBoardVec& finishBoard)
{
    std::string playerName = mModeContext.getGameState().getPlayerName();

    size_t podiumChars = mPodiumCharacters;
    if(finishBoard.size() < podiumChars) podiumChars = finishBoard.size();

    for(size_t q = 0; q < podiumChars; ++q)
    {
        if(finishBoard[q].mIsPlayer)
            mPodiumCharactersFirstSecondThirdLabel[q]->setCaption(playerName);
        else
            mPodiumCharactersFirstSecondThirdLabel[q]->setCaption(STRPowerslide::getCharacterTitle(finishBoard[q].mCharName));
        mPodiumCharactersFirstSecondThirdLabel[q]->show();
    }

    mPodiumTableTitle1Label->show();
    mPodiumTableTitle2Label->show();
    mPodiumTableTitle3Label->show();
    mPodiumTableTitle4Label->show();

    for(size_t q = 0; q < mModeContext.getGameState().getAICountInRace() + 1; ++q)
    {
        if(finishBoard[q].mIsPlayer)
            mPodiumTable2Label[q]->setCaption(playerName);
        else
            mPodiumTable2Label[q]->setCaption(STRPowerslide::getCharacterTitle(finishBoard[q].mCharName));

        if(finishBoard[q].mRaceTime != 0.0f)
            mPodiumTable3Label[q]->setCaption(Tools::SecondsToString(finishBoard[q].mRaceTime));
        else
            mPodiumTable3Label[q]->setCaption("DNF");

        if(finishBoard[q].mBestLapTime != 0.0f)
            mPodiumTable4Label[q]->setCaption(Tools::SecondsToString(finishBoard[q].mBestLapTime));
        else
            mPodiumTable4Label[q]->setCaption("N/A");

        mPodiumTable1Label[q]->show();
        mPodiumTable2Label[q]->show();
        mPodiumTable3Label[q]->show();
        mPodiumTable4Label[q]->show();
    }
}

void UIMainMenuLabels::showLeaderboardLabels(const finishBoardVec& finishBoard)
{
    std::string playerName = mModeContext.getGameState().getPlayerName();

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();

    //mLeaderboardTable.show();
    mLeaderboardTableTitle1Label->show();
    mLeaderboardTableTitle2Label->show();
    mLeaderboardTableTitle3Label->show();
    mLeaderboardTableTitle4Label->show();

    for(size_t q = 0; q < finishBoard.size(); ++q)
    {
        if(finishBoard[q].mIsPlayer)
        {
            //mLeaderboardTable.setText(playerName, q);
            mLeaderboardTable2Label[q]->setCaption(playerName);
        }
        else
        {
            //mLeaderboardTable.setText(STRPowerslide::getCharacterTitle(finishBoard[q].mCharName), q);
            mLeaderboardTable2Label[q]->setCaption(STRPowerslide::getCharacterTitle(finishBoard[q].mCharName));
        }

        mLeaderboardTable4Label[q]->setCaption(Conversions::DMToString(finishBoard[q].mPos));

        mLeaderboardTable1Label[q]->show();
        mLeaderboardTable2Label[q]->show();

        std::string iconName = strPowerslide.getValue(finishBoard[q].mCharName + " parameters", "icon", "car0_0s.bmp");
        std::string matName = "Test/" + iconName;
        mLeaderboardTable3Icon[q]->setMaterialName(matName);
        mLeaderboardTable3Icon[q]->show();

        mLeaderboardTable4Label[q]->show();
    }
}

void UIMainMenuLabels::showChampionshipResultsLabels()
{
    for(size_t q = 0; q < mChampionshipResultsLabelsAmountMax; ++q)
    {
        mChampionshipResultsLabel[q]->show();
    }
}

void UIMainMenuLabels::showMultiIPLabels()
{
    mModeMultiIP->show();
    mModeMultiListOfRooms->show();
    mModeMultiUserName->show();
    mModeMultiRoomName->show();
    mModeMultiCreateRoom->show();
}

void UIMainMenuLabels::showMultiJoin()
{
    mModeMultiJoinRoom->show();
}

void UIMainMenuLabels::hideMultiJoin()
{
    mModeMultiJoinRoom->hide();
}

void UIMainMenuLabels::hideAllLabels()
{
    mWindowTitleTrophies->hide();
    mModeMultiIP->hide();
    mModeMultiUserName->hide();
    mModeMultiRoomName->hide();

    mSingleTrackDescription->hide();
    mSingleTrackBestTime->hide();
    mSingleTrackViewBy->hide();
    mSingleTrackViewBySelection->hide();

    mSingleBioDescription->hide();
    mSingleBioViewBy->hide();
    mSingleBioViewBySelection->hide();

    mCarParam_Mass->hide();
    mCarParam_Power->hide();
    mCarParam_Acceleration->hide();
    mCarParam_Traction->hide();
    mCarParam_TopSpeed->hide();
    mCarParam_AeroDynamics->hide();
    mCarParam_Stability->hide();

    mOptionGraphicsLabel_Vendor->hide();
    mOptionGraphicsLabel_Vendor_Val->hide();
    mOptionGraphicsLabel_Renderer->hide();
    mOptionGraphicsLabel_Renderer_Val->hide();
    mOptionGraphicsLabel_Shadow->hide();
    mOptionGraphicsLabel_VSync->hide();
    mOptionGraphicsLabel_Fulscreen->hide();
    mOptionGraphicsLabel_Resolution->hide();
    mOptionGraphicsLabel_Resolution_Val->hide();
    mOptionGraphicsLabel_Resolution_Apply->hide();
    mOptionInputLabel_Type->hide();
    mOptionSoundLabel_VolumeFX->hide();
    mOptionSoundLabel_VolumeFX_Val->hide();
    mOptionSoundLabel_VolumeMusic->hide();
    mOptionSoundLabel_VolumeMusic_Val->hide();
    mOptionRaceLabel_Opponents->hide();
    mOptionRaceLabel_Opponents_Val->hide();
    mOptionRaceLabel_Transmission->hide();
    mOptionRaceLabel_Transmission_Val->hide();
    mOptionRaceLabel_KMPH->hide();
    mOptionRaceLabel_KMPH_Val->hide();
    mOptionRaceLabel_Mirror->hide();
    mOptionRaceLabel_Ghost->hide();
    mOptionHighScoresButtonLabel->hide();
    mOptionHighScoresTrackLabel->hide();
    mOptionHighScoresReplayLabel->hide();
    mOptionHighScoresReplayIcon->hide();
    mOptionHighScoresEmergentGunLabel->hide();
    mOptionNameLabel->hide();
    mOptionNameLabel_Save->hide();
    mOptionVersionLabel->hide();

    mStartingGridTimeLabel->hide();

    mExitLabel->hide();

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        mRaceGridCharactersLabel[q]->hide();
    }

    for(size_t q = 0; q < mPodiumCharacters; ++q)
    {
        mPodiumCharactersFirstSecondThirdLabel[q]->hide();
    }

    mPodiumTableTitle1Label->hide();
    mPodiumTableTitle2Label->hide();
    mPodiumTableTitle3Label->hide();
    mPodiumTableTitle4Label->hide();

    mLeaderboardTableTitle1Label->hide();
    mLeaderboardTableTitle2Label->hide();
    mLeaderboardTableTitle3Label->hide();
    mLeaderboardTableTitle4Label->hide();

    for(size_t q = 0; q < mChampionshipResultsLabelsAmountMax; ++q)
    {
        mChampionshipResultsLabel[q]->hide();
    }

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        mOptionHighScoresTable1Label[q]->hide();
        mOptionHighScoresTable2Label[q]->hide();
        mOptionHighScoresTable3Label[q]->hide();
        mOptionHighScoresTable4Icon[q]->hide();

        mPodiumTable1Label[q]->hide();
        mPodiumTable2Label[q]->hide();
        mPodiumTable3Label[q]->hide();
        mPodiumTable4Label[q]->hide();

        mLeaderboardTable1Label[q]->hide();
        mLeaderboardTable2Label[q]->hide();
        mLeaderboardTable3Icon[q]->hide();
        mLeaderboardTable4Label[q]->hide();
    }

    mUIButtonsManager.hide();
    mUIButtonTicksManager.hide();
    mUILabelsManager.hide();
}

void UIMainMenuLabels::setCurrentTrackBestTime()
{
    setTrackBestTime(mRemapTrack[mModeContext.getGameState().getTrackNameAsOriginal()]);
}

void UIMainMenuLabels::setTrackBestTime(size_t index)
{
    const STRHiscores& strHiscores = mModeContext.getGameState().getSTRHiscores();

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    std::string curTrack = availTracks[index];
    if(index == 8)//stunt
        curTrack = availTracks[index + 1];

    float time;
    std::vector<std::string> times = strHiscores.getArrayValue(curTrack + " parameters", "lap times");

    Conversions::DMFromString(times[0], time);

    if(index == 8)//stunt
        mSingleTrackBestTime->setCaption("");
    else
        mSingleTrackBestTime->setCaption("Best Lap Time " + Tools::SecondsToString(time));
}

Ogre::Real UIMainMenuLabels::getTextWidth(const std::string& text, Ogre::TextAreaOverlayElement * element) const
{

    unsigned int lastSpace = 0;
    Ogre::Real lineWidth = 0.0f;

    Ogre::Font* font = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(element->getFontName()).getPointer();

    for(size_t q = 0; q < text.size(); ++q)
    {
        if (text[q] == ' ')
        {
            if (element->getSpaceWidth() != 0) lineWidth += element->getSpaceWidth();
            else lineWidth += font->getGlyphAspectRatio(' ') * element->getCharHeight();
        }
        else
        {
            // use glyph information to calculate line width
            lineWidth += font->getGlyphAspectRatio(text[q]) * element->getCharHeight();
        }
    }

    return lineWidth;
}

void UIMainMenuLabels::setCurrentTrackDescription()
{
    setTrackDescription(mRemapTrack[mModeContext.getGameState().getTrackNameAsOriginal()]);
}

void UIMainMenuLabels::setTrackDescription(size_t index)
{

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
    std::string desc = mTrackDesc[availTracks[index]];
    if(!desc.empty())
    {

        size_t surfPos = desc.find("Track surface");
        if(surfPos != std::string::npos)
        {
            desc[surfPos - 1] = '\n';
        }

        surfPos = desc.find("Length:");
        if(surfPos != std::string::npos)
        {
            desc[surfPos - 1] = '\n';
            desc.insert(surfPos, "\n");
        }

        //word wrap

        Ogre::Font* font = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(mSingleTrackDescription->getFontName()).getPointer();

        bool firstWord = true;
        unsigned int lastSpace = 0;
        Ogre::Real lineWidth = 0.0f;
        Ogre::Real rightBoundary = viewportWidth / 2.0f - mSingleTrackDescription->getLeft() - viewportWidth / 50.0f;

        for(size_t q = 0; q < desc.size(); ++q)
        {
            if (desc[q] == ' ')
            {
                if (mSingleTrackDescription->getSpaceWidth() != 0) lineWidth += mSingleTrackDescription->getSpaceWidth();
                else lineWidth += font->getGlyphAspectRatio(' ') * mSingleTrackDescription->getCharHeight();

                firstWord = false;
                lastSpace = q;
            }
            else if (desc[q] == '\n')
            {
                firstWord = true;
                lineWidth = 0;
            }
            else
            {
                // use glyph information to calculate line width
                lineWidth += font->getGlyphAspectRatio(desc[q]) * mSingleTrackDescription->getCharHeight();
                if (lineWidth > rightBoundary)
                {
                    if (firstWord)
                    {
                        desc.insert(q, "\n");
                        q = q - 1;
                    }
                    else
                    {
                        desc[lastSpace] = '\n';
                        q = lastSpace - 1;
                    }
                }
            }
        }

        mSingleTrackDescription->setCaption(desc);
    }
    else
    {
        mSingleTrackDescription->setCaption("N/A");
    }

}

void UIMainMenuLabels::setCurrentBioDescription()
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
    std::vector<std::string>::iterator i = std::find(availableCharacters.begin(), availableCharacters.end(), mModeContext.getGameState().getPlayerCar().getCharacterName());
    setBioDescription(i - availableCharacters.begin());
}

void UIMainMenuLabels::setBioDescription(size_t index)
{

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
    std::string desc = mCharacterDesc[availableCharacters[index]];
    if(!desc.empty())
    {
        //word wrap

        Ogre::Font* font = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(mSingleBioDescription->getFontName()).getPointer();

        bool firstWord = true;
        unsigned int lastSpace = 0;
        Ogre::Real lineWidth = 0.0f;
        Ogre::Real rightBoundary = viewportWidth / 2.0f - mSingleBioDescription->getLeft() - viewportWidth / 50.0f;

        for(size_t q = 0; q < desc.size(); ++q)
        {
            if (desc[q] == ' ')
            {
                if (mSingleBioDescription->getSpaceWidth() != 0) lineWidth += mSingleBioDescription->getSpaceWidth();
                else lineWidth += font->getGlyphAspectRatio(' ') * mSingleBioDescription->getCharHeight();

                firstWord = false;
                lastSpace = q;
            }
            else if (desc[q] == '\n')
            {
                firstWord = true;
                lineWidth = 0;
            }
            else
            {
                // use glyph information to calculate line width
                lineWidth += font->getGlyphAspectRatio(desc[q]) * mSingleBioDescription->getCharHeight();
                if (lineWidth > rightBoundary)
                {
                    if (firstWord)
                    {
                        desc.insert(q, "\n");
                        q = q - 1;
                    }
                    else
                    {
                        desc[lastSpace] = '\n';
                        q = lastSpace - 1;
                    }
                }
            }
        }

        std::replace( desc.begin(), desc.end(), (char)(0xe9), 'y');
        mSingleBioDescription->setCaption(desc);
    }
    else
    {
        mSingleBioDescription->setCaption("N/A");
    }

}

bool UIMainMenuLabels::checkCursorOverLabel(const Ogre::Vector2& pos, Ogre::TextAreaOverlayElement * label)
{
    bool ret = false;
    if(label->isVisible() && OgreBites::Widget::isCursorOver(label, pos, 0))
    {
        ret = true;
        label->setColour(Ogre::ColourValue::White);
    }
    else
    {
        label->setColour(UILabel::mInactiveLabel);
    }

    return ret;
}

void UIMainMenuLabels::fillHighScoreTable()
{
    const STRHiscores& strHiscores = mModeContext.getGameState().getSTRHiscores();

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    std::string curTrack = availTracks[mHighScoreTrackIndex];
    if(mHighScoreTrackIndex == 8)//skip stunt
        curTrack = availTracks[mHighScoreTrackIndex + 1];

    std::vector<std::string> names = strHiscores.getArrayValue(curTrack + " parameters", "names");
    std::vector<std::string> times = strHiscores.getArrayValue(curTrack + " parameters", "lap times");
    std::vector<std::string> characters = strHiscores.getArrayValue(curTrack + " parameters", "characters");
    std::vector<std::string> dimCoeff = strHiscores.getArrayValue(curTrack + " parameters", "4th dimension coefficient");

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        float time;
        Conversions::DMFromString(times[q], time);

        if(dimCoeff[q] == "0")
            mOptionHighScoresTable2Label[q]->setCaption(STRPowerslide::getCharacterTitle(names[q]));
        else
            mOptionHighScoresTable2Label[q]->setCaption(names[q]);
        mOptionHighScoresTable3Label[q]->setCaption(Tools::SecondsToString(time));

        std::string iconName = strPowerslide.getValue(characters[q] + " parameters", "icon", "car0_0s.bmp");
        std::string matName = "Test/" + iconName;
        mOptionHighScoresTable4Icon[q]->setMaterialName(matName);
    }

    const STRRacetimes& strRacetimes = mModeContext.getGameState().getSTRRacetimes();
    float emergentGun = strRacetimes.getFloatValue("emergent gun times", curTrack);
    if(emergentGun != 0.0f)
        mOptionHighScoresEmergentGunLabel->setCaption("Ratbag`s Hot Time: " + Tools::SecondsToString(emergentGun));
    else
        mOptionHighScoresEmergentGunLabel->setCaption("");

}

void UIMainMenuLabels::fillReplayStat()
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    std::string curTrack = availTracks[mHighScoreTrackIndex];
    if(mHighScoreTrackIndex == 8)//skip stunt
        curTrack = availTracks[mHighScoreTrackIndex + 1];

    SectionsFile sectionFile;
    sectionFile.load(mModeContext.getGameState().getDataDir(), TrialGhost::mFileName);

    if(sectionFile.isSectionExists(curTrack))
    {
        mOptionHighScoresReplayLabel->setCaption("Time Trial Replay Available: " + Tools::SecondsToString(sectionFile.getSectionTime(curTrack)));
        mOptionHighScoresReplayLabel->show();

        std::string iconName = strPowerslide.getValue(sectionFile.getSectionCharacter(curTrack) + " parameters", "icon", "car0_0s.bmp");
        std::string matName = "Test/" + iconName;
        mOptionHighScoresReplayIcon->setMaterialName(matName);
        mOptionHighScoresReplayIcon->show();
    }
    else
    {
        mOptionHighScoresReplayLabel->hide();
        mOptionHighScoresReplayIcon->hide();
    }
}

std::string UIMainMenuLabels::getTextFileContent(const std::string& dir, const std::string& filename) const
{
    std::string ret = "";

    const PFLoader& patchLoader = mModeContext.getGameState().getPFLoaderPatch();
    Ogre::DataStreamPtr fileToLoad = patchLoader.getFile(dir, filename);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        size_t fileSize = patchLoader.getFileSize(dir, filename);
        std::vector<char> fileBuffer(fileSize);
        fileToLoad->read(&fileBuffer[0], fileSize);

        ret = std::string(fileBuffer.begin(), fileBuffer.end());
    }

    return ret;
}