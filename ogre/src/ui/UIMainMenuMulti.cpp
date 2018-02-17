
#ifndef NO_MULTIPLAYER

#include "UIMainMenuMulti.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

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

UIMainMenuMulti::UIMainMenuMulti(const ModeContext& modeContext, MenuMultiMode * menuMultiMode)
    : UIBaseMenu(modeContext),
    mMenuMultiMode(menuMultiMode),
    mEditBoxMessage(50),
    mWidgetJoin(NULL),
    mWidgetStart(NULL)

{
    memset(mMainChatButtons, NULL, sizeof(Ogre::PanelOverlayElement*));
}

void UIMainMenuMulti::loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell)
{
    loadCommonTextures(pfLoaderGameshell);

    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "m_chat.bmp", 
                                "OriginalMainBackground", TEMP_RESOURCE_GROUP_NAME);

    //empty
    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "chatbut0.bmp", 
                                "OriginalChatBut0", TEMP_RESOURCE_GROUP_NAME,
                                Ogre::ColourValue(1.0f, 0.0f, 0.487f),
                                0.1f,
                                false, 64.0f, 
                                true);

    //green
    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "chatbut1.bmp", 
                                "OriginalChatBut1", TEMP_RESOURCE_GROUP_NAME,
                                Ogre::ColourValue(1.0f, 0.0f, 0.487f),
                                0.1f,
                                false, 64.0f, 
                                true);

    //PC
    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "chatbut2.bmp", 
                                "OriginalChatBut2", TEMP_RESOURCE_GROUP_NAME,
                                Ogre::ColourValue(1.0f, 0.0f, 0.487f),
                                0.1f,
                                false, 64.0f, 
                                true);

    //red
    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "chatbut3.bmp", 
                                "OriginalChatBut3", TEMP_RESOURCE_GROUP_NAME,
                                Ogre::ColourValue(1.0f, 0.0f, 0.487f),
                                0.1f,
                                false, 64.0f, 
                                true);
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
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalChatBut0");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/OriginalChatBut0", 
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
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalChatBut1");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/OriginalChatBut1", 
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
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalChatBut2");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/OriginalChatBut2", 
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
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalChatBut3");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/OriginalChatBut3", 
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
}

void UIMainMenuMulti::load(CustomTrayManager* trayMgr, const GameState& gameState, bool isEnterFromBaseMenu)
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

    if(mModeContext.mGameState.isMultiplayerMaster())
    {
        mWidgetJoin = createButton(trayMgr, OgreBites::TL_NONE, "mWidgetJoin", "Ready", 100.0f);

        Ogre::Vector4 startPos = screenAdaptionRelative * Ogre::Vector4(0.0f, 50.0f, 0.0f, 0.0f);
        mWidgetStart = createButton(trayMgr, OgreBites::TL_NONE, "mWidgetStart", "Start", 100.0f);
        mWidgetStart->getOverlayElement()->setTop(startPos.y);
        mWidgetStart->hide();
    }
    else
    {
        mWidgetJoin = createButton(trayMgr, OgreBites::TL_NONE, "mWidgetJoin", "Ready", 100.0f);
    }

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
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(185.0f, 22.0f * q + 110.0f, 0.0f, 0.0f);
            mChatroomPlayers[q] = createTextArea("MainWindowPlayers1_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mChatroomPlayers[q]->setCaption("");
            mChatroomPlayers[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mChatroomPlayers[q]->setSpaceWidth(9.0f);
            mChatroomPlayers[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mChatroomPlayers[q]->setAlignment(Ogre::TextAreaOverlayElement::Right);
            mChatroomPlayers[q]->setFontName("SdkTrays/Caption");
            mChatroomPlayers[q]->setColour(Ogre::ColourValue::White);
            mMainBackground->addChild(mChatroomPlayers[q]);
        }
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
        }
    }


#if 0
    {
        MyGUI::ImageBox * mainBackground = gui->createWidget<MyGUI::ImageBox>("ImageBox", 0, 0, viewportWidth, viewportHeight, MyGUI::Align::Default, "Wallpaper");
        mainBackground->setImageTexture("OriginalMainBackground");
    }

    {
        if(mModeContext.mGameState.isMultiplayerMaster())
        {
            Ogre::Vector4 posReady = screenAdaptionRelative * Ogre::Vector4(220.0f, 80.0f, 40.0f, 12.0f);
            mWidgetJoin = gui->createWidget<MyGUI::Button>("Button", posReady.x, posReady.y, posReady.z, posReady.w, MyGUI::Align::Default, "Middle");
            mWidgetJoin->setCaption("Ready");
            mWidgetJoin->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);

            Ogre::Vector4 posJoin = screenAdaptionRelative * Ogre::Vector4(260.0f, 80.0f, 40.0f, 12.0f);
            mWidgetStart = gui->createWidget<MyGUI::Button>("Button", posJoin.x, posJoin.y, posJoin.z, posJoin.w, MyGUI::Align::Default, "Middle");
            mWidgetStart->setCaption("Start");
            mWidgetStart->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);
            mWidgetStart->setEnabled(false);

            Ogre::Vector4 posTrack = screenAdaptionRelative * Ogre::Vector4(300.0f, 80.0f, 60.0f, 12.0f);
            mWidgetTrack = gui->createWidget<MyGUI::ComboBox>("ComboBox", posTrack.x, posTrack.y, posTrack.z, posTrack.w, MyGUI::Align::Default, "Middle");

            const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
            std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

            size_t itemToSelect = 0;
            for(size_t q = 0; q < availTracks.size(); ++q)
            {
                mWidgetTrack->addItem(strPowerslide.getTrackTitle(availTracks[q]));

                if(availTracks[q] == mModeContext.getGameState().getTrackNameAsOriginal())
                    itemToSelect = q;
            }

            mWidgetTrack->setIndexSelected(itemToSelect);

            mWidgetTrack->setEditReadOnly(true);
            mWidgetTrack->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenuMulti::processChangeComboBox);

            Ogre::Vector4 posAI = screenAdaptionRelative * Ogre::Vector4(420.0f, 80.0f, 30.0f, 12.0f);
            mWidgetAICount = gui->createWidget<MyGUI::ComboBox>("ComboBox", posAI.x, posAI.y, posAI.z, posAI.w, MyGUI::Align::Default, "Middle");
            for(size_t q = 0; q < 11; ++q)
                mWidgetAICount->addItem(Conversions::DMToString(q));

            mWidgetAICount->setIndexSelected(mModeContext.getGameState().getAICount());
            mWidgetAICount->setEditReadOnly(true);
            mWidgetAICount->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenuMulti::processChangeComboBox);


            Ogre::Vector4 posAIStrength = screenAdaptionRelative * Ogre::Vector4(460.0f, 80.0f, 30.0f, 12.0f);
            mWidgetAIStrength = gui->createWidget<MyGUI::ComboBox>("ComboBox", posAIStrength.x, posAIStrength.y, posAIStrength.z, posAIStrength.w, MyGUI::Align::Default, "Middle");
            mWidgetAIStrength->addItem("Novice");
            mWidgetAIStrength->addItem("Advanced");
            mWidgetAIStrength->addItem("Expert");
            mWidgetAIStrength->addItem("Insane");

            mWidgetAIStrength->setIndexSelected(mModeContext.getGameState().getAIStrength());
            mWidgetAIStrength->setEditReadOnly(true);
            mWidgetAIStrength->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenuMulti::processChangeComboBox);


            Ogre::Vector4 posLapsCount = screenAdaptionRelative * Ogre::Vector4(500.0f, 80.0f, 30.0f, 12.0f);
            mWidgetLapsCount = gui->createWidget<MyGUI::ComboBox>("ComboBox", posLapsCount.x, posLapsCount.y, posLapsCount.z, posLapsCount.w, MyGUI::Align::Default, "Middle");
            for(size_t q = 1; q <= 10; ++q)
                mWidgetLapsCount->addItem(Conversions::DMToString(q));

            mWidgetLapsCount->setIndexSelected(mModeContext.getGameState().getLapsCount() - 1);
            mWidgetLapsCount->setEditReadOnly(true);
            mWidgetLapsCount->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenuMulti::processChangeComboBox);

        }
        else
        {
            Ogre::Vector4 posJoin = screenAdaptionRelative * Ogre::Vector4(220.0f, 80.0f, 40.0f, 12.0f);
            mWidgetJoin = gui->createWidget<MyGUI::Button>("Button", posJoin.x, posJoin.y, posJoin.z, posJoin.w, MyGUI::Align::Default, "Middle");
            mWidgetJoin->setCaption("Ready");
            mWidgetJoin->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);
        }

        {
            const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
            std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
            characterCar = strPowerslide.getBaseCarFromCar(characterCar);

            std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");
            std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

            //car
            Ogre::Vector4 posCar = screenAdaptionRelative * Ogre::Vector4(360.0f, 60.0f, 60.0f, 12.0f);
            mWidgetCar = gui->createWidget<MyGUI::ComboBox>("ComboBox", posCar.x, posCar.y, posCar.z, posCar.w, MyGUI::Align::Default, "Middle");

            size_t itemToSelect = 0;
            for(size_t q = 0; q < availCars.size(); ++q)
            {
                mWidgetCar->addItem(strPowerslide.getCarTitle(availCars[q]));

                if(availCars[q] == characterCar)
                    itemToSelect = q;
            }
            mWidgetCar->setIndexSelected(itemToSelect);
            mWidgetCar->setEditReadOnly(true);
            mWidgetCar->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenuMulti::processChangeComboBox);


            //character
            Ogre::Vector4 posChar = screenAdaptionRelative * Ogre::Vector4(360.0f, 80.0f, 60.0f, 12.0f);
            mWidgetCharacter = gui->createWidget<MyGUI::ComboBox>("ComboBox", posChar.x, posChar.y, posChar.z, posChar.w, MyGUI::Align::Default, "Middle");

            itemToSelect = 0;
            for(size_t q = 0; q < availChars.size(); ++q)
            {
                mWidgetCharacter->addItem(strPowerslide.getCharacterTitle(availChars[q]));

                if(availChars[q] == mModeContext.getGameState().getPlayerCar().getCharacterName())
                    itemToSelect = q;
            }
            mWidgetCharacter->setIndexSelected(itemToSelect);

            mWidgetCharacter->setEditReadOnly(true);
            mWidgetCharacter->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenuMulti::processChangeComboBox);


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

        //messages
        {
            Ogre::Vector4 posMessage = screenAdaptionRelative * Ogre::Vector4(220.0f, 110.0f, 200.0f, 12.0f);
            mWidgetMessage = gui->createWidget<MyGUI::EditBox>("EditBox", posMessage.x, posMessage.y, posMessage.z, posMessage.w, MyGUI::Align::Default, "Middle");
            mWidgetMessage->eventKeyButtonPressed += MyGUI::newDelegate(this, &UIMainMenuMulti::processKeyPress);

            Ogre::Vector4 posSend = screenAdaptionRelative * Ogre::Vector4(440.0f, 110.0f, 40.0f, 12.0f);
            mWidgetSendMessage = gui->createWidget<MyGUI::Button>("Button", posSend.x, posSend.y, posSend.z, posSend.w, MyGUI::Align::Default, "Middle");
            mWidgetSendMessage->setCaption("Send");
            mWidgetSendMessage->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);
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
}

void UIMainMenuMulti::mouseReleased(const Ogre::Vector2& pos)
{
    UIBaseMenu::mouseReleased(pos);

    if(mMainChatButtons[0] && OgreBites::Widget::isCursorOver(mMainChatButtons[0], pos, 0))
    {
        buttonHit(mWidgetJoin);
    }

    if(mModeContext.mGameState.isMultiplayerMaster())
    {
        for(size_t q = 1; q < GameState::mRaceGridCarsMax; ++q)
        {
            if(mMainChatButtons[q] && OgreBites::Widget::isCursorOver(mMainChatButtons[q], pos, 0))
            {
                aiCountSwitcher(q);
            }
        }
    }

    if(mWidgetJoin && OgreBites::Widget::isCursorOver(mWidgetJoin->getOverlayElement(), pos, 0))
    {
        buttonHit(mWidgetJoin);
    }
    if(mWidgetStart && OgreBites::Widget::isCursorOver(mWidgetStart->getOverlayElement(), pos, 0))
    {
        buttonHit(mWidgetStart);
    }
}

void UIMainMenuMulti::mouseMoved(const Ogre::Vector2& pos)
{
    UIBaseMenu::mouseMoved(pos);
}

void UIMainMenuMulti::buttonHit(OgreBites::Button* button)
{
#if defined(__ANDROID__)
    LOGI("UIMainMenuMulti[buttonHit]: Begin"); 
#endif

    if(button->getName() == "mWidgetJoin")
    {
        bool changeToReady = true;
        if(mWidgetJoin->getCaption() != "Ready")
        {
            changeToReady = false;
        }

        std::string trackName = "";

        if(mModeContext.mGameState.isMultiplayerMaster())
        {
            const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
            std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
            //trackName = availTracks[mWidgetTrack->getItemIndexSelected()];
            trackName = availTracks[0];
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
            if(mWidgetJoin->getCaption() == "Ready")
            {
                mMainChatButtons[0]->setMaterialName("Test/OriginalChatBut1");
                mWidgetJoin->setCaption("Not ready");
            }
            else
            {
                mMainChatButtons[0]->setMaterialName("Test/OriginalChatBut0");
                mWidgetJoin->setCaption("Ready");
            }
        }
    }

    if(button->getName() == "mWidgetStart")
    {
        if(mModeContext.mGameState.isMultiplayerMaster())
        {
            //mWidgetStart->setEnabled(false);

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

#if defined(__ANDROID__)
    LOGI("UIMainMenuMulti[buttonHit]: End"); 
#endif
}

void UIMainMenuMulti::aiCountSwitcher(size_t index)
{
    if(index >= mPlayerToChatList.size())
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
    }
}

void UIMainMenuMulti::destroy(CustomTrayManager* trayMgr)
{
    UIBase::destroy(trayMgr);

    mEditBoxMessage.destroy(trayMgr);
}

#if 0

void UIMainMenuMulti::processChangeComboBox(MyGUI::Widget* sender, size_t index)
{
    if(sender == mWidgetTrack)
    {
        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
        mModeContext.getGameState().setRaceParameters(availTracks[index], mModeContext.getGameState().getAIStrength());
        mWidgetLapsCount->setIndexSelected(mModeContext.getGameState().getLapsCount() - 1);
        if(
            mModeContext.getGameState().getTrackName() == "stunt track"         ||
            mModeContext.getGameState().getTrackName() == "luge track"          ||
            mModeContext.getGameState().getTrackName() == "Foxnhound1 track"    ||
            mModeContext.getGameState().getTrackName() == "Foxnhound2 track"
            )
            mModeContext.getGameState().setAICount(0);
    }

    if(sender == mWidgetCar)
    {
        mWidgetCharacter->deleteAllItems();

        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");
        std::string characterCar = availCars[index];
        std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

        for(size_t q = 0; q < availChars.size(); ++q)
        {
            mWidgetCharacter->addItem(strPowerslide.getCharacterTitle(availChars[q]));
        }

        mWidgetCharacter->setIndexSelected(0);
        mModeContext.getGameState().getPlayerCar().setCharacterName(availChars[0]);
    }

    if(sender == mWidgetCharacter)
    {
        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();

        std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
        characterCar = strPowerslide.getBaseCarFromCar(characterCar);

        std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

        mModeContext.getGameState().getPlayerCar().setCharacterName(availChars[index]);
    }

    if(sender == mWidgetAICount)
    {
        mModeContext.getGameState().setAICount(index);
        if(
            mModeContext.getGameState().getTrackName() == "stunt track"         ||
            mModeContext.getGameState().getTrackName() == "luge track"          ||
            mModeContext.getGameState().getTrackName() == "Foxnhound1 track"    ||
            mModeContext.getGameState().getTrackName() == "Foxnhound2 track"
            )
            mModeContext.getGameState().setAICount(0);
        static_cast<MultiplayerControllerMaster *>(mMenuMultiMode->getMultiplayerController().get())->reconfigureSession(index);
    }

    if(sender == mWidgetAIStrength)
    {
        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), static_cast<AIStrength>(index), mModeContext.getGameState().getLapsCount());
    }

    if(sender == mWidgetLapsCount)
    {
        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), mModeContext.getGameState().getAIStrength(), index + 1);
    }

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
    mWidgetStart->show();
}

void UIMainMenuMulti::onStartNotPossible()
{
    mWidgetStart->hide();
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
}

void UIMainMenuMulti::roomLeft(const std::string& player)
{
    std::map<std::string, size_t>::iterator i = mPlayerToChatList.find(player);
    if(i != mPlayerToChatList.end())
    {
        size_t indexToDelete = (*i).second;
        size_t indexToDeleteMessage = indexToDelete - 1;

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

                    mMainChatButtons[q]->setMaterialName(mMainChatButtons[indexToRead]->getMaterialName());
                    mMainChatButtons[indexToRead]->setMaterialName("Test/OriginalChatBut0");
                }
            }
            mChatroomPlayers[q]->setCaption("");
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
            mChatroomPlayers[index]->setColour(Ogre::ColourValue::White);
        }
        else
        {
            mMainChatButtons[index]->setMaterialName("Test/OriginalChatBut1");
            mChatroomPlayers[index]->setColour(Ogre::ColourValue::Green);
        }
    }
}

void UIMainMenuMulti::hostAICountChange(size_t count)
{
    mModeContext.getGameState().setAICount(count);

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
}

void UIMainMenuMulti::updateRoomState(const std::string& playerMessage)const
{
    bool changeToReady = true;
    if(mWidgetJoin->getCaption() == "Ready")
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
}

void UIMainMenuMulti::addCurrentPlayer(const std::string& player)
{
    mMainChatButtons[0]->setMaterialName("Test/OriginalChatBut0");
    mChatroomPlayers[0]->setCaption(player);
    mChatroomPlayers[0]->setColour(Ogre::ColourValue::White);

    mPlayerToChatList.insert(std::make_pair(player, 0));
}

void UIMainMenuMulti::addOtherPlayer(size_t index, const std::string& player, bool isInSession)
{
    mChatroomPlayers[index + 1]->setCaption(player);
    mPlayerToChatList.insert(std::make_pair(player, index + 1));
    mChatroomPlayers[index + 1]->setColour(Ogre::ColourValue::White);
    mMainChatButtons[index + 1]->setMaterialName("Test/OriginalChatBut0");
    if(isInSession)
    {
        mMainChatButtons[index + 1]->setMaterialName("Test/OriginalChatBut3");
        mChatroomPlayers[index + 1]->setColour(Ogre::ColourValue(1.0f, 1.0f, 0.0f));
    }
}

#endif