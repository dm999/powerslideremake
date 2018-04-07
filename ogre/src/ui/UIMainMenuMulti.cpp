
#ifndef NO_MULTIPLAYER

#include "UIMainMenuMulti.h"

#include "UIMainMenuLabels.h"
#include "UIRace.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../customs/CustomTrayManager.h"

#include "../loaders/TextureLoader.h"

#include "../gamemodes/MenuMultiMode.h"

#include "../multiplayer/MultiplayerControllerMaster.h"
#include "../multiplayer/MultiplayerControllerSlave.h"

#if defined(__ANDROID__)
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_write(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_write(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 
#endif

namespace{
    const size_t CHATBUTTONS_COUNT = 4;
}//anonimuos namespace

UIMainMenuMulti::UIMainMenuMulti(const ModeContext& modeContext, MenuMultiMode * menuMultiMode)
    : UIBaseMenu(modeContext),
    mAICountBeforeNonAITrack(mModeContext.getGameState().getAICount()),
    mMenuMultiMode(menuMultiMode),
    mEditBoxMessage(50),
    mTrackNameSelected(NULL),
    mLapsCount(NULL),
    mAIStrengthVal(NULL),
    mModeVal(NULL),
    mCarVal(NULL),
    mCharacterVal(NULL)
{
    memset(mMainChatButtons, 0, sizeof(Ogre::PanelOverlayElement*));
}

void UIMainMenuMulti::loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell)
{
    loadCommonTextures(pfLoaderGameshell);

    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "m_chat.bmp", 
                                "OriginalMainBackground", TEMP_RESOURCE_GROUP_NAME);

    //ChatBut0 - empty
    //ChatBut1 - green
    //ChatBut2 - PC
    //ChatBut3 - red
    for(size_t q = 0; q < CHATBUTTONS_COUNT; ++q)
    {
        TextureLoader().loadChroma( pfLoaderGameshell, 
                                    "data/gameshell", "chatbut" + Conversions::DMToString(q) + ".bmp", 
                                    "OriginalChatBut" + Conversions::DMToString(q), TEMP_RESOURCE_GROUP_NAME,
                                    Ogre::ColourValue(1.0f, 0.0f, 0.487f),
                                    0.1f,
                                    false, 64.0f, 
                                    true);
    }

    UIRace::loadDashboardCars(mModeContext.getGameState());
}

void UIMainMenuMulti::createMaterials(CustomTrayManager* trayMgr)
{

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    Ogre::Matrix4 screenAdaptionRelative(
        viewportWidth / 640.0f, 0.0f,                       0.0f,                   0.0f,
        0.0f,                   viewportHeight / 480.0f,    0.0f,                   0.0f,
        0.0f,                   0.0f,                       viewportWidth / 640.0f, 0.0f,
        0.0f,                   0.0f,                       0.0f,                   viewportHeight / 480.0f);

    //main background
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalMainBackground");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/MainBackground", 
                            "Test/Diffuse", 
                            texName, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
    }

    //chatbut
    for(size_t q = 0; q < CHATBUTTONS_COUNT; ++q)
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalChatBut" + Conversions::DMToString(q));
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/OriginalChatBut" + Conversions::DMToString(q), 
                            "Test/DiffuseTransparent", 
                            texName, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
    }


    {
        mMainBackground = createPanel("MainBackground", viewportWidth, viewportHeight, 0.0f, 0.0f, "Test/MainBackground");
        mMainBackground->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mMainBackground);
        //mMainBackground->hide();
    }

    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
    for (size_t q = 0; q < availableCharacters.size(); ++q)
    {
        std::string iconName = mModeContext.getGameState().getSTRPowerslide().getValue(availableCharacters[q] + " parameters", "icon", "car0_0s.bmp");

        std::vector<Ogre::String> texName;
        texName.push_back("chroma_" + iconName);
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/" + iconName, 
                            "Test/DiffuseTransparent", 
                            texName, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setSceneBlending (Ogre::SBT_TRANSPARENT_ALPHA);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);

        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
        state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
        state->setTextureScale(-1.0f, 1.0f);
    }
}

void UIMainMenuMulti::load(CustomTrayManager* trayMgr, const GameState& gameState, bool isEnterFromBaseMenu, const std::map<std::string, std::string>& playersSkins)
{


    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    Ogre::Matrix4 screenAdaptionRelative(
        viewportWidth / 640.0f, 0.0f,                       0.0f,                   0.0f,
        0.0f,                   viewportHeight / 480.0f,    0.0f,                   0.0f,
        0.0f,                   0.0f,                       viewportWidth / 640.0f, 0.0f,
        0.0f,                   0.0f,                       0.0f,                   viewportHeight / 480.0f);

    loadMisc(gameState.getPFLoaderData(), gameState.getPFLoaderGameshell());

    createCommonMaterials();
    createMaterials(trayMgr);

    createControls(screenAdaptionRelative, mMainBackground);
    for(size_t q = 0; q < mControlsCount - 1; ++q)
        setControlActive(q, false);

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {

        //chatbuttons
        {
            const Ogre::Real left = 15.0f;
            const Ogre::Real top = 106.0f;
            Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(left, 22.0f * q + top, left + 15.0f, 22.0f * q + top + 15.0f);
            mMainChatButtons[q] = createPanel("ChatBut_" + Conversions::DMToString(q), pos, "Test/OriginalChatBut0");
            mMainChatButtons[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mMainChatButtons[q]);

            if(q >= GameState::mRaceGridCarsMax - mModeContext.getGameState().getAICount())
            {
                mMainChatButtons[q]->setMaterialName("Test/OriginalChatBut2");
            }
        }

        //names
        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(40.0f, 22.0f * q + 106.0f, 0.0f, 0.0f);
            mChatroomPlayers[q] = createTextArea("MainWindowPlayers1_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mChatroomPlayers[q]->setCaption("");
            mChatroomPlayers[q]->setCharHeight(36.0f * viewportHeight / 1024.0f);
            mChatroomPlayers[q]->setSpaceWidth(9.0f);
            mChatroomPlayers[q]->setHeight(36.0f * viewportHeight / 1024.0f);
            mChatroomPlayers[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mChatroomPlayers[q]->setFontName("SdkTrays/Caption");
            mChatroomPlayers[q]->setColour(Ogre::ColourValue::White);
            mMainBackground->addChild(mChatroomPlayers[q]);
        }

        //icons
        {
            const Ogre::Real left = 126.0f;
            const Ogre::Real top = 101.0f;
            Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(left, 22.0f * q + top, left + 62.0f, 22.0f * q + top + 22.0f);
            mMainCarIcons[q] = createPanel("MainWindowPlayersIcon_" + Conversions::DMToString(q), pos, "Test/car0_0s.bmp");
            mMainCarIcons[q]->setUV(0.0f, 0.0f, -1.0f, 1.0f);
            if(q > 0)
                mMainCarIcons[q]->hide();
            mMainBackground->addChild(mMainCarIcons[q]);
        }
    }

    {
        std::string iconName = mModeContext.getGameState().getSTRPowerslide().getValue(mModeContext.getGameState().getPlayerCar().getCharacterName() + " parameters", "icon", "car0_0s.bmp");
        std::string matName = "Test/" + iconName;
        mMainCarIcons[0]->setMaterialName(matName);
    }

    for(size_t q = 0; q < GameState::mRaceGridCarsMax - 1; ++q)
    {
        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(189.0f, 22.0f * (q + 1) + 112.0f, 0.0f, 0.0f);
            mChatroomPlayersMessages[q] = createTextArea("MainWindowPlayersMessages1_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mChatroomPlayersMessages[q]->setCaption("");
            mChatroomPlayersMessages[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mChatroomPlayersMessages[q]->setSpaceWidth(9.0f);
            mChatroomPlayersMessages[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mChatroomPlayersMessages[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mChatroomPlayersMessages[q]->setFontName("SdkTrays/Caption");
            mChatroomPlayersMessages[q]->setColour(Ogre::ColourValue::White);
            mMainBackground->addChild(mChatroomPlayersMessages[q]);
        }
    }

    {
        mEditBoxMessage.setBackgroundMaterial("Test/CustomBackgroundTransparent");
        mEditBoxMessage.init(screenAdaptionRelative, mMainBackground, Ogre::Vector4(189.0f, 105.0f, 450.0f, 18.0f), 36.0f, true);
        mEditBoxMessage.setText("");
    }

    //Chatroom
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(30.0f, 8.0f, 0.0f, 0.0f);
        mChatroom = createTextArea("MainWindowChatroom", 0.0f, 0.0f, pos.x, pos.y); 
        mChatroom->setCaption("Chatroom");
        mChatroom->setCharHeight(50.0f * viewportHeight / 1024.0f);
        mChatroom->setSpaceWidth(9.0f);
        mChatroom->setHeight(50.0f * viewportHeight / 1024.0f);
        mChatroom->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mChatroom->setFontName("SdkTrays/Caption");
        mChatroom->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mChatroom);
    }

    //Track
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(321.0f, 13.0f, 0.0f, 0.0f);
        mTrackName = createTextArea("MainWindowTrackName", 0.0f, 0.0f, pos.x, pos.y); 
        mTrackName->setCaption("TRACK");
        mTrackName->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mTrackName->setSpaceWidth(9.0f);
        mTrackName->setHeight(20.0f * viewportHeight / 1024.0f);
        mTrackName->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mTrackName->setFontName("SdkTrays/Caption");
        mTrackName->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mTrackName);
    }
    {
        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(321.0f, 23.0f, 0.0f, 0.0f);
        mTrackNameSelected = createTextArea("MainWindowTrackNameSelected", 0.0f, 0.0f, pos.x, pos.y); 
        mTrackNameSelected->setCaption(strPowerslide.getTrackTitle(availTracks[getCurrentTrackIndex()]));
        mTrackNameSelected->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mTrackNameSelected->setSpaceWidth(9.0f);
        mTrackNameSelected->setHeight(16.0f * viewportHeight / 1024.0f);
        mTrackNameSelected->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mTrackNameSelected->setFontName("SdkTrays/Caption");
        mTrackNameSelected->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mTrackNameSelected);
    }
    {
        mTracksValLeft.loadBackground(mModeContext.getGameState().getPFLoaderGameshell(), "OriginalButtonDownMulti");
        mTracksValLeft.init(screenAdaptionRelative, mMainBackground, Ogre::Vector4(320.0f, 33.0f, 12.0f, 12.0f), mModeContext.mGameState.isMultiplayerMaster());
        mTracksValLeft.setButtonOnAction(this);

        mTracksValRight.loadBackground(mModeContext.getGameState().getPFLoaderGameshell(), "OriginalButtonUpMulti");
        mTracksValRight.init(screenAdaptionRelative, mMainBackground, Ogre::Vector4(340.0f, 33.0f, 12.0f, 12.0f), mModeContext.mGameState.isMultiplayerMaster());
        mTracksValRight.setButtonOnAction(this);
    }

    //Laps
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(381.0f, 13.0f, 0.0f, 0.0f);
        mLaps = createTextArea("MainWindowLaps", 0.0f, 0.0f, pos.x, pos.y); 
        mLaps->setCaption("LAPS");
        mLaps->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mLaps->setSpaceWidth(9.0f);
        mLaps->setHeight(20.0f * viewportHeight / 1024.0f);
        mLaps->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mLaps->setFontName("SdkTrays/Caption");
        mLaps->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mLaps);
    }
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(381.0f, 23.0f, 0.0f, 0.0f);
        mLapsCount = createTextArea("MainWindowLapsCount", 0.0f, 0.0f, pos.x, pos.y); 
        mLapsCount->setCaption(getLapsCountString());
        mLapsCount->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mLapsCount->setSpaceWidth(9.0f);
        mLapsCount->setHeight(16.0f * viewportHeight / 1024.0f);
        mLapsCount->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mLapsCount->setFontName("SdkTrays/Caption");
        mLapsCount->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mLapsCount);
    }
    {
        mLapsValLeft.loadBackground(mModeContext.getGameState().getPFLoaderGameshell(), "OriginalButtonDownMulti");
        mLapsValLeft.init(screenAdaptionRelative, mMainBackground, Ogre::Vector4(380.0f, 33.0f, 12.0f, 12.0f), mModeContext.mGameState.isMultiplayerMaster());
        mLapsValLeft.setButtonOnAction(this);

        mLapsValRight.loadBackground(mModeContext.getGameState().getPFLoaderGameshell(), "OriginalButtonUpMulti");
        mLapsValRight.init(screenAdaptionRelative, mMainBackground, Ogre::Vector4(400.0f, 33.0f, 12.0f, 12.0f), mModeContext.mGameState.isMultiplayerMaster());
        mLapsValRight.setButtonOnAction(this);
    }

    //Car
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(441.0f, 13.0f, 0.0f, 0.0f);
        mCar = createTextArea("MainWindowCar", 0.0f, 0.0f, pos.x, pos.y); 
        mCar->setCaption("CAR");
        mCar->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mCar->setSpaceWidth(9.0f);
        mCar->setHeight(20.0f * viewportHeight / 1024.0f);
        mCar->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mCar->setFontName("SdkTrays/Caption");
        mCar->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mCar);
    }
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(441.0f, 23.0f, 0.0f, 0.0f);
        mCarVal = createTextArea("MainWindowCarVal", 0.0f, 0.0f, pos.x, pos.y); 
        mCarVal->setCaption(getCarString());
        mCarVal->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mCarVal->setSpaceWidth(9.0f);
        mCarVal->setHeight(16.0f * viewportHeight / 1024.0f);
        mCarVal->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mCarVal->setFontName("SdkTrays/Caption");
        mCarVal->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mCarVal);
    }
    {
        mCarValLeft.loadBackground(mModeContext.getGameState().getPFLoaderGameshell(), "OriginalButtonDownMulti");
        mCarValLeft.init(screenAdaptionRelative, mMainBackground, Ogre::Vector4(440.0f, 33.0f, 12.0f, 12.0f), true);
        mCarValLeft.setButtonOnAction(this);

        mCarValRight.loadBackground(mModeContext.getGameState().getPFLoaderGameshell(), "OriginalButtonUpMulti");
        mCarValRight.init(screenAdaptionRelative, mMainBackground, Ogre::Vector4(460.0f, 33.0f, 12.0f, 12.0f), true);
        mCarValRight.setButtonOnAction(this);
    }

    //Character
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(501.0f, 13.0f, 0.0f, 0.0f);
        mCharacter = createTextArea("MainWindowCharacter", 0.0f, 0.0f, pos.x, pos.y); 
        mCharacter->setCaption("CHARACTER");
        mCharacter->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mCharacter->setSpaceWidth(9.0f);
        mCharacter->setHeight(20.0f * viewportHeight / 1024.0f);
        mCharacter->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mCharacter->setFontName("SdkTrays/Caption");
        mCharacter->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mCharacter);
    }
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(501.0f, 23.0f, 0.0f, 0.0f);
        mCharacterVal = createTextArea("MainWindowCharacterVal", 0.0f, 0.0f, pos.x, pos.y); 
        mCharacterVal->setCaption(getCharacterString());
        mCharacterVal->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mCharacterVal->setSpaceWidth(9.0f);
        mCharacterVal->setHeight(16.0f * viewportHeight / 1024.0f);
        mCharacterVal->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mCharacterVal->setFontName("SdkTrays/Caption");
        mCharacterVal->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mCharacterVal);
    }
    {
        mCharacterValLeft.loadBackground(mModeContext.getGameState().getPFLoaderGameshell(), "OriginalButtonDownMulti");
        mCharacterValLeft.init(screenAdaptionRelative, mMainBackground, Ogre::Vector4(500.0f, 33.0f, 12.0f, 12.0f), true);
        mCharacterValLeft.setButtonOnAction(this);

        mCharacterValRight.loadBackground(mModeContext.getGameState().getPFLoaderGameshell(), "OriginalButtonUpMulti");
        mCharacterValRight.init(screenAdaptionRelative, mMainBackground, Ogre::Vector4(520.0f, 33.0f, 12.0f, 12.0f), true);
        mCharacterValRight.setButtonOnAction(this);
    }

    //Mode
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(156.0f, 13.0f, 0.0f, 0.0f);
        mMode = createTextArea("MainWindowMode", 0.0f, 0.0f, pos.x, pos.y); 
        mMode->setCaption("MODE");
        mMode->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mMode->setSpaceWidth(9.0f);
        mMode->setHeight(20.0f * viewportHeight / 1024.0f);
        mMode->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mMode->setFontName("SdkTrays/Caption");
        mMode->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mMode);
    }
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(210.0f, 14.0f, 0.0f, 0.0f);
        mModeVal = createTextArea("MainWindowModeVal", 0.0f, 0.0f, pos.x, pos.y); 
        mModeVal->setCaption("Single Race");
        mModeVal->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mModeVal->setSpaceWidth(9.0f);
        mModeVal->setHeight(16.0f * viewportHeight / 1024.0f);
        mModeVal->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeVal->setFontName("SdkTrays/Caption");
        if(mModeContext.mGameState.isMultiplayerMaster())
            mModeVal->setColour(UIMainMenuLabels::mInactiveLabel);
        else
            mModeVal->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mModeVal);
    }

    //AIStrength
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(156.0f, 23.0f, 0.0f, 0.0f);
        mAIStrength = createTextArea("MainWindowAIStrength", 0.0f, 0.0f, pos.x, pos.y); 
        mAIStrength->setCaption("DIFFICULTY");
        mAIStrength->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mAIStrength->setSpaceWidth(9.0f);
        mAIStrength->setHeight(20.0f * viewportHeight / 1024.0f);
        mAIStrength->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mAIStrength->setFontName("SdkTrays/Caption");
        mAIStrength->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mAIStrength);
    }
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(210.0f, 24.0f, 0.0f, 0.0f);
        mAIStrengthVal = createTextArea("MainWindowAIStrengthVal", 0.0f, 0.0f, pos.x, pos.y); 
        mAIStrengthVal->setCaption(getAIStrengthString());
        mAIStrengthVal->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mAIStrengthVal->setSpaceWidth(9.0f);
        mAIStrengthVal->setHeight(16.0f * viewportHeight / 1024.0f);
        mAIStrengthVal->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mAIStrengthVal->setFontName("SdkTrays/Caption");
        if(mModeContext.mGameState.isMultiplayerMaster())
            mAIStrengthVal->setColour(UIMainMenuLabels::mInactiveLabel);
        else
            mAIStrengthVal->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mAIStrengthVal);
    }

    //Weapons
    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(156.0f, 33.0f, 0.0f, 0.0f);
        mWeapons = createTextArea("MainWindowWeapons", 0.0f, 0.0f, pos.x, pos.y); 
        mWeapons->setCaption("WEAPONS");
        mWeapons->setCharHeight(20.0f * viewportHeight / 1024.0f);
        mWeapons->setSpaceWidth(9.0f);
        mWeapons->setHeight(20.0f * viewportHeight / 1024.0f);
        mWeapons->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mWeapons->setFontName("SdkTrays/Caption");
        mWeapons->setColour(Ogre::ColourValue::White);
        mMainBackground->addChild(mWeapons);
    }
    {
        mWeaponsVal.loadBackground(mModeContext.getGameState().getPFLoaderGameshell(), "OriginalButtonTickMulti");
        //mWeaponsVal.init(screenAdaptionRelative, mMainBackground, Ogre::Vector4(210.0f, 34.0f, 12.0f, 12.0f), true, mModeContext.mGameState.isMultiplayerMaster());
        mWeaponsVal.init(screenAdaptionRelative, mMainBackground, Ogre::Vector4(210.0f, 34.0f, 12.0f, 12.0f), true, false);
        mWeaponsVal.setButtonOnAction(this);
    }

    if(!isEnterFromBaseMenu)
    {
        const multislider::RoomInfo& roomInfo = mMenuMultiMode->getMultiplayerController()->getRoomInfo();
        const std::string& player = mMenuMultiMode->getMultiplayerController()->getPlayerName();
        const std::vector<std::string>& players = roomInfo.getPlayers();
        addCurrentPlayer(player);
        for(size_t q = 0, index = 0; q < players.size(); ++q)
        {
            if(players[q] != player)
            {
                addOtherPlayer(index++, players[q], false);
            }

            std::map<std::string, std::string>::const_iterator i = playersSkins.find(players[q]);
            playerSkinChanged(players[q], (*i).second);
        }
    }

    selectTrack(mModeContext.mGameState.getTrackNameAsOriginal());

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
    characterCar = strPowerslide.getBaseCarFromCar(characterCar);
    selectCar(characterCar);

    hostTrackUpdate(mModeContext.getGameState().getAICount());


#if 0
    {
        {

            //broadcast interval
            Ogre::Vector4 posBroadcast = screenAdaptionRelative * Ogre::Vector4(540.0f, 80.0f, 60.0f, 12.0f);
            mWidgetBroadcast = gui->createWidget<MyGUI::ComboBox>("ComboBox", posBroadcast.x, posBroadcast.y, posBroadcast.z, posBroadcast.w, MyGUI::Align::Default, "Middle");

            itemToSelect = 0;
            for(size_t q = 50, w = 0; q <= 500; q += 50, ++w)
            {
                mWidgetBroadcast->addItem(Conversions::DMToString(q));

                if(q == mModeContext.getGameState().getMultiplayerBroadcastInterval())
                    itemToSelect = w;
            }
            mWidgetBroadcast->setIndexSelected(itemToSelect);

            mWidgetBroadcast->setEditReadOnly(true);
            mWidgetBroadcast->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenuMulti::processChangeComboBox);
        }

        //labels
        {
            Ogre::Vector4 posLabelPing = screenAdaptionRelative * Ogre::Vector4(0.0f, 0.0f, 40.0f, 12.0f);
            mWidgetPingLabel = gui->createWidget<MyGUI::TextBox>("TextBox", posLabelPing.x, posLabelPing.y, posLabelPing.z, posLabelPing.w, MyGUI::Align::Default, "Middle");
            mWidgetPingLabel->setCaption("");
            //mWidgetPingLabel->setVisible(false);
        }

        {
            Ogre::Vector4 posEvents = screenAdaptionRelative * Ogre::Vector4(20.0f, 130.0f, 600.0f, 300.0f);
            mWidgetEvents = gui->createWidget<MyGUI::ListBox>("ListBox", posEvents.x, posEvents.y, posEvents.z, posEvents.w, MyGUI::Align::Default, "Middle");
            mWidgetEvents->setColour(MyGUI::Colour(0.0f, 0.0f, 0.0f));
        }
    }
#endif

}

#if defined(__ANDROID__)
void UIMainMenuMulti::reloadTextures(const GameState& gameState)
{
    loadMisc(gameState.getPFLoaderData(), gameState.getPFLoaderGameshell());
}
#endif

void UIMainMenuMulti::onButtonReleased(UIButton * button)
{
    if(button == &mTracksValLeft)
    {
        size_t totalTracks;
        int currentTrack = getCurrentTrackIndex(totalTracks);
        --currentTrack;
        if(currentTrack < 0)currentTrack = 0;

        switchTrack(currentTrack);
    }

    if(button == &mTracksValRight)
    {
        size_t totalTracks;
        size_t currentTrack = getCurrentTrackIndex(totalTracks);
        ++currentTrack;
        if(currentTrack >= totalTracks)currentTrack = totalTracks - 1;

        switchTrack(currentTrack);
    }

    if(button == &mLapsValLeft)
    {
        int lapsCount = mModeContext.getGameState().getLapsCount();
        --lapsCount;
        if(lapsCount < GameState::mLapsMin) lapsCount = GameState::mLapsMin;

        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), mModeContext.getGameState().getAIStrength(), lapsCount);
        mLapsCount->setCaption(getLapsCountString());
        updateRoomState();
    }

    if(button == &mLapsValRight)
    {
        size_t lapsCount = mModeContext.getGameState().getLapsCount();
        ++lapsCount;
        if(lapsCount > GameState::mLapsMax) lapsCount = GameState::mLapsMax;

        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), mModeContext.getGameState().getAIStrength(), lapsCount);
        mLapsCount->setCaption(getLapsCountString());
        updateRoomState();
    }

    if(button == &mCarValLeft)
    {
        size_t totalCars;
        int carIndex = getCurrentCarIndex(totalCars);
        --carIndex;
        if(carIndex < 0) carIndex = 0;

        switchCar(carIndex);
    }

    if(button == &mCarValRight)
    {
        size_t totalCars;
        size_t carIndex = getCurrentCarIndex(totalCars);
        ++carIndex;
        if(carIndex >= totalCars) carIndex = totalCars - 1;

        switchCar(carIndex);
    }

    if(button == &mCharacterValLeft)
    {
        size_t totalChars;
        int characterIndex = getCurrentCharacterIndex(totalChars);
        --characterIndex;
        if(characterIndex < 0) characterIndex = 0;

        switchCharacter(characterIndex);
    }

    if(button == &mCharacterValRight)
    {
        size_t totalChars;
        size_t characterIndex = getCurrentCharacterIndex(totalChars);
        ++characterIndex;
        if(characterIndex >= totalChars) characterIndex = totalChars - 1;

        switchCharacter(characterIndex);
    }
}

void UIMainMenuMulti::frameStarted(const Ogre::FrameEvent &evt)
{
    mEditBoxMessage.frameStarted(evt);
}

void UIMainMenuMulti::keyUp(MyGUI::KeyCode _key, wchar_t _char)
{
#if !defined(__ANDROID__)
    mEditBoxMessage.keyUp(_key, _char);
    if(_key == MyGUI::KeyCode::Return || _key == MyGUI::KeyCode::NumpadEnter)
    {
        mEditBoxMessage.setText("");
    }
    updateRoomState(mEditBoxMessage.getText());
#endif
}

void UIMainMenuMulti::mousePressed(const Ogre::Vector2& pos)
{
    UIBaseMenu::mousePressed(pos);
    //mEditBoxMessage.mouseReleased(pos);

    mWeaponsVal.mousePressed(pos);
    mTracksValLeft.mousePressed(pos);
    mTracksValRight.mousePressed(pos);
    mLapsValLeft.mousePressed(pos);
    mLapsValRight.mousePressed(pos);
    mCarValLeft.mousePressed(pos);
    mCarValRight.mousePressed(pos);
    mCharacterValLeft.mousePressed(pos);
    mCharacterValRight.mousePressed(pos);
}

void UIMainMenuMulti::mouseReleased(const Ogre::Vector2& pos)
{
    UIBaseMenu::mouseReleased(pos);

    if(mMainChatButtons[0] && OgreBites::Widget::isCursorOver(mMainChatButtons[0], pos, 0))
    {
        readySwitcher();
    }

    //race params switch
    if(mModeContext.mGameState.isMultiplayerMaster())
    {
        //AI count
        for(size_t q = 1; q < GameState::mRaceGridCarsMax; ++q)
        {
            if(mMainChatButtons[q] && OgreBites::Widget::isCursorOver(mMainChatButtons[q], pos, 0))
            {
                aiCountSwitcher(q);
            }
        }

        //AI strength
        if(mAIStrengthVal && mAIStrengthVal->isVisible() && OgreBites::Widget::isCursorOver(mAIStrengthVal, pos, 0))
        {
            size_t strenthIndex = mModeContext.getGameState().getAIStrength();
            ++strenthIndex;
            if(strenthIndex > Insane)strenthIndex = Easy;

            mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), static_cast<AIStrength>(strenthIndex), mModeContext.getGameState().getLapsCount());
            mAIStrengthVal->setCaption(getAIStrengthString());
            updateRoomState();
        }
    }

    mWeaponsVal.mouseReleased(pos);
    mTracksValLeft.mouseReleased(pos);
    mTracksValRight.mouseReleased(pos);
    mLapsValLeft.mouseReleased(pos);
    mLapsValRight.mouseReleased(pos);
    mCarValLeft.mouseReleased(pos);
    mCarValRight.mouseReleased(pos);
    mCharacterValLeft.mouseReleased(pos);
    mCharacterValRight.mouseReleased(pos);
}

void UIMainMenuMulti::mouseMoved(const Ogre::Vector2& pos)
{
    UIBaseMenu::mouseMoved(pos);

    //race params highlight
    if(mModeContext.mGameState.isMultiplayerMaster())
    {
        UIMainMenuLabels::checkCursorOverLabel(pos, mAIStrengthVal);
    }

    mTracksValLeft.mouseMoved(pos);
    mTracksValRight.mouseMoved(pos);
    mLapsValLeft.mouseMoved(pos);
    mLapsValRight.mouseMoved(pos);
    mCarValLeft.mouseMoved(pos);
    mCarValRight.mouseMoved(pos);
    mCharacterValLeft.mouseMoved(pos);
    mCharacterValRight.mouseMoved(pos);
}

void UIMainMenuMulti::readySwitcher()
{
    bool changeToReady = true;
    if(mMainChatButtons[0]->getMaterialName() == "Test/OriginalChatBut1")
    {
        changeToReady = false;
    }

    std::string trackName = "";

    if(mModeContext.mGameState.isMultiplayerMaster())
    {
        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
        trackName = availTracks[getCurrentTrackIndex()];
    }


    MultiplayerLobbyData multiplayerLobbyData(
        changeToReady, 
        mModeContext.mGameState.getPlayerCar().getCharacterName(), 
        "", 
        trackName, 
        mModeContext.mGameState.getAICount(), mModeContext.mGameState.getAIStrength(), 
        mModeContext.mGameState.getLapsCount());

    bool success = mMenuMultiMode->getMultiplayerController()->sendLobbyMessage(multiplayerLobbyData, true, 10);

    if(success)
    {
        if(mMainChatButtons[0]->getMaterialName() == "Test/OriginalChatBut0")
        {
            mMainChatButtons[0]->setMaterialName("Test/OriginalChatBut1");
        }
        else
        {
            mMainChatButtons[0]->setMaterialName("Test/OriginalChatBut0");
        }
    }
}

void UIMainMenuMulti::aiCountSwitcher(size_t index)
{
    if(index >= mPlayerToChatList.size())
    {
        if(isAITrack())
        {
            //increase
            if(
                index == GameState::mRaceGridCarsMax - 1                                &&
                mMainChatButtons[index]->getMaterialName() != "Test/OriginalChatBut2"   ||
                index < GameState::mRaceGridCarsMax - 1                                 &&
                mMainChatButtons[index]->getMaterialName() != "Test/OriginalChatBut2"   &&
                mMainChatButtons[index + 1]->getMaterialName() == "Test/OriginalChatBut2"
                )
            {
                mMainChatButtons[index]->setMaterialName("Test/OriginalChatBut2");
                mModeContext.getGameState().setAICount(mModeContext.getGameState().getAICount() + 1);
                static_cast<MultiplayerControllerMaster *>(mMenuMultiMode->getMultiplayerController().get())->reconfigureSession(mModeContext.getGameState().getAICount());
                updateRoomState();
            }
            else
            {
                //decrease
                if(
                    mMainChatButtons[index]->getMaterialName() == "Test/OriginalChatBut2"   &&
                    mMainChatButtons[index - 1]->getMaterialName() != "Test/OriginalChatBut2"
                    )
                {
                    mMainChatButtons[index]->setMaterialName("Test/OriginalChatBut0");
                    mModeContext.getGameState().setAICount(mModeContext.getGameState().getAICount() - 1);
                    static_cast<MultiplayerControllerMaster *>(mMenuMultiMode->getMultiplayerController().get())->reconfigureSession(mModeContext.getGameState().getAICount());
                    updateRoomState();
                }
            }

            mAICountBeforeNonAITrack = mModeContext.getGameState().getAICount();
        }
    }
}

void UIMainMenuMulti::destroy(CustomTrayManager* trayMgr)
{
    UIBase::destroy(trayMgr);

    mEditBoxMessage.destroy(trayMgr);

    mWeaponsVal.destroy(trayMgr);
    mTracksValLeft.destroy(trayMgr);
    mTracksValRight.destroy(trayMgr);
    mLapsValLeft.destroy(trayMgr);
    mLapsValRight.destroy(trayMgr);
    mCarValLeft.destroy(trayMgr);
    mCarValRight.destroy(trayMgr);
    mCharacterValLeft.destroy(trayMgr);
    mCharacterValRight.destroy(trayMgr);
}

#if 0

void UIMainMenuMulti::processChangeComboBox(MyGUI::Widget* sender, size_t index)
{
    if(sender == mWidgetBroadcast)
    {
        mModeContext.getGameState().setMultiplayerBroadcastInterval((index + 1) * 50);
        mMenuMultiMode->getMultiplayerController()->setBroadcastInterval(mModeContext.getGameState().getMultiplayerBroadcastInterval());
    }

    updateRoomState();
}

#endif

void UIMainMenuMulti::onStartPossible()
{
    setControlActive(4, true);
}

void UIMainMenuMulti::onStartNotPossible()
{
    setControlActive(4, false);
}

void UIMainMenuMulti::addEvent(const std::string& eventItem, bool isMessage)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[UIMainMenuMulti::addEvent]: " + eventItem);

    /*
    if(isMessage)
        mWidgetEvents->insertItem(0, "#FF0000" + eventItem);
    else
        mWidgetEvents->insertItem(0, "#FFFFFF" + eventItem);

    mWidgetEvents->setScrollPosition(0);
    */
}

void UIMainMenuMulti::roomEnter(const std::string& roomName, const std::string& player, const std::vector<std::string>& players)
{
    addCurrentPlayer(player);

    for(size_t q = 0; q < players.size(); ++q)
    {
        addOtherPlayer(q, players[q], false);
    }
}

void UIMainMenuMulti::roomJoined(const std::string& player)
{
    addOtherPlayer(mPlayerToChatList.size() - 1, player, false);
    updateRoomState();//send skins info
}

void UIMainMenuMulti::roomLeft(const std::string& player)
{
    std::map<std::string, std::pair<std::string, std::string> > playerToPlayerStatus;//ready material, skin

    for(std::map<std::string, size_t>::const_iterator i = mPlayerToChatList.begin(); i != mPlayerToChatList.end(); ++i)
    {
        playerToPlayerStatus[(*i).first] = std::make_pair(
            mMainChatButtons[(*i).second]->getMaterialName(), 
            mMainCarIcons[(*i).second]->getMaterialName());
    }

    std::map<std::string, size_t>::iterator i = mPlayerToChatList.find(player);
    if(i != mPlayerToChatList.end())
    {
        size_t indexToDelete = (*i).second;
        size_t indexToDeleteMessage = indexToDelete - 1;

        playerToPlayerStatus.erase(player);

        mPlayerToChatList.erase(i);

        for(std::map<std::string, size_t>::iterator ii = mPlayerToChatList.begin(), j = mPlayerToChatList.end(); ii != j; ++ii)
        {
            if((*ii).second > indexToDelete)
            {
                --(*ii).second;
            }
        }

        for(size_t q = 1; q < GameState::mRaceGridCarsMax; ++q)
        {
            if(q >= indexToDeleteMessage && q < (GameState::mRaceGridCarsMax - 1))
            {
                size_t indexToRead = q + 1;
                if(indexToRead < (GameState::mRaceGridCarsMax - 1))
                {
                    mChatroomPlayersMessages[q]->setCaption(mChatroomPlayersMessages[indexToRead]->getCaption());
                    mChatroomPlayersMessages[indexToRead]->setCaption("");
                }
            }
            mChatroomPlayers[q]->setCaption("");
        }

        mMainCarIcons[playerToPlayerStatus.size()]->hide();
        mMainChatButtons[playerToPlayerStatus.size()]->setMaterialName("Test/OriginalChatBut0");

        for(std::map<std::string, std::pair<std::string, std::string> >::iterator ii = playerToPlayerStatus.begin(), j = playerToPlayerStatus.end(); ii != j; ++ii)
        {
            std::string curPlayer = (*ii).first;
            std::map<std::string, size_t>::iterator iii = mPlayerToChatList.find(curPlayer);
            if((*iii).second != 0)
            {
                mMainChatButtons[(*iii).second]->setMaterialName((*ii).second.first);
                mMainCarIcons[(*iii).second]->setMaterialName((*ii).second.second);
            }
        }

        for(std::map<std::string, size_t>::iterator ii = mPlayerToChatList.begin(), j = mPlayerToChatList.end(); ii != j; ++ii)
        {
            mChatroomPlayers[(*ii).second]->setCaption((*ii).first);
        }
    }
}

void UIMainMenuMulti::playerMessage(const std::string& player, const std::string& message)
{
    std::map<std::string, size_t>::iterator i = mPlayerToChatList.find(player);
    if(i != mPlayerToChatList.end())
    {
        size_t index = (*i).second;
        if(index > 0)
        {
            mChatroomPlayersMessages[index - 1]->setCaption(message);
        }
    }
}

void UIMainMenuMulti::playerReadyChange(const std::string& player, bool isReady)
{
    std::map<std::string, size_t>::iterator i = mPlayerToChatList.find(player);
    if(i != mPlayerToChatList.end())
    {
        size_t index = (*i).second;
        if(!isReady)
        {
            mMainChatButtons[index]->setMaterialName("Test/OriginalChatBut0");
        }
        else
        {
            mMainChatButtons[index]->setMaterialName("Test/OriginalChatBut1");
        }
    }
}

void UIMainMenuMulti::playerSkinChanged(const std::string& player, const std::string& characterName)
{
    std::map<std::string, size_t>::iterator i = mPlayerToChatList.find(player);
    if(i != mPlayerToChatList.end())
    {
        size_t index = (*i).second;
        std::string iconName = mModeContext.getGameState().getSTRPowerslide().getValue(characterName + " parameters", "icon", "car0_0s.bmp");
        std::string matName = "Test/" + iconName;
        mMainCarIcons[index]->setMaterialName(matName);
    }
}

void UIMainMenuMulti::hostTrackUpdate(size_t aiCount)
{

    //ai count
    mModeContext.getGameState().setAICount(aiCount);
    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        if(mMainChatButtons[q]->getMaterialName() == "Test/OriginalChatBut2")
        {
            mMainChatButtons[q]->setMaterialName("Test/OriginalChatBut0");
        }

        if(q >= GameState::mRaceGridCarsMax - mModeContext.getGameState().getAICount())
        {
            mMainChatButtons[q]->setMaterialName("Test/OriginalChatBut2");
        }
    }

    //track name
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
    size_t currentTrack = getCurrentTrackIndex();
    mTrackNameSelected->setCaption(strPowerslide.getTrackTitle(availTracks[currentTrack]));
    selectTrack(mModeContext.mGameState.getTrackNameAsOriginal());

    //laps count
    mLapsCount->setCaption(getLapsCountString());

    //AI strength
    mAIStrengthVal->setCaption(getAIStrengthString());
}

void UIMainMenuMulti::updateRoomState(const std::string& playerMessage)const
{
    bool changeToReady = true;
    if(mMainChatButtons[0]->getMaterialName() != "Test/OriginalChatBut1")
        changeToReady = false;

    if(mModeContext.mGameState.isMultiplayerMaster())
    {

        MultiplayerLobbyData multiplayerLobbyData(
            changeToReady, 
            mModeContext.mGameState.getPlayerCar().getCharacterName(), 
            playerMessage, 
            mModeContext.mGameState.getTrackName(), 
            mModeContext.mGameState.getAICount(), mModeContext.mGameState.getAIStrength(), 
            mModeContext.mGameState.getLapsCount());

        bool success = mMenuMultiMode->getMultiplayerController()->sendLobbyMessage(multiplayerLobbyData, true, 10);
    }
    else
    {
        MultiplayerLobbyData multiplayerLobbyData(
            changeToReady, 
            mModeContext.mGameState.getPlayerCar().getCharacterName(), 
            playerMessage, 
            "", 
            0, 0, 
            0);

        bool success = mMenuMultiMode->getMultiplayerController()->sendLobbyMessage(multiplayerLobbyData, true, 10);
    }
}

void UIMainMenuMulti::setMiscText(const std::string& text, const Ogre::ColourValue& color)
{
    /*
    std::string textColor = OgreColorToString(color);
    textColor = "#" + textColor;
    mWidgetPingLabel->setCaption(textColor + text);
    */
}

void UIMainMenuMulti::panelHit(Ogre::PanelOverlayElement* panel)
{
    if(panel != NULL && panel->getName() == "Race")
    {
        if(mModeContext.mGameState.isMultiplayerMaster())
        {
            mMenuMultiMode->recalculateCharacterNames(static_cast<MultiplayerControllerMaster*>(mMenuMultiMode->getMultiplayerController().get())->getAllPlayersSkins());

            std::vector<std::string> gameCars;
            for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
            {
                gameCars.push_back(mModeContext.mGameState.getAICar(q).getCharacterName());
            }

            static_cast<MultiplayerControllerMaster*>(mMenuMultiMode->getMultiplayerController().get())->setAISkins(gameCars);
        }

        static_cast<MultiplayerControllerMaster *>(mMenuMultiMode->getMultiplayerController().get())->startSession(mModeContext.mGameState.getTrackName(), mModeContext.mGameState.getAIStrength(), mModeContext.mGameState.getLapsCount());
    }

    if(panel != NULL && panel->getName() == "Exit")
    {
        mModeContext.getGameModeSwitcher()->switchMode(ModeMenu);
    }
}

void UIMainMenuMulti::addCurrentPlayer(const std::string& player)
{
    mMainChatButtons[0]->setMaterialName("Test/OriginalChatBut0");
    mChatroomPlayers[0]->setCaption(player);

    mPlayerToChatList.insert(std::make_pair(player, 0));
}

void UIMainMenuMulti::addOtherPlayer(size_t index, const std::string& player, bool isInSession)
{
    mChatroomPlayers[index + 1]->setCaption(player);
    mMainCarIcons[index + 1]->show();
    mPlayerToChatList.insert(std::make_pair(player, index + 1));
    mMainChatButtons[index + 1]->setMaterialName("Test/OriginalChatBut0");
    if(isInSession)
    {
        mMainChatButtons[index + 1]->setMaterialName("Test/OriginalChatBut3");
    }
}

void UIMainMenuMulti::switchTrack(size_t trackIndex)
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    mModeContext.getGameState().setRaceParameters(availTracks[trackIndex], mModeContext.getGameState().getAIStrength());
    mTrackNameSelected->setCaption(strPowerslide.getTrackTitle(availTracks[trackIndex]));
    mLapsCount->setCaption(getLapsCountString());
    if(!isAITrack())
    {
        mModeContext.getGameState().setAICount(0);
        hostTrackUpdate(0);
    }
    else
    {
        mModeContext.getGameState().setAICount(mAICountBeforeNonAITrack);
        hostTrackUpdate(mAICountBeforeNonAITrack);
    }
    selectTrack(mModeContext.mGameState.getTrackNameAsOriginal());
    updateRoomState();
}

void UIMainMenuMulti::switchCar(size_t carIndex)
{
        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");

        std::string characterCar = availCars[carIndex];

        std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

        mModeContext.getGameState().getPlayerCar().setCharacterName(availChars[0]);
        mCarVal->setCaption(getCarString());
        mCharacterVal->setCaption(getCharacterString());
        std::string iconName = mModeContext.getGameState().getSTRPowerslide().getValue(availChars[0] + " parameters", "icon", "car0_0s.bmp");
        std::string matName = "Test/" + iconName;
        mMainCarIcons[0]->setMaterialName(matName);
        selectCar(characterCar);

        updateRoomState();
}

void UIMainMenuMulti::switchCharacter(size_t characterIndex)
{
        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();

        std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
        characterCar = strPowerslide.getBaseCarFromCar(characterCar);

        std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

        mModeContext.getGameState().getPlayerCar().setCharacterName(availChars[characterIndex]);
        std::string iconName = mModeContext.getGameState().getSTRPowerslide().getValue(availChars[characterIndex] + " parameters", "icon", "car0_0s.bmp");
        std::string matName = "Test/" + iconName;
        mMainCarIcons[0]->setMaterialName(matName);

        mCharacterVal->setCaption(getCharacterString());
        selectCar(characterCar);

        updateRoomState();
}

bool UIMainMenuMulti::isAITrack() const
{
    return 
        mModeContext.getGameState().getTrackName() != "stunt track"         &&
        mModeContext.getGameState().getTrackName() != "luge track"          &&
        mModeContext.getGameState().getTrackName() != "Foxnhound1 track"    &&
        mModeContext.getGameState().getTrackName() != "Foxnhound2 track";
}

std::string UIMainMenuMulti::getLapsCountString() const
{
    std::string ret = "N/A";

    if(isAITrack())
    {
        ret = Conversions::DMToString(mModeContext.getGameState().getLapsCount());
    }

    return ret;
}

std::string UIMainMenuMulti::getAIStrengthString() const
{
    std::string ret = "Novice";

    switch(mModeContext.getGameState().getAIStrength())
    {
    case Medium:
        ret = "Medium";
        break;
    case Hard:
        ret = "Hard";
        break;
    case Insane:
        ret = "Insane";
        break;
    }

    return ret;
}

std::string UIMainMenuMulti::getCarString() const
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
    characterCar = strPowerslide.getBaseCarFromCar(characterCar);

    std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");

    size_t itemToSelect = 0;
    for(size_t q = 0; q < availCars.size(); ++q)
    {
        if(availCars[q] == characterCar)
            itemToSelect = q;
    }

    return strPowerslide.getCarTitle(availCars[itemToSelect]);
}

std::string UIMainMenuMulti::getCharacterString() const
{

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
    characterCar = strPowerslide.getBaseCarFromCar(characterCar);
    std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

    size_t itemToSelect = 0;
    for(size_t q = 0; q < availChars.size(); ++q)
    {
        if(availChars[q] == mModeContext.getGameState().getPlayerCar().getCharacterName())
            itemToSelect = q;
    }
    return strPowerslide.getCharacterTitle(availChars[itemToSelect]);
}

size_t UIMainMenuMulti::getCurrentTrackIndex(size_t& total) const
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    total = availTracks.size();

    size_t itemToSelect = 0;
    for(size_t q = 0; q < availTracks.size(); ++q)
    {
        if(availTracks[q] == mModeContext.getGameState().getTrackNameAsOriginal())
            itemToSelect = q;
    }

    return itemToSelect;
}

size_t UIMainMenuMulti::getCurrentTrackIndex() const
{
    size_t total;
    return getCurrentTrackIndex(total);
}

size_t UIMainMenuMulti::getCurrentCarIndex(size_t& total) const
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();

    std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
    std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");
    characterCar = strPowerslide.getBaseCarFromCar(characterCar);

    total = availCars.size();

    size_t itemToSelect = 0;
    for(size_t q = 0; q < availCars.size(); ++q)
    {
        if(availCars[q] == characterCar)
            itemToSelect = q;
    }

    return itemToSelect;
}

size_t UIMainMenuMulti::getCurrentCarIndex() const
{
    size_t total;
    return getCurrentCarIndex(total);
}

size_t UIMainMenuMulti::getCurrentCharacterIndex(size_t& total) const
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
    characterCar = strPowerslide.getBaseCarFromCar(characterCar);

    std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

    total = availChars.size();

    size_t itemToSelect = 0;
    for(size_t q = 0; q < availChars.size(); ++q)
    {
        if(availChars[q] == mModeContext.getGameState().getPlayerCar().getCharacterName())
            itemToSelect = q;
    }

    return itemToSelect;
}

size_t UIMainMenuMulti::getCurrentCharacterIndex() const
{
    size_t total;
    return getCurrentCharacterIndex(total);
}
#endif