#include "../pcheader.h"

#include "UIBaseMenu.h"

#include "../loaders/PFLoader.h"
#include "../loaders/TextureLoader.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

namespace{
    //order based on mRemapTrack
    const std::string controlTracksFiles[UIBaseMenu::amountTracks] = {
        "desert.bmp", "speedway.bmp", "track.bmp", 
        "mount.bmp", "mines.bmp", "alpine.bmp", 
        "city.bmp", "underg.bmp", "stunt.bmp", 
        "luge.bmp", "fox2.bmp", "fox1.bmp"
    };

    //order based on mRemapCar
    const std::string controlCarsFiles[UIBaseMenu::amountTracks] = {
        "wart.bmp", "heave.bmp", "pickup.bmp",
        "sabre.bmp", "orc.bmp", "skeeto.bmp",
        "truck.bmp"
    };
}

UIBaseMenu::UIBaseMenu(const ModeContext& modeContext)
    : mModeContext(modeContext)
{
    memset(mControlClicked, 0, sizeof(bool) * mControlsCount);

    mRemapTrack.insert(std::make_pair<std::string, size_t>("desert track", 0));
    mRemapTrack.insert(std::make_pair<std::string, size_t>("speedway track", 1));
    mRemapTrack.insert(std::make_pair<std::string, size_t>("dam", 2));
    mRemapTrack.insert(std::make_pair<std::string, size_t>("mountain forest track", 3));
    mRemapTrack.insert(std::make_pair<std::string, size_t>("mineshaft", 4));
    mRemapTrack.insert(std::make_pair<std::string, size_t>("alpine track", 5));
    mRemapTrack.insert(std::make_pair<std::string, size_t>("city track", 6));
    mRemapTrack.insert(std::make_pair<std::string, size_t>("nutopia track", 7));
    mRemapTrack.insert(std::make_pair<std::string, size_t>("stunt track", 8));
    mRemapTrack.insert(std::make_pair<std::string, size_t>("luge track", 9));
    mRemapTrack.insert(std::make_pair<std::string, size_t>("Foxnhound2 track", 10));
    mRemapTrack.insert(std::make_pair<std::string, size_t>("Foxnhound1 track", 11));

    mRemapCar.insert(std::make_pair<std::string, size_t>("feral max", 0));
    mRemapCar.insert(std::make_pair<std::string, size_t>("mustang", 1));
    mRemapCar.insert(std::make_pair<std::string, size_t>("pickup", 2));
    mRemapCar.insert(std::make_pair<std::string, size_t>("exotic", 3));
    mRemapCar.insert(std::make_pair<std::string, size_t>("orc", 4));
    mRemapCar.insert(std::make_pair<std::string, size_t>("skeeto", 5));
    mRemapCar.insert(std::make_pair<std::string, size_t>("supercar", 6));
}

void UIBaseMenu::loadCommonTextures(const PFLoader& pfLoaderGameshell)
{
#if !defined(__ANDROID__)
    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "cursor.bmp", 
                                "OriginalCursor", TEMP_RESOURCE_GROUP_NAME, 
                                Ogre::ColourValue(0.0f, 1.0f, 0.0f), 0.2f, false, 64, true);
#endif

    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "single.bmp", 
                                "OriginalBackgroundSingle", TEMP_RESOURCE_GROUP_NAME,
                                Ogre::ColourValue(0.0f, 0.0f, 0.0f), 0.2f);

    for(size_t q = 0; q < amountTracks; ++q)
    {
        TextureLoader().loadChroma( pfLoaderGameshell, 
                                    "data/gameshell", controlTracksFiles[q], 
                                    "OriginalBackground_Textures_Controls_Track_" + Conversions::DMToString(q), TEMP_RESOURCE_GROUP_NAME,
                                    Ogre::ColourValue(0.0f, 0.0f, 0.0f), 0.2f);
    }

    for(size_t q = 0; q < amountCars; ++q)
    {
        TextureLoader().loadChroma( pfLoaderGameshell, 
                                    "data/gameshell", controlCarsFiles[q], 
                                    "OriginalBackground_Textures_Controls_Car_" + Conversions::DMToString(q), TEMP_RESOURCE_GROUP_NAME,
                                    Ogre::ColourValue(0.0f, 0.0f, 0.0f), 0.2f);
    }

    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "option.bmp", 
                                "OriginalBackgroundOption", TEMP_RESOURCE_GROUP_NAME,
                                Ogre::ColourValue(0.0f, 0.0f, 0.0f), 0.2f);

    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "race.bmp", 
                                "OriginalBackgroundRace", TEMP_RESOURCE_GROUP_NAME,
                                Ogre::ColourValue(0.0f, 0.0f, 0.0f), 0.2f);

    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "exit.bmp", 
                                "OriginalBackgroundExit", TEMP_RESOURCE_GROUP_NAME,
                                Ogre::ColourValue(0.0f, 0.0f, 0.0f), 0.2f);
}

void UIBaseMenu::createCommonMaterials()
{
#if !defined(__ANDROID__)
    //cursor
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalCursor");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/Cursor", 
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
#endif

    //explicitly load font
    Ogre::FontManager::getSingleton().getByName("SdkTrays/Caption")->load();

    //controls
    {
        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalBackgroundSingle");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BackgroundSingle", 
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

        for(size_t q = 0; q < amountTracks; ++q)
        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalBackground_Textures_Controls_Track_" + Conversions::DMToString(q));
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/Background_Controls_Track_" + Conversions::DMToString(q), 
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

        for(size_t q = 0; q < amountCars; ++q)
        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalBackground_Textures_Controls_Car_" + Conversions::DMToString(q));
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/Background_Controls_Car_" + Conversions::DMToString(q), 
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
            texName.push_back("OriginalBackgroundOption");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BackgroundOption", 
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
            texName.push_back("OriginalBackgroundRace");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BackgroundRace", 
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
            texName.push_back("OriginalBackgroundExit");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BackgroundExit", 
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
    }//controls
}

void UIBaseMenu::createControls(const Ogre::Matrix4& screenAdaptionRelative, Ogre::PanelOverlayElement* base)
{
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f;

    //controls
    {
        {
            const float elemWidth = 108.0f;
            Ogre::Vector4 backgroundMode = screenAdaptionRelative * Ogre::Vector4(1.0f, 392.0f, 1.0f + elemWidth, 392.0f + 83.0f);
            mControls[0] = createPanel("Mode", backgroundMode, "Test/BackgroundSingle");
            mControls[0]->setUV(0.0f, 0.0f, 1.0f, 0.25f);
            base->addChild(mControls[0]);
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(elemWidth / 2.0f, -15.0f, 0.0f, 0.0f);
            mControlsText[0] = createTextArea("ModeTextBox", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mControlsText[0]->setCaption("MODE");
            mControlsText[0]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mControlsText[0]->setSpaceWidth(9.0f);
            mControlsText[0]->setAlignment(Ogre::TextAreaOverlayElement::Center);
            mControlsText[0]->setFontName("SdkTrays/Caption");
            mControlsText[0]->setColour(Ogre::ColourValue::White);
            mControlsText[0]->hide();
            mControls[0]->addChild(mControlsText[0]);
        }


        {
            const float elemWidth = 94.0f;
            Ogre::Vector4 backgroundTrack = screenAdaptionRelative * Ogre::Vector4(116.0f, 392.0f, 116.0f + elemWidth, 392.0f + 83.0f);
            mControls[1] = createPanel("Track", backgroundTrack, "Test/Background_Controls_Track_0");
            mControls[1]->setUV(0.0f, 0.0f, 1.0f, 0.25f);
            base->addChild(mControls[1]);
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(elemWidth / 2.0f, -15.0f, 0.0f, 0.0f);
            mControlsText[1] = createTextArea("TracksTextBox", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mControlsText[1]->setCaption("TRACKS");
            mControlsText[1]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mControlsText[1]->setSpaceWidth(9.0f);
            mControlsText[1]->setAlignment(Ogre::TextAreaOverlayElement::Center);
            mControlsText[1]->setFontName("SdkTrays/Caption");
            mControlsText[1]->setColour(Ogre::ColourValue::White);
            mControlsText[1]->hide();
            mControls[1]->addChild(mControlsText[1]);
        }

        {
            const float elemWidth = 106.0f;
            Ogre::Vector4 backgroundCar = screenAdaptionRelative * Ogre::Vector4(212.0f, 392.0f, 212.0f + elemWidth, 392.0f + 83.0f);
            mControls[2]= createPanel("Car", backgroundCar, "Test/Background_Controls_Car_0");
            mControls[2]->setUV(0.0f, 0.0f, 1.0f, 0.25f);
            base->addChild(mControls[2]);
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(elemWidth / 2.0f, -15.0f, 0.0f, 0.0f);
            mControlsText[2] = createTextArea("CarsTextBox", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mControlsText[2]->setCaption("CARS");
            mControlsText[2]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mControlsText[2]->setSpaceWidth(9.0f);
            mControlsText[2]->setAlignment(Ogre::TextAreaOverlayElement::Center);
            mControlsText[2]->setFontName("SdkTrays/Caption");
            mControlsText[2]->setColour(Ogre::ColourValue::White);
            mControlsText[2]->hide();
            mControls[2]->addChild(mControlsText[2]);
        }

        {
            const float elemWidth = 74.0f;
            Ogre::Vector4 backgroundOption = screenAdaptionRelative * Ogre::Vector4(335.0f, 392.0f, 335.0f + elemWidth, 392.0f + 83.0f);
            mControls[3] = createPanel("Option", backgroundOption, "Test/BackgroundOption");
            mControls[3]->setUV(0.0f, 0.0f, 1.0f, 0.25f);
            base->addChild(mControls[3]);
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(elemWidth / 2.0f, -15.0f, 0.0f, 0.0f);
            mControlsText[3] = createTextArea("OptionTextBox", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mControlsText[3]->setCaption("OPTIONS");
            mControlsText[3]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mControlsText[3]->setSpaceWidth(9.0f);
            mControlsText[3]->setAlignment(Ogre::TextAreaOverlayElement::Center);
            mControlsText[3]->setFontName("SdkTrays/Caption");
            mControlsText[3]->setColour(Ogre::ColourValue::White);
            mControlsText[3]->hide();
            mControls[3]->addChild(mControlsText[3]);
        }

        {
            const float elemWidth = 81.0f;
            Ogre::Vector4 backgroundRace = screenAdaptionRelative * Ogre::Vector4(435.0f, 392.0f, 435.0f + elemWidth, 392.0f + 83.0f);
            mControls[4] = createPanel("Race", backgroundRace, "Test/BackgroundRace");
            mControls[4]->setUV(0.0f, 0.0f, 1.0f, 0.25f);
            base->addChild(mControls[4]);
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(elemWidth / 2.0f, -15.0f, 0.0f, 0.0f);
            mControlsText[4] = createTextArea("RaceTextBox", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mControlsText[4]->setCaption("RACE");
            mControlsText[4]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mControlsText[4]->setSpaceWidth(9.0f);
            mControlsText[4]->setAlignment(Ogre::TextAreaOverlayElement::Center);
            mControlsText[4]->setFontName("SdkTrays/Caption");
            mControlsText[4]->setColour(Ogre::ColourValue::White);
            mControlsText[4]->hide();
            mControls[4]->addChild(mControlsText[4]);
        }

        {
            const float elemWidth = 66.0f;
            Ogre::Vector4 backgroundExit = screenAdaptionRelative * Ogre::Vector4(542.0f, 392.0f, 542.0f + elemWidth, 392.0f + 83.0f);
            mControls[5] = createPanel("Exit", backgroundExit, "Test/BackgroundExit");
            mControls[5]->setUV(0.0f, 0.0f, 1.0f, 0.25f);
            base->addChild(mControls[5]);
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(elemWidth / 2.0f, -15.0f, 0.0f, 0.0f);
            mControlsText[5] = createTextArea("ExitTextBox", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mControlsText[5]->setCaption("EXIT");
            mControlsText[5]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mControlsText[5]->setSpaceWidth(9.0f);
            mControlsText[5]->setAlignment(Ogre::TextAreaOverlayElement::Center);
            mControlsText[5]->setFontName("SdkTrays/Caption");
            mControlsText[5]->setColour(Ogre::ColourValue::White);
            mControlsText[5]->hide();
            mControls[5]->addChild(mControlsText[5]);
        }
    }//controls
}

void UIBaseMenu::mousePressed(const Ogre::Vector2& pos)
{
    for(int q = 0; q < mControlsCount; ++q)
    {
        if(OgreBites::Widget::isCursorOver(mControls[q], pos, 0))
        {
            mControls[q]->setUV(0.0f, 0.5f, 1.0f, 0.75f);
            mControlClicked[q] = true;
        }
    }
}

void UIBaseMenu::mouseReleased(const Ogre::Vector2& pos)
{
    for(int q = 0; q < mControlsCount; ++q)
    {
        mControls[q]->setUV(0.0f, 0.0f, 1.0f, 0.25f);
        mControlClicked[q] = false;

        if(OgreBites::Widget::isCursorOver(mControls[q], pos, 0))
        {
            mControls[q]->setUV(0.0f, 0.25f, 1.0f, 0.5f);
            panelHit(mControls[q]);
        }
    }
}

void UIBaseMenu::mouseMoved(const Ogre::Vector2& pos)
{
    for(int q = 0; q < mControlsCount; ++q)
    {
        if(OgreBites::Widget::isCursorOver(mControls[q], pos, 0))
        {
            if(mControlClicked[q])
                mControls[q]->setUV(0.0f, 0.5f, 1.0f, 0.75f);
            else
                mControls[q]->setUV(0.0f, 0.25f, 1.0f, 0.5f);
            mControlsText[q]->show();
        }
        else
        {
            mControls[q]->setUV(0.0f, 0.0f, 1.0f, 0.25f);
            mControlsText[q]->hide();
        }
    }
}

void UIBaseMenu::selectTrack(const std::string& trackName)
{
    if(mRemapTrack.find(trackName) != mRemapTrack.end())
    {
        mControls[1]->setMaterialName("Test/Background_Controls_Track_" + Conversions::DMToString(mRemapTrack[trackName]));
    }
    else
    {
        assert(false);
    }
}

void UIBaseMenu::selectCar(const std::string& carName)
{
    if(mRemapCar.find(carName) != mRemapCar.end())
    {
        mControls[2]->setMaterialName("Test/Background_Controls_Car_" + Conversions::DMToString(mRemapCar[carName]));
    }
    else
    {
        assert(false);
    }
}