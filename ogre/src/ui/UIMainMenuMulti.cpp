#include "../pcheader.h"

#include "UIMainMenuMulti.h"

#include "MyGUI.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

#include "../customs/CustomTrayManager.h"

#include "../loaders/TextureLoader.h"

#include "../gamemodes/MenuMultiMode.h"

#include "../multiplayer/MultiplayerControllerMaster.h"
#include "../multiplayer/MultiplayerControllerSlave.h"

UIMainMenuMulti::UIMainMenuMulti(const ModeContext& modeContext, MenuMultiMode * menuMultiMode)
    : mModeContext(modeContext),
    mMenuMultiMode(menuMultiMode)
{}

UIMainMenuMulti::~UIMainMenuMulti()
{}

void UIMainMenuMulti::loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell)
{
    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "cursor.bmp", 
                                "OriginalCursor", TEMP_RESOURCE_GROUP_NAME, 
                                Ogre::ColourValue(0.0f, 1.0f, 0.1f, 1.0f), 0.1f);

    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "m_chat.bmp", 
                                "OriginalMainBackground", TEMP_RESOURCE_GROUP_NAME);
}

void UIMainMenuMulti::load(MyGUI::Gui* gui, const GameState& gameState)
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

        {
            Ogre::Vector4 posEvents = screenAdaptionRelative * Ogre::Vector4(20.0f, 130.0f, 600.0f, 300.0f);
            mWidgetEvents = gui->createWidget<MyGUI::ListBox>("ListBox", posEvents.x, posEvents.y, posEvents.z, posEvents.w, MyGUI::Align::Default, "Middle");
            //mWidgetEvents->setColour(MyGUI::Colour(0.0f, 0.0f, 0.0f));
        }
    }

    //MyGUI::PointerManager::getInstance().getByName("arrow")->setResourceName();

}

#if defined(__ANDROID__)
void UIMainMenuMulti::reloadTextures(const GameState& gameState)
{
    loadMisc(gameState.getPFLoaderData(), gameState.getPFLoaderGameshell());
}
#endif

void UIMainMenuMulti::processButtonClick(MyGUI::Widget* sender)
{
    if(sender == mWidgetStart)
    {
        mWidgetStart->setEnabled(false);

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

    if(sender == mWidgetJoin)
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
            trackName = availTracks[mWidgetTrack->getItemIndexSelected()];
        }


        MultiplayerLobbyData multiplayerLobbyData(
            changeToReady, 
            mModeContext.mGameState.getPlayerCar().getCharacterName(), 
            "", 
            trackName, 
            mModeContext.mGameState.getAICount(), mModeContext.mGameState.getAIStrength(), 
            mModeContext.mGameState.getLapsCount());

        bool success = mMenuMultiMode->getMultiplayerController()->sendLobbyMessage(multiplayerLobbyData, 10);

        if(success)
        {
            if(mWidgetJoin->getCaption() == "Ready")
            {
                mWidgetJoin->setCaption("Not ready");
            }
            else
            {
                mWidgetJoin->setCaption("Ready");
            }
        }
    }

}

void UIMainMenuMulti::processChangeComboBox(MyGUI::Widget* sender, size_t index)
{
    if(sender == mWidgetTrack)
    {
        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
        mModeContext.getGameState().setRaceParameters(availTracks[index], mModeContext.getGameState().getAIStrength());
        mWidgetLapsCount->setIndexSelected(mModeContext.getGameState().getLapsCount() - 1);
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

void UIMainMenuMulti::onStartPossible()
{
    mWidgetStart->setEnabled(true);
}

void UIMainMenuMulti::onStartNotPossible()
{
    mWidgetStart->setEnabled(false);
}

void UIMainMenuMulti::addEvent(const std::string& eventItem)
{
    mWidgetEvents->insertItem(0, eventItem);
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

        bool success = mMenuMultiMode->getMultiplayerController()->sendLobbyMessage(multiplayerLobbyData, 10);
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

        bool success = mMenuMultiMode->getMultiplayerController()->sendLobbyMessage(multiplayerLobbyData, 10);
    }
}