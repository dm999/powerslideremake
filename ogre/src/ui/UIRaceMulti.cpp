
#ifndef NO_MULTIPLAYER
#include "UIRaceMulti.h"

#include "../gamemodes/MultiPlayerMode.h"

#include "../tools/OgreTools.h"
#include "../loaders/TextureLoader.h"

#include "../customs/CustomTrayManager.h"

UIRaceMulti::UIRaceMulti(const ModeContext& modeContext, MultiPlayerMode * multiPlayerMode)
    : mModeContext(modeContext),
    mMultiPlayerMode(multiPlayerMode),
    mEditBoxMessage(50)
{}

void UIRaceMulti::load(CustomTrayManager* trayMgr, const GameState& gameState)
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
        std::vector<Ogre::String> texName;
        texName.push_back("CustomBackgroundBlackTransparent");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/CustomBackgroundBlackTransparent", 
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
        mEditBoxMessage.setBackgroundMaterial("Test/CustomBackgroundBlackTransparent");
        mEditBoxMessage.init(screenAdaptionRelative, trayMgr->getTrayContainer(OgreBites::TL_NONE), Ogre::Vector4(439.0f, 0.0f, 200.0f, 18.0f), 36.0f, true);
        mEditBoxMessage.setText("");
        mEditBoxMessage.hide();
    }
}

void UIRaceMulti::loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell)
{
    TextureLoader().generate("CustomBackgroundBlackTransparent", 64, 64, Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.5f));
}

#if defined(__ANDROID__)
void UIRaceMulti::reloadTextures(const GameState& gameState)
{
    loadMisc(gameState.getPFLoaderData(), gameState.getPFLoaderGameshell());
}
#endif

void UIRaceMulti::keyUp(MyGUI::KeyCode _key, wchar_t _char)
{
#if !defined(__ANDROID__)
    if(mEditBoxMessage.isVisible())
    {
        mEditBoxMessage.keyUp(_key, _char);
        if(_key == MyGUI::KeyCode::Return || _key == MyGUI::KeyCode::NumpadEnter)
        {
            std::string message = mEditBoxMessage.getText();
            if(!message.empty())
            {
                mEditBoxMessage.setText("");

                MultiplayerLobbyData multiplayerLobbyData(
                    false, 
                    mModeContext.getGameState().getPlayerCar().getCharacterName(), 
                    message, 
                    "", 
                    0, 0, 
                    0);

                bool success = mMultiPlayerMode->getMultiplayerController()->sendLobbyMessage(multiplayerLobbyData, true, 10);
            }
        }
    }
#endif
}

void UIRaceMulti::frameStarted(const Ogre::FrameEvent &evt)
{
    mEditBoxMessage.frameStarted(evt);
}

void UIRaceMulti::destroy(CustomTrayManager* trayMgr)
{
    mEditBoxMessage.destroy(trayMgr);
}

void UIRaceMulti::switchVisibleMessageWidget()
{
    if(mEditBoxMessage.isVisible())
    {
        
        mEditBoxMessage.hide();
    }
    else
    {
        mEditBoxMessage.setText("");
        mEditBoxMessage.show();
    }
}

#endif