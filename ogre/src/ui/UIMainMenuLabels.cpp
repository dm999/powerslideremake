
#include "UIMainMenuLabels.h"

#include "../tools/Conversions.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../gamelogic/TrialGhost.h"
#include "../tools/SectionsFile.h"
#include "../tools/OgreTools.h"

#include "../customs/CustomTrayManager.h"

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

#include "routines/UIMainMenuCreateLabelsMain.h"
#include "routines/UIMainMenuCreateLabelsOptionsGraphics.h"
#include "routines/UIMainMenuCreateLabelsOptionsInput.h"
#include "routines/UIMainMenuCreateLabelsOptionsSound.h"
#include "routines/UIMainMenuCreateLabelsOptionsRace.h"
#include "routines/UIMainMenuCreateLabelsOptionsHiscores.h"
#include "routines/UIMainMenuCreateLabelsOptionsCreditsAbout.h"
#include "routines/UIMainMenuCreateLabelsOther.h"


UIMainMenuLabels::UIMainMenuLabels(const ModeContext& modeContext, const GameMode gameMode) :
    UIMainMenuBackground(modeContext, gameMode),
    mIsViewByDescription(false),
    mIsBioByDescription(false),
    mHighScoreTrackIndex(0)
#if !defined(__ANDROID__)
    ,mIsInKeyInsertMode(false)
#endif
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
            mOptionGraphicsLabel_Resolution_Val->getTextArea()->setCaption(videoMode.currentValue);
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
            mOptionGraphicsLabel_Resolution_Val->getTextArea()->setCaption(videoMode.currentValue);
        }

        mModeContext.getGameState().savePlayerData();

        mModeContext.getGameModeSwitcher()->recreateMenu();
    }

    if (button == mGammaValLeft)
    {
        Ogre::Real gamma = mModeContext.getGameState().getGamma();
        if (gamma > 0.5f)
        {
            gamma -= 0.1f;
            mOptionGraphicsLabel_Gamma_Val->getTextArea()->setCaption(Conversions::DMToString(gamma, 3, 1));
            mModeContext.getGameState().setGamma(gamma);
            mModeContext.getGameState().savePlayerData();
        }
    }

    if (button == mGammaValRight)
    {
        Ogre::Real gamma = mModeContext.getGameState().getGamma();
        if (gamma < 3.0f)
        {
            gamma += 0.1f;
            mOptionGraphicsLabel_Gamma_Val->getTextArea()->setCaption(Conversions::DMToString(gamma, 3, 1));
            mModeContext.getGameState().setGamma(gamma);
            mModeContext.getGameState().savePlayerData();
        }
    }

    if(button == mInputTypeValLeft || button == mInputTypeValRight)
    {
#if !defined(__ANDROID__)
        InputType input = mModeContext.getGameState().getInputType();

        if(input == itKeyboard)
        {
            mModeContext.getGameState().setInputType(itMouse);
            mOptionInputLabel_Type->getTextArea()->setCaption("Mouse");
        }
        if(input == itMouse)
        {
            mModeContext.getGameState().setInputType(itKeyboard);
            mOptionInputLabel_Type->getTextArea()->setCaption("Keyboard");
        }

        setKeyText();
        mModeContext.getGameState().savePlayerData();
#endif
    }

    if(button == mSoundVolumeFXValLeft)
    {
        size_t gain = mModeContext.getGameState().getSoundsGain() * 9.0f;
        if(gain > 0)
            --gain;
        mModeContext.getGameState().setSoundsGain(gain / 9.0f);
        mOptionSoundLabel_VolumeFX_Val->getTextArea()->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getSoundsGain() * 9.0f)));

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
        mOptionSoundLabel_VolumeFX_Val->getTextArea()->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getSoundsGain() * 9.0f)));

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
        mOptionSoundLabel_VolumeMusic_Val->getTextArea()->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getMusicGain() * 9.0f)));

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
        mOptionSoundLabel_VolumeMusic_Val->getTextArea()->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getMusicGain() * 9.0f)));

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
            mOptionRaceLabel_Opponents_Val->getTextArea()->setCaption(Conversions::DMToString(mModeContext.getGameState().getAICountInRace()));
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
            mOptionRaceLabel_Opponents_Val->getTextArea()->setCaption(Conversions::DMToString(mModeContext.getGameState().getAICountInRace()));
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

    if(button == mAdvLightingPlayerVal)
    {
        if(mAdvLightingPlayerVal->getChecked())
        {
            mModeContext.getGameState().setAdvancedLightingPlayer(true);
            mAttenuationPlayerVal->setActive(true);
        }
        else
        {
            mModeContext.getGameState().setAdvancedLightingPlayer(false);
            mAttenuationPlayerVal->setActive(false);
        }

        mModeContext.getGameState().savePlayerData();
    }

    if (button == mAttenuationPlayerVal)
    {
        if (mAttenuationPlayerVal->getChecked())
        {
            mModeContext.getGameState().setAttenuationPlayer(true);
        }
        else
        {
            mModeContext.getGameState().setAttenuationPlayer(false);
        }

        mModeContext.getGameState().savePlayerData();
    }

    if(button == mAdvLightingAIVal)
    {
        if(mAdvLightingAIVal->getChecked())
        {
            mModeContext.getGameState().setAdvancedLightingAI(true);
        }
        else
        {
            mModeContext.getGameState().setAdvancedLightingAI(false);
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

        mOptionHighScoresTrackLabel->getTextArea()->setCaption(strPowerslide.getTrackTitle(availTrack));

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

        mOptionHighScoresTrackLabel->getTextArea()->setCaption(strPowerslide.getTrackTitle(availTrack));

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

            for(size_t qq = 0; qq < 7; ++qq)
            {
                if(qq < availChars.size())
                    mCharactersLabels[qq]->getTextArea()->setCaption(STRPowerslide::getCharacterTitle(availChars[qq]));
                else
                    mCharactersLabels[qq]->getTextArea()->setCaption("");
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

#if !defined(__ANDROID__)
    if(!mIsInKeyInsertMode)
    for (size_t q = 0; q < InputKeyMapping::kmSticky; ++q)
    {
        if (label == mOptionInputLabels_Keys[q])
        {
            mSelectedKey = static_cast<InputKeyMapping>(q);
            mIsInKeyInsertMode = true;
            mSelectedKeyLabel = label;
            mSelectedKeyLabel->setFixed(true);
        }
    }

    if (label == mOptionInputLabel_ResetDefaults)
    {
        mModeContext.getGameState().setDefaultKeyCodeMappers();
        setKeyText();
        mModeContext.getGameState().savePlayerData();
    }
#endif


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

    if(label == mOptionGraphicsLabel_Resolution_Val)
    {
            Ogre::RenderSystem * rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
            Ogre::ConfigOptionMap configOpts = rs->getConfigOptions();
            Ogre::ConfigOption videoMode = configOpts["Video Mode"];
            size_t curVal = 0;
            for(size_t q = 0; q < videoMode.possibleValues.size(); ++q)
            {
                if(mOptionGraphicsLabel_Resolution_Val->getTextArea()->getCaption() == videoMode.possibleValues[q])
                {
                    curVal = q;
                    break;
                }
            }
            ++curVal;
            if(curVal >= videoMode.possibleValues.size()) curVal = 0;
            mOptionGraphicsLabel_Resolution_Val->getTextArea()->setCaption(videoMode.possibleValues[curVal]);

            Ogre::Real viewBySize = getTextWidth(mOptionGraphicsLabel_Resolution_Val->getTextArea()->getCaption(), mOptionGraphicsLabel_Resolution_Val->getTextArea());
            mOptionGraphicsLabel_Resolution_Apply->getTextArea()->setLeft(mOptionGraphicsLabel_Resolution_Val->getTextArea()->getLeft() + viewBySize + 9.0f);
    }

    if(label == mOptionGraphicsLabel_Resolution_Apply)
    {
        Ogre::RenderSystem * rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
        Ogre::ConfigOptionMap& configOpts = rs->getConfigOptions();
        Ogre::ConfigOption& videoMode = configOpts["Video Mode"];
        unsigned int desiredWidth, desiredHeight;
        std::string desiredRes = mOptionGraphicsLabel_Resolution_Val->getTextArea()->getCaption();
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

    if(label == mOptionRaceLabel_Transmission_Val)
    {
        if(mModeContext.getGameState().getTransmissionType() == trAuto)
        {
            mModeContext.getGameState().setTransmissionType(trManual);
            mOptionRaceLabel_Transmission_Val->getTextArea()->setCaption("Manual");
        }
        else
        {
            mModeContext.getGameState().setTransmissionType(trAuto);
            mOptionRaceLabel_Transmission_Val->getTextArea()->setCaption("Auto");
        }

        mModeContext.getGameState().savePlayerData();
    }

    if(label == mOptionRaceLabel_KMPH_Val)
    {
        if(mModeContext.getGameState().isKMPh())
        {
            mModeContext.getGameState().setKMPh(false);
            mOptionRaceLabel_KMPH_Val->getTextArea()->setCaption("Mph");
        }
        else
        {
            mModeContext.getGameState().setKMPh(true);
            mOptionRaceLabel_KMPH_Val->getTextArea()->setCaption("Kph");
        }

        mModeContext.getGameState().savePlayerData();
    }

    if(label == mSingleTrackViewBySelection)
    {
        mIsViewByDescription = !mIsViewByDescription;

        if(mIsViewByDescription)
        {
            mSingleTrackViewBySelection->getTextArea()->setCaption("Description");
            hideBackgroundTrack();
            mSingleTrackDescription->show();
        }
        else
        {
            mSingleTrackViewBySelection->getTextArea()->setCaption("Image");
            showBackgroundTrack();
            mSingleTrackDescription->hide();
        }
    }


    if(label == mSingleBioViewBySelection)
    {
        mIsBioByDescription = !mIsBioByDescription;

        if(mIsBioByDescription)
        {
            mSingleBioViewBySelection->getTextArea()->setCaption("Description");
            hideBackgroundCharacter();
            mSingleBioDescription->show();
        }
        else
        {
            mSingleBioViewBySelection->getTextArea()->setCaption("Bio");
            showBackgroundCharacter();
            mSingleBioDescription->hide();
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
    createLabelsOptionsMain(screenAdaptionRelative);
    createLabelsOptionsGraphics(screenAdaptionRelative);
    createLabelsOptionsInput(screenAdaptionRelative);
    createLabelsOptionsSound(screenAdaptionRelative);
    createLabelsOptionsRace(screenAdaptionRelative);
    createLabelsOptionsHiscores(screenAdaptionRelative);
    createLabelsOptionsCreditsAbout(screenAdaptionRelative);
    createLabelsOptionsOther(screenAdaptionRelative);

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

void UIMainMenuLabels::keyUp(MyGUI::KeyCode _key, wchar_t _char)
{
#if !defined(__ANDROID__)
    if (mIsInKeyInsertMode)
    {
        mSelectedKeyLabel->setFixed(false);
        mIsInKeyInsertMode = false;

        std::string code = keyCodeToString(static_cast<OIS::KeyCode>(_key.getValue()));
        if (!code.empty())
        {
            mSelectedKeyLabel->getTextArea()->setCaption(code);
            mModeContext.getGameState().setKeyCode(static_cast<OIS::KeyCode>(_key.getValue()), mSelectedKey);
            mModeContext.getGameState().setKeyCode(static_cast<OIS::MouseButtonID>(-1), mSelectedKey);
            mModeContext.getGameState().savePlayerData();
        }
    }
#endif
}

void UIMainMenuLabels::mousePressed(const Ogre::Vector2& pos)
{
    UIBaseMenu::mousePressed(pos);

    mUIButtonsManager.mousePressed(pos);
    mUIButtonTicksManager.mousePressed(pos);
    mUILabelsManager.mousePressed(pos);
}

void UIMainMenuLabels::mouseReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id)
{
#if !defined(__ANDROID__)
    bool prevIsInKeyInsertMode = mIsInKeyInsertMode;
#endif
    UIMainMenuBackground::mouseReleased(pos, id);

    mUIButtonsManager.mouseReleased(pos);
    mUIButtonTicksManager.mouseReleased(pos);
    mUILabelsManager.mouseReleased(pos);

#if !defined(__ANDROID__)
    if (mIsInKeyInsertMode && prevIsInKeyInsertMode == true && mModeContext.getGameState().getInputType() == itMouse)
    {
        mSelectedKeyLabel->setFixed(false);
        mIsInKeyInsertMode = false;

        std::string code = keyCodeToString(id);
        if (!code.empty())
        {
            mSelectedKeyLabel->getTextArea()->setCaption(code);
            mModeContext.getGameState().setKeyCode(OIS::KC_UNASSIGNED, mSelectedKey);
            mModeContext.getGameState().setKeyCode(id, mSelectedKey);
            mModeContext.getGameState().savePlayerData();
        }
    }
#endif
}

void UIMainMenuLabels::mouseMoved(const Ogre::Vector2& pos)
{
    UIMainMenuBackground::mouseMoved(pos);

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
    mUILabelsManager.show("mModeSingle");
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
        mSingleTrackViewBySelection->getTextArea()->setCaption("Description");
        hideBackgroundTrack();
        mSingleTrackDescription->show();
    }
    else
    {
        mSingleTrackViewBySelection->getTextArea()->setCaption("Image");
        showBackgroundTrack();
        mSingleTrackDescription->hide();
    }

    mSingleTrackBestTime->show();

    if(mModeContext.getGameState().isPatchDataInited())
    {
        mUILabelsManager.show("mSingleTrackView");
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

    mUILabelsManager.show("mCarParam");
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
        mSingleBioViewBySelection->getTextArea()->setCaption("Description");
    }
    else
    {
        showBackgroundCharacter();
        mSingleBioDescription->hide();
        mSingleBioViewBySelection->getTextArea()->setCaption("Bio");
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
        mUILabelsManager.show("mSingleBio");
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
    Ogre::RenderSystem * rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
    Ogre::ConfigOptionMap configOpts = rs->getConfigOptions();
    Ogre::ConfigOption videoMode = configOpts["Video Mode"];
    mOptionGraphicsLabel_Resolution_Val->getTextArea()->setCaption(videoMode.currentValue);
    mUILabelsManager.show("mOptionGraphicsLabel");
    mUIButtonsManager.show("mGraphics");
    mUIButtonTicksManager.show("mGraphics");
}

void UIMainMenuLabels::showOptionInputLabels()
{
    mUILabelsManager.show("mInputType");
    mUIButtonsManager.show("mInputType");
}

void UIMainMenuLabels::showOptionSoundLabels()
{
    mOptionSoundLabel_VolumeFX_Val->getTextArea()->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getSoundsGain() * 9.0f)));
    mOptionSoundLabel_VolumeMusic_Val->getTextArea()->setCaption(Conversions::DMToString(static_cast<size_t>(mModeContext.getGameState().getMusicGain() * 9.0f)));

    mUIButtonsManager.show("mSoundVolume");
    mUILabelsManager.show("mSoundVolume");
}

void UIMainMenuLabels::showOptionRaceLabels()
{
    mUIButtonsManager.show("mOptionRace");
    mUIButtonTicksManager.show("mOptionRace");
    mUILabelsManager.show("mOptionRace");
}

void UIMainMenuLabels::showOptionHiscoreLabels()
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    size_t highScoreTrackIndex = mHighScoreTrackIndex;
    if(highScoreTrackIndex == 8)//skip stunt
        ++highScoreTrackIndex;
    mOptionHighScoresTrackLabel->getTextArea()->setCaption(strPowerslide.getTrackTitle(availTracks[highScoreTrackIndex]));


    fillHighScoreTable();
    fillReplayStat();

    mUIButtonsManager.show("mHighScores");
    mUILabelsManager.show("mHighScores");

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        mOptionHighScoresTable4Icon[q]->show();
    }
}

void UIMainMenuLabels::showExitLabels(const std::string& title)
{
    mExitLabel->getTextArea()->setCaption(title);
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

    mOptionHighScoresReplayIcon->hide();
    mOptionVersionLabel->hide();

    mStartingGridTimeLabel->hide();

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
        mSingleTrackBestTime->getTextArea()->setCaption("");
    else
        mSingleTrackBestTime->getTextArea()->setCaption("Best Lap Time " + Tools::SecondsToString(time));
}

Ogre::Real UIMainMenuLabels::getTextWidth(const std::string& text, Ogre::TextAreaOverlayElement * element) const
{

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

void UIMainMenuLabels::wrapText(std::string& text, Ogre::Real rightBoundary, const Ogre::Font* font, Ogre::Real spaceWidth, Ogre::Real characterHeight)
{
    //word wrap

    bool firstWord = true;
    unsigned int lastSpace = 0;
    Ogre::Real lineWidth = 0.0f;

    for(size_t q = 0; q < text.size(); ++q)
    {
        if (text[q] == ' ')
        {
            if (spaceWidth != 0) lineWidth += spaceWidth;
            else lineWidth += font->getGlyphAspectRatio(' ') * characterHeight;

            firstWord = false;
            lastSpace = q;
        }
        else if (text[q] == '\n')
        {
            firstWord = true;
            lineWidth = 0;
        }
        else
        {
            // use glyph information to calculate line width
            lineWidth += font->getGlyphAspectRatio(text[q]) * characterHeight;
            if (lineWidth > rightBoundary)
            {
                if (firstWord)
                {
                    text.insert(q, "\n");
                    q = q - 1;
                }
                else
                {
                    text[lastSpace] = '\n';
                    q = lastSpace - 1;
                }
            }
        }
    }
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

        Ogre::Real rightBoundary = viewportWidth / 2.0f - mSingleTrackDescription->getTextArea()->getLeft() - viewportWidth / 50.0f;
        wrapText(
            desc,
            rightBoundary,
            (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(mSingleTrackDescription->getTextArea()->getFontName()).getPointer(),
            mSingleTrackDescription->getTextArea()->getSpaceWidth(),
            mSingleTrackDescription->getTextArea()->getCharHeight());

        mSingleTrackDescription->getTextArea()->setCaption(desc);
    }
    else
    {
        mSingleTrackDescription->getTextArea()->setCaption("N/A");
    }

}

void UIMainMenuLabels::setCurrentBioDescription()
{
    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
    std::vector<std::string>::iterator i = std::find(availableCharacters.begin(), availableCharacters.end(), mModeContext.getGameState().getPlayerCar().getCharacterName());
    setBioDescription(i - availableCharacters.begin());
}

void UIMainMenuLabels::setBioDescription(size_t index)
{

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 

    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
    std::string desc = mCharacterDesc[availableCharacters[index]];
    if(!desc.empty())
    {
        Ogre::Real rightBoundary = viewportWidth / 2.0f - mSingleBioDescription->getTextArea()->getLeft() - viewportWidth / 50.0f;
        wrapText(
            desc,
            rightBoundary,
            (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(mSingleBioDescription->getTextArea()->getFontName()).getPointer(),
            mSingleBioDescription->getTextArea()->getSpaceWidth(),
            mSingleBioDescription->getTextArea()->getCharHeight());

        std::replace( desc.begin(), desc.end(), (char)(0xe9), 'y');
        mSingleBioDescription->getTextArea()->setCaption(desc);
    }
    else
    {
        mSingleBioDescription->getTextArea()->setCaption("N/A");
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
            mOptionHighScoresTable2Label[q]->getTextArea()->setCaption(STRPowerslide::getCharacterTitle(names[q]));
        else
            mOptionHighScoresTable2Label[q]->getTextArea()->setCaption(names[q]);
        mOptionHighScoresTable3Label[q]->getTextArea()->setCaption(Tools::SecondsToString(time));

        std::string iconName = strPowerslide.getValue(characters[q] + " parameters", "icon", "car0_0s.bmp");
        std::string matName = "Test/" + iconName;
        mOptionHighScoresTable4Icon[q]->setMaterialName(matName);
    }

    const STRRacetimes& strRacetimes = mModeContext.getGameState().getSTRRacetimes();
    float emergentGun = strRacetimes.getFloatValue("emergent gun times", curTrack);
    if(emergentGun != 0.0f)
        mOptionHighScoresEmergentGunLabel->getTextArea()->setCaption("Ratbag`s Hot Time: " + Tools::SecondsToString(emergentGun));
    else
        mOptionHighScoresEmergentGunLabel->getTextArea()->setCaption("");

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
        mOptionHighScoresReplayLabel->getTextArea()->setCaption("Time Trial Replay Available: " + Tools::SecondsToString(sectionFile.getSectionTime(curTrack)));
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

#if !defined(__ANDROID__)
void UIMainMenuLabels::setKeyText()
{
    for (size_t q = 0; q < InputKeyMapping::kmSticky; ++q)
    {
        OIS::KeyCode code = mModeContext.getGameState().getKeyCode(static_cast<InputKeyMapping>(q));
        if (code != OIS::KC_UNASSIGNED)
        {
            mOptionInputLabels_Keys[q]->getTextArea()->setCaption(keyCodeToString(code));
        }
        else
        {
            OIS::MouseButtonID id = mModeContext.getGameState().getMouseID(static_cast<InputKeyMapping>(q));
            mOptionInputLabels_Keys[q]->getTextArea()->setCaption(keyCodeToString(id));
        }
    }

    if (mModeContext.getGameState().getInputType() == itMouse)
    {
        mOptionInputLabels_Keys[0]->setActive(false);
        mOptionInputLabels_Keys[0]->getTextArea()->setCaption("Move Mouse Left");
        mOptionInputLabels_Keys[1]->setActive(false);
        mOptionInputLabels_Keys[1]->getTextArea()->setCaption("Move Mouse Right");
    }
    else
    {
        mOptionInputLabels_Keys[0]->setActive(true);
        mOptionInputLabels_Keys[1]->setActive(true);
    }
}
#endif