
#include "UIMainMenuBackground.h"

#include "../customs/CustomTrayManager.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

#include "../listeners/LoaderListener.h"

#include "../loaders/TextureLoader.h"

void UIMainMenuBackground::createBackgroundTextures(const PFLoader& pfLoaderGameshell, LoaderListener* loaderListener)
{
    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "bg.bmp", 
                                "OriginalMainBackground", TEMP_RESOURCE_GROUP_NAME);

    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "tmga.bmp", 
                                "OriginalBackgroundA", TEMP_RESOURCE_GROUP_NAME);

    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "tmgb.bmp", 
                                "OriginalBackgroundB", TEMP_RESOURCE_GROUP_NAME);

    if(loaderListener)
        loaderListener->loadState(0.2f, "backgrounds loaded");

    {
        const char * trackNames[] = {"track1.bmp", "track0.bmp", "track4.bmp", "track7.bmp", "track5.bmp", "track3.bmp", "track2.bmp", "track6.bmp", "track9.bmp", "track8.bmp", "track11.bmp", "track10.bmp"};

        for(size_t q = 0; q < amountTracks; ++q)
        {
            TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", trackNames[q], 
                                "OriginalTrack" + Conversions::DMToString(q), TEMP_RESOURCE_GROUP_NAME);
        }
    }

    if(loaderListener)
        loaderListener->loadState(0.4f, "track textures loaded");

    {

        const char * carNames[] = {"car0_1.bmp", "car1_0.bmp", "car5_0.bmp", "car3_0.bmp", "car2_0.bmp", "car4_0.bmp", "car6_0.bmp"};
        const char * carSNames[] = {"carst0.bmp", "carst1.bmp", "carst5.bmp", "carst3.bmp", "carst2.bmp", "carst4.bmp", "carst6.bmp"};

        for(size_t q = 0; q < amountCars; ++q)
        {
            TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", carNames[q], 
                                "OriginalCar" + Conversions::DMToString(q), TEMP_RESOURCE_GROUP_NAME);

            TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", carSNames[q], 
                                "OriginalCarS" + Conversions::DMToString(q), TEMP_RESOURCE_GROUP_NAME);
        }

        std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
        for (size_t q = 0; q < availableCharacters.size(); ++q)
        {
            std::string iconName = mModeContext.getGameState().getSTRPowerslide().getValue(availableCharacters[q] + " parameters", "icon", "car0_0s.bmp");
            size_t carIndex = 0;
            size_t charIndex = 0;
            sscanf(iconName.c_str(), "car%d_%ds.bmp", &carIndex, &charIndex);

            std::string carName = "car" + Conversions::DMToString(carIndex) + "_" + Conversions::DMToString(charIndex) + ".bmp";
            std::string charName = "char" + Conversions::DMToString(carIndex) + "_" + Conversions::DMToString(charIndex) + ".bmp";

            //supercar is different
            if(carIndex < 6)
            {
                TextureLoader().load( pfLoaderGameshell, 
                                    "data/gameshell", charName, 
                                    "OriginalChar" + Conversions::DMToString(carIndex) + "_" + Conversions::DMToString(charIndex), TEMP_RESOURCE_GROUP_NAME);
            }
            else
            {
                TextureLoader().load( pfLoaderGameshell, 
                                    "data/gameshell", carName, 
                                    "OriginalChar" + Conversions::DMToString(carIndex) + "_" + Conversions::DMToString(charIndex), TEMP_RESOURCE_GROUP_NAME);
            }

            if(loaderListener)
                loaderListener->loadState(0.4f + 0.6f * (static_cast<float>(q + 1) / availableCharacters.size()), availableCharacters[q] + " loaded");
        }

    }

}

void UIMainMenuBackground::createBackgroundMaterials()
{

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

    //additional background (feryl)
    {
        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalBackgroundA");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BackgroundA", 
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

        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalBackgroundB");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BackgroundB", 
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
    }

    for(size_t q = 0; q < amountTracks; ++q)
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalTrack" + Conversions::DMToString(q));
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/Background_Track_" + Conversions::DMToString(q), 
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

    for(size_t q = 0; q < amountCars; ++q)
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalCar" + Conversions::DMToString(q));
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/Background_Car_" + Conversions::DMToString(q), 
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

    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
    for (size_t q = 0; q < availableCharacters.size(); ++q)
    {
        std::string iconName = mModeContext.getGameState().getSTRPowerslide().getValue(availableCharacters[q] + " parameters", "icon", "car0_0s.bmp");
        size_t carIndex = 0;
        size_t charIndex = 0;
        sscanf(iconName.c_str(), "car%d_%ds.bmp", &carIndex, &charIndex);

        std::vector<Ogre::String> texName;
        texName.push_back("OriginalChar" + Conversions::DMToString(carIndex) + "_" + Conversions::DMToString(charIndex));
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/Background_Character_" + Conversions::DMToString(q), 
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
}

void UIMainMenuBackground::createBackgroundUI(const Ogre::Matrix4& screenAdaptionRelative, CustomTrayManager* trayMgr)
{

    Ogre::Real viewportWidth = screenAdaptionRelative[0][0] * 640.0f; 
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f; 

    {
        mMainBackground = createPanel("MainBackground", viewportWidth, viewportHeight, 0.0f, 0.0f, "Test/MainBackground");
        mMainBackground->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mMainBackground);
        //mMainBackground->hide();
    }

    {
        Ogre::Vector4 backgroundA = screenAdaptionRelative * Ogre::Vector4(0.0f, 0.0f, 197.0f, 328.0f);
        Ogre::Vector4 backgroundB = screenAdaptionRelative * Ogre::Vector4(197.0f, 0.0f, 197.0f + 102.0f, 217.0f);

        mBackgroundA = createPanel("BackgroundA", backgroundA, "Test/BackgroundA");
        mBackgroundA->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackgroundA);

        mBackgroundB = createPanel("BackgroundB", backgroundB, "Test/BackgroundB");
        mBackgroundB->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackgroundB);
    }

    {
        Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(0.0f, 0.0f, 313.0f, 218.0f);

        mBackgroundTrack = createPanel("BackgroundTrack", background, "Test/Background_Track_0");
        mBackgroundTrack->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackgroundTrack);
    }

    {
        Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(0.0f, 0.0f, 311.0f, 219.0f);

        mBackgroundCar = createPanel("BackgroundCar", background, "Test/Background_Car_0");
        mBackgroundCar->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackgroundCar);
    }

    {
        Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(0.0f, 0.0f, 309.0f, 358.0f);

        mBackgroundCharacter = createPanel("BackgroundCharacter", background, "Test/Background_Character_0");
        mBackgroundCharacter->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackgroundCharacter);
    }
}

void UIMainMenuBackground::setCurrentTrackLogo()
{
    setTrackLogo(mRemapTrack[mModeContext.getGameState().getTrackNameAsOriginal()]);
}


void UIMainMenuBackground::setTrackLogo(size_t index)
{
    mBackgroundTrack->setMaterialName("Test/Background_Track_" + Conversions::DMToString(index));
}

void UIMainMenuBackground::setCurrentCarLogo()
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::string character = mModeContext.getGameState().getPlayerCar().getCharacterName();
    std::string characterCar = strPowerslide.getCarFromCharacter(character);
    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
    characterCar = strPowerslide.getBaseCarFromCar(characterCar);

    setCarLogo(mRemapCar[characterCar]);
}

void UIMainMenuBackground::setCarLogo(size_t index)
{
    mBackgroundCar->setMaterialName("Test/Background_Car_" + Conversions::DMToString(index));
}

void UIMainMenuBackground::setCharacterLogo(size_t index)
{
    mBackgroundCharacter->setMaterialName("Test/Background_Character_" + Conversions::DMToString(index));
}

void UIMainMenuBackground::selectTrack(const std::string& trackName)
{
    UIBaseMenu::selectTrack(trackName);

    if(mRemapTrack.find(trackName) != mRemapTrack.end())
    {
        setTrackLogo(mRemapTrack[trackName]);
    }
    else
    {
        assert(false);
    }
}

void UIMainMenuBackground::selectCar(const std::string& carName)
{
    UIBaseMenu::selectCar(carName);

    if(mRemapCar.find(carName) != mRemapCar.end())
    {
        setCarLogo(mRemapCar[carName]);
    }
    else
    {
        assert(false);
    }
}

void UIMainMenuBackground::showBackgroundCharacter()
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::string character = mModeContext.getGameState().getPlayerCar().getCharacterName();
    std::string characterCar = strPowerslide.getCarFromCharacter(character);
    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
    characterCar = strPowerslide.getBaseCarFromCar(characterCar);

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    if(characterCar == "supercar")
    {
        mBackgroundCharacter->setHeight(219.0f * viewportHeight / 480.0f);
        mBackgroundCharacter->setWidth(311.0f * viewportWidth / 640.0f);
    }
    else
    {
        mBackgroundCharacter->setHeight(358.0f * viewportHeight / 480.0f);
        mBackgroundCharacter->setWidth(309.0f * viewportWidth / 640.0f);
    }
    mBackgroundCharacter->show();
}

void UIMainMenuBackground::hideAllBackgrounds()
{
    mBackgroundA->hide(); 
    mBackgroundB->hide();

    mBackgroundTrack->hide();
    mBackgroundCar->hide();
    mBackgroundCharacter->hide();
}