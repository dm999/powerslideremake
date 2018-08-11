
#include "UIMainMenuBackground.h"

#include "../customs/CustomTrayManager.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

#include "../listeners/LoaderListener.h"

#include "../loaders/TextureLoader.h"

#include "UIRace.h"

void UIMainMenuBackground::mouseMoved(const Ogre::Vector2& pos)
{
    UIBaseMenu::mouseMoved(pos);

    bool overFruit = false;
    std::string materialName;

    if(mBackgroundFruitApple->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitApple, pos, 0))
    {
        materialName = mBackgroundFruitApple->getMaterialName() + "+";
        overFruit = true;
    }

    if(mBackgroundFruitBanana->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitBanana, pos, 0))
    {
        materialName = mBackgroundFruitBanana->getMaterialName() + "+";
        overFruit = true;
    }

    if(mBackgroundFruitOrange->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitOrange, pos, 0))
    {
        materialName = mBackgroundFruitOrange->getMaterialName() + "+";
        overFruit = true;
    }

    if(mBackgroundFruitSberry->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitSberry, pos, 0))
    {
        materialName = mBackgroundFruitSberry->getMaterialName() + "+";
        overFruit = true;
    }

    if(mBackgroundFruitTangelo->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitTangelo, pos, 0))
    {
        materialName = mBackgroundFruitTangelo->getMaterialName() + "+";
        overFruit = true;
    }

    if(mBackgroundFruitPeach->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitPeach, pos, 0))
    {
        materialName = mBackgroundFruitPeach->getMaterialName() + "+";
        overFruit = true;
    }

    if(mBackgroundFruitPassion->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitPassion, pos, 0))
    {
        materialName = mBackgroundFruitPassion->getMaterialName() + "+";
        overFruit = true;
    }

    if(mBackgroundFruitCherries->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitCherries, pos, 0))
    {
        materialName = mBackgroundFruitCherries->getMaterialName() + "+";
        overFruit = true;
    }

    for(size_t q = 0; q < amountCars; ++q)
    {
        if(mBackgroundFruitPapaya[q]->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitPapaya[q], pos, 0))
        {
            materialName = "Test/Background_FruitPapaya+";
            overFruit = true;
            break;
        }

        if(mBackgroundFruitMango[q]->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitMango[q], pos, 0))
        {
            materialName = "Test/Background_FruitMango+";
            overFruit = true;
            break;
        }
    }

    for(size_t q = 0; q < mAmountTracksWithFruits; ++q)
    {
        if(mBackgroundFruitRberry[q]->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitRberry[q], pos, 0))
        {
            materialName = "Test/Background_FruitRberry+";
            overFruit = true;
            break;
        }
    }

    if(mBackgroundFruitBrussel->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitBrussel, pos, 0))
    {
        materialName = mBackgroundFruitBrussel->getMaterialName() + "+";
        overFruit = true;
    }

    if(overFruit)
    {
        mBackgroundFruitEnlarge->setMaterialName(materialName);
        mBackgroundFruitEnlarge->show();
    }
    else
    {
        mBackgroundFruitEnlarge->hide();
    }
}

void UIMainMenuBackground::mouseReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id)
{
    UIBaseMenu::mouseReleased(pos, id);

    if(mBackgroundFruitApple->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitApple, pos, 0))
    {
        playVideo(Championship::mWinnerFruitOffset + 0);
        return;
    }

    if(mBackgroundFruitBanana->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitBanana, pos, 0))
    {
        playVideo(Championship::mWinnerFruitOffset + 1);
        return;
    }

    if(mBackgroundFruitOrange->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitOrange, pos, 0))
    {
        playVideo(Championship::mWinnerFruitOffset + 2);
        return;
    }

    if(mBackgroundFruitSberry->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitSberry, pos, 0))
    {
        playVideo(Championship::mWinnerFruitOffset + 3);
        return;
    }

    if(mBackgroundFruitTangelo->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitTangelo, pos, 0))
    {
        playVideo(Championship::mEveryWinnerFruitOffset + 0);
        return;
    }

    if(mBackgroundFruitPeach->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitPeach, pos, 0))
    {
        playVideo(Championship::mEveryWinnerFruitOffset + 1);
        return;
    }

    if(mBackgroundFruitPassion->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitPassion, pos, 0))
    {
        playVideo(Championship::mEveryWinnerFruitOffset + 2);
        return;
    }

    if(mBackgroundFruitCherries->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitCherries, pos, 0))
    {
        playVideo(Championship::mEveryWinnerFruitOffset + 3);
        return;
    }

    for(size_t q = 0; q < amountCars; ++q)
    {
        if(mBackgroundFruitPapaya[q]->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitPapaya[q], pos, 0))
        {
            playVideo(Championship::mExpertCarFruitOffset + q);
            return;
        }
    }

    for(size_t q = 0; q < amountCars; ++q)
    {
        if(mBackgroundFruitMango[q]->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitMango[q], pos, 0))
        {
            playVideo(Championship::mInsaneCarFruitOffset + q);
            return;
        }
    }

    for(size_t q = 0; q < mAmountTracksWithFruits; ++q)
    {
        if(mBackgroundFruitRberry[q]->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitRberry[q], pos, 0))
        {
            playVideo(Championship::mBeatEmergentGunFruitOffset + q);
            return;
        }
    }

    if(mBackgroundFruitBrussel->isVisible() && OgreBites::Widget::isCursorOver(mBackgroundFruitBrussel, pos, 0))
    {
        playVideo(Championship::mBrusselFruitOffset + 0);
        return;
    }
}

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

    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "endchamp.bmp", 
                                "OriginalBackgroundEndChampionship", TEMP_RESOURCE_GROUP_NAME);

    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "fruit.bmp", 
                                "OriginalBackgroundTrophies", TEMP_RESOURCE_GROUP_NAME);

    if (!Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_NON_POWER_OF_2_TEXTURES))
    {
        TextureLoader().generate("VideoTexture", 512, 512, Ogre::ColourValue::Black, TEMP_RESOURCE_GROUP_NAME, Ogre::PF_R8G8B8);
    }
    else
    {
        TextureLoader().generate("VideoTexture", 640, 480, Ogre::ColourValue::Black, TEMP_RESOURCE_GROUP_NAME, Ogre::PF_R8G8B8);
    }


    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "name_bge.bmp", 
                                "OriginalBackgroundName", TEMP_RESOURCE_GROUP_NAME);

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
            Ogre::uint32 carIndex = 0;
            Ogre::uint32 charIndex = 0;
            sscanf(iconName.c_str(), "car%d_%ds.bmp", &carIndex, &charIndex);

            std::string carName = "car" + Conversions::DMToString(carIndex) + "_" + Conversions::DMToString(charIndex) + ".bmp";
            std::string charName = "char" + Conversions::DMToString(carIndex) + "_" + Conversions::DMToString(charIndex) + ".bmp";
            std::string charSmallName = "char" + Conversions::DMToString(carIndex) + "_" + Conversions::DMToString(charIndex) + "s.bmp";

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

            //supercar not different for small characters
            TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", charSmallName, 
                                "OriginalCharS" + Conversions::DMToString(carIndex) + "_" + Conversions::DMToString(charIndex), TEMP_RESOURCE_GROUP_NAME);

            if(loaderListener && q % 2)
                loaderListener->loadState(0.4f + 0.6f * (static_cast<float>(q + 1) / availableCharacters.size()), availableCharacters[q] + " loaded");
        }

    }

    //fruits
    {
        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "apple.bmp", 
                                "OriginalFruitApple", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "apple+.bmp", 
                                "OriginalFruitApple+", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "banana.bmp", 
                                "OriginalFruitBanana", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "banana+.bmp", 
                                "OriginalFruitBanana+", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "orange.bmp", 
                                "OriginalFruitOrange", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "orange+.bmp", 
                                "OriginalFruitOrange+", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "sberry.bmp", 
                                "OriginalFruitSberry", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "sberry+.bmp", 
                                "OriginalFruitSberry+", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "tangelo.bmp", 
                                "OriginalFruitTangelo", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "tangelo+.bmp", 
                                "OriginalFruitTangelo+", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "peach.bmp", 
                                "OriginalFruitPeach", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "peach+.bmp", 
                                "OriginalFruitPeach+", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "passion.bmp", 
                                "OriginalFruitPassion", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "passion+.bmp", 
                                "OriginalFruitPassion+", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "cherries.bmp", 
                                "OriginalFruitCherries", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "cherry+.bmp", 
                                "OriginalFruitCherries+", TEMP_RESOURCE_GROUP_NAME);

        for(size_t q = 0; q < amountCars; ++q)
        {
            TextureLoader().load( pfLoaderGameshell, 
                                    "data/gameshell", "papaya" + Conversions::DMToString(q) + ".bmp", 
                                    "OriginalFruitPapaya" + Conversions::DMToString(q), TEMP_RESOURCE_GROUP_NAME);
        }

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "papaya+.bmp", 
                                "OriginalFruitPapaya+", TEMP_RESOURCE_GROUP_NAME);

        for(size_t q = 0; q < amountCars; ++q)
        {
            TextureLoader().load( pfLoaderGameshell, 
                                    "data/gameshell", "mango" + Conversions::DMToString(q) + ".bmp", 
                                    "OriginalFruitMango" + Conversions::DMToString(q), TEMP_RESOURCE_GROUP_NAME);
        }

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "mango+.bmp", 
                                "OriginalFruitMango+", TEMP_RESOURCE_GROUP_NAME);

        for(size_t q = 0; q < mAmountTracksWithFruits; ++q)
        {
            TextureLoader().load( pfLoaderGameshell, 
                                    "data/gameshell", "rberry" + Conversions::DMToString(q) + ".bmp", 
                                    "OriginalFruitRberry" + Conversions::DMToString(q), TEMP_RESOURCE_GROUP_NAME);
        }

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "rberry+.bmp", 
                                "OriginalFruitRberry+", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "brussel.bmp", 
                                "OriginalFruitBrussel", TEMP_RESOURCE_GROUP_NAME);

        TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "brussel+.bmp", 
                                "OriginalFruitBrussel+", TEMP_RESOURCE_GROUP_NAME);
    }

    UIRace::loadDashboardCars(mModeContext.getGameState());

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        TextureLoader().load( pfLoaderGameshell, 
        "data/gameshell", "grid" + Conversions::DMToString(q) +".bmp", 
        "OriginalGrid" + Conversions::DMToString(q), TEMP_RESOURCE_GROUP_NAME);
    }

    TextureLoader().load( pfLoaderGameshell, 
        "data/gameshell", "exitsign.bmp", 
        "OriginalExitSign", TEMP_RESOURCE_GROUP_NAME);

    TextureLoader().load( pfLoaderGameshell, 
        "data/gameshell", "1st.bmp", 
        "Original1st", TEMP_RESOURCE_GROUP_NAME);

    TextureLoader().load( pfLoaderGameshell, 
        "data/gameshell", "2nd.bmp", 
        "Original2nd", TEMP_RESOURCE_GROUP_NAME);

    TextureLoader().load( pfLoaderGameshell, 
        "data/gameshell", "3rd.bmp", 
        "Original3rd", TEMP_RESOURCE_GROUP_NAME);

}

void UIMainMenuBackground::createBackgroundMaterials()
{

    //main background
    {
        clone2DMaterial("OriginalMainBackground", "Test/MainBackground");
    }

    //additional background (feryl)
    {
        clone2DMaterial("OriginalBackgroundA", "Test/BackgroundA");
        clone2DMaterial("OriginalBackgroundB", "Test/BackgroundB");
    }

    //championship finish background
    {
        clone2DMaterial("OriginalBackgroundEndChampionship", "Test/EndChampionshipBackground");
    }

    //trophies background
    {
        clone2DMaterial("OriginalBackgroundTrophies", "Test/TrophiesBackground");
    }

    //video background
    {
        clone2DMaterial("VideoTexture", "Test/VideoTexture");
    }

    //name background
    {
        clone2DMaterial("OriginalBackgroundName", "Test/NameBackground");
    }

    for(size_t q = 0; q < amountTracks; ++q)
    {
        clone2DMaterial("OriginalTrack" + Conversions::DMToString(q), "Test/Background_Track_" + Conversions::DMToString(q));
    }

    for(size_t q = 0; q < amountCars; ++q)
    {
        clone2DMaterial("OriginalCar" + Conversions::DMToString(q), "Test/Background_Car_" + Conversions::DMToString(q));
        clone2DMaterial("OriginalCarS" + Conversions::DMToString(q), "Test/Background_CarS_" + Conversions::DMToString(q));
    }

    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
    for (size_t q = 0; q < availableCharacters.size(); ++q)
    {
        std::string iconName = mModeContext.getGameState().getSTRPowerslide().getValue(availableCharacters[q] + " parameters", "icon", "car0_0s.bmp");
        Ogre::uint32 carIndex = 0;
        Ogre::uint32 charIndex = 0;
        sscanf(iconName.c_str(), "car%d_%ds.bmp", &carIndex, &charIndex);

        {
            clone2DMaterial("OriginalChar" + Conversions::DMToString(carIndex) + "_" + Conversions::DMToString(charIndex), "Test/Background_Character_" + Conversions::DMToString(q));
            clone2DMaterial("OriginalCharS" + Conversions::DMToString(carIndex) + "_" + Conversions::DMToString(charIndex), "Test/Background_Character_Small_" + Conversions::DMToString(q));
        }

        //small icons of cars
        {
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
        }
    }

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        clone2DMaterial("OriginalGrid" + Conversions::DMToString(q), "Test/Background_Grid_" + Conversions::DMToString(q));
    }

    //fruits
    {
        clone2DMaterial("OriginalFruitApple", "Test/Background_FruitApple");
        clone2DMaterial("OriginalFruitApple+", "Test/Background_FruitApple+");

        clone2DMaterial("OriginalFruitBanana", "Test/Background_FruitBanana");
        clone2DMaterial("OriginalFruitBanana+", "Test/Background_FruitBanana+");

        clone2DMaterial("OriginalFruitOrange", "Test/Background_FruitOrange");
        clone2DMaterial("OriginalFruitOrange+", "Test/Background_FruitOrange+");

        clone2DMaterial("OriginalFruitSberry", "Test/Background_FruitSberry");
        clone2DMaterial("OriginalFruitSberry+", "Test/Background_FruitSberry+");

        clone2DMaterial("OriginalFruitTangelo", "Test/Background_FruitTangelo");
        clone2DMaterial("OriginalFruitTangelo+", "Test/Background_FruitTangelo+");

        clone2DMaterial("OriginalFruitPeach", "Test/Background_FruitPeach");
        clone2DMaterial("OriginalFruitPeach+", "Test/Background_FruitPeach+");

        clone2DMaterial("OriginalFruitPassion", "Test/Background_FruitPassion");
        clone2DMaterial("OriginalFruitPassion+", "Test/Background_FruitPassion+");

        clone2DMaterial("OriginalFruitCherries", "Test/Background_FruitCherries");
        clone2DMaterial("OriginalFruitCherries+", "Test/Background_FruitCherries+");

        for(size_t q = 0; q < amountCars; ++q)
        {
            clone2DMaterial("OriginalFruitPapaya" + Conversions::DMToString(q), "Test/Background_FruitPapaya" + Conversions::DMToString(q));
        }
        clone2DMaterial("OriginalFruitPapaya+", "Test/Background_FruitPapaya+");

        for(size_t q = 0; q < amountCars; ++q)
        {
            clone2DMaterial("OriginalFruitMango" + Conversions::DMToString(q), "Test/Background_FruitMango" + Conversions::DMToString(q));
        }
        clone2DMaterial("OriginalFruitMango+", "Test/Background_FruitMango+");

        for(size_t q = 0; q < mAmountTracksWithFruits; ++q)
        {
            clone2DMaterial("OriginalFruitRberry" + Conversions::DMToString(q), "Test/Background_FruitRberry" + Conversions::DMToString(q));
        }
        clone2DMaterial("OriginalFruitRberry+", "Test/Background_FruitRberry+");

        clone2DMaterial("OriginalFruitBrussel", "Test/Background_FruitBrussel");
        clone2DMaterial("OriginalFruitBrussel+", "Test/Background_FruitBrussel+");
    }

    {
        clone2DMaterial("OriginalExitSign", "Test/Background_ExitSign");
        clone2DMaterial("Original1st", "Test/Background_1st");
        clone2DMaterial("Original2nd", "Test/Background_2nd");
        clone2DMaterial("Original3rd", "Test/Background_3rd");
    }
}

void UIMainMenuBackground::clone2DMaterial(const std::string& textureName, const std::string& newMaterialName) const
{
        std::vector<Ogre::String> texName;
        texName.push_back(textureName);
        Ogre::MaterialPtr newMat = CloneMaterial( newMaterialName, 
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
        Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(169.0f, 219.0f, 169.0f + 119.0f, 219.0f + 136.0f);

        mBackgroundCarS = createPanel("BackgroundCarS", background, "Test/Background_CarS_0");
        mBackgroundCarS->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackgroundCarS);
    }

    {
        Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(0.0f, 0.0f, 309.0f, 358.0f);

        mBackgroundCharacter = createPanel("BackgroundCharacter", background, "Test/Background_Character_0");
        mBackgroundCharacter->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackgroundCharacter);
    }

    for(size_t q = 0, u = 0, l = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        Ogre::Vector4 background;

        if(q % 2 == 0)
        {
            background = screenAdaptionRelative * Ogre::Vector4(u * 100.0f, 59.0f, u * 100.0f + 83.0f, 59.0f + 130.0f);
            ++u;
        }
        else
        {
            background = screenAdaptionRelative * Ogre::Vector4(l * 100.0f + 40.0f, 209.0f, l * 100.0f + 40.0f + 83.0f, 209.0f + 130.0f);
            l++;
        }

        mBackgroundCharacterSmall[q] = createPanel("BackgroundCharacter_" + Conversions::DMToString(q), background, "Test/Background_Character_Small_0");
        mBackgroundCharacterSmall[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackgroundCharacterSmall[q]);
    }

    {
        Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(20.0f, 45.0f, 20.0f + 290.0f, 45.0f + 322.0f);

        mBackgroundExitSign = createPanel("BackgroundExitSign", background, "Test/Background_ExitSign");
        mBackgroundExitSign->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackgroundExitSign);
    }

    {
        Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(17.0f, 29.0f, 17.0f + 96.0f, 29.0f + 80.0f);

        mBackground1st = createPanel("Background1st", background, "Test/Background_1st");
        mBackground1st->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackground1st);
    }

    {
        Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(205.0f, 100.0f, 205.0f + 78.0f, 100.0f + 76.0f);

        mBackground2nd = createPanel("Background2nd", background, "Test/Background_2nd");
        mBackground2nd->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackground2nd);
    }

    {
        Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(52.0f, 216.0f, 52.0f + 63.0f, 216.0f + 65.0f);

        mBackground3rd = createPanel("Background3rd", background, "Test/Background_3rd");
        mBackground3rd->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mMainBackground->addChild(mBackground3rd);
    }

    for(size_t q = 0; q < mPodiumCharacters; ++q)
    {
        Ogre::Vector4 background;
        
        if(q == 0)
            background = screenAdaptionRelative * Ogre::Vector4(57.0f, 21.0f, 57.0f + 83.0f, 21.0f + 130.0f);
        if(q == 1)
            background = screenAdaptionRelative * Ogre::Vector4(-15.0f, 53.0f, -15.0f + 83.0f, 53.0f + 130.0f);
        if(q == 2)
            background = screenAdaptionRelative * Ogre::Vector4(48.0f, -16.0f, 48.0f + 83.0f, -16.0f + 130.0f);

        mBackgroundCharacterSmallPodium[q] = createPanel("BackgroundCharacterPodium_" + Conversions::DMToString(q), background, "Test/Background_Character_Small_0");
        mBackgroundCharacterSmallPodium[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);

        if(q == 0)
            mBackground1st->addChild(mBackgroundCharacterSmallPodium[q]);
        if(q == 1)
            mBackground2nd->addChild(mBackgroundCharacterSmallPodium[q]);
        if(q == 2)
            mBackground3rd->addChild(mBackgroundCharacterSmallPodium[q]);
    }

    //fruits
    {
        {
            //0x004D5AA0
            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(71.0f, 158.0f, 71.0f + 31.0f, 158.0f + 34.0f);

            mBackgroundFruitApple = createPanel("Background_FruitApple", background, "Test/Background_FruitApple");
            mBackgroundFruitApple->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitApple);
        }

        {
            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(105.0f, 148.0f, 105.0f + 43.0f, 148.0f + 44.0f);

            mBackgroundFruitBanana = createPanel("Background_FruitBanana", background, "Test/Background_FruitBanana");
            mBackgroundFruitBanana->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitBanana);
        }

        {
            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(150.0f, 156.0f, 150.0f + 31.0f, 156.0f + 40.0f);

            mBackgroundFruitOrange = createPanel("Background_FruitOrange", background, "Test/Background_FruitOrange");
            mBackgroundFruitOrange->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitOrange);
        }

        {
            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(185.0f, 170.0f, 185.0f + 17.0f, 170.0f + 22.0f);

            mBackgroundFruitSberry = createPanel("Background_FruitSberry", background, "Test/Background_FruitSberry");
            mBackgroundFruitSberry->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitSberry);
        }

        {
            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(69.0f, 260.0f, 69.0f + 21.0f, 260.0f + 23.0f);

            mBackgroundFruitTangelo = createPanel("Background_FruitTangelo", background, "Test/Background_FruitTangelo");
            mBackgroundFruitTangelo->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitTangelo);
        }

        {
            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(97.0f, 248.0f, 97.0f + 36.0f, 248.0f + 34.0f);

            mBackgroundFruitPeach = createPanel("Background_FruitPeach", background, "Test/Background_FruitPeach");
            mBackgroundFruitPeach->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitPeach);
        }

        {
            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(181.0f, 259.0f, 181.0f + 16.0f, 259.0f + 22.0f);

            mBackgroundFruitPassion = createPanel("Background_FruitPassion", background, "Test/Background_FruitPassion");
            mBackgroundFruitPassion->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitPassion);
        }

        {
            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(146.0f, 247.0f, 146.0f + 26.0f, 247.0f + 36.0f);

            mBackgroundFruitCherries = createPanel("Background_FruitCherries", background, "Test/Background_FruitCherries");
            mBackgroundFruitCherries->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitCherries);
        }

        for(size_t q = 0; q < amountCars; ++q)
        {
            Ogre::Vector2 leftPoint;
            Ogre::Vector2 size(0.0f, 0.0f);

            switch(q)
            {
            case 0:
                leftPoint = Ogre::Vector2(261.0f, 281.0f);
                break;
            case 1:
                leftPoint = Ogre::Vector2(281.0f, 281.0f);
                break;
            case 2:
                leftPoint = Ogre::Vector2(302.0f, 281.0f);
                break;
            case 3:
                leftPoint = Ogre::Vector2(327.0f, 279.0f);
                break;
            case 4:
                leftPoint = Ogre::Vector2(346.0f, 278.0f);
                break;
            case 5:
                leftPoint = Ogre::Vector2(369.0f, 276.0f);
                break;
            case 6:
                leftPoint = Ogre::Vector2(344.0f, 323.0f);
                break;
            }

            if(q < 4)
                size = Ogre::Vector2(16.0f, 51.0f);
            if(q >= 4 && q < 6)
                size = Ogre::Vector2(18.0f, 45.0f);
            if(q >= 6)
                size = Ogre::Vector2(27.0f, 23.0f);

            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(leftPoint.x, leftPoint.y, leftPoint.x + size.x, leftPoint.y + size.y);

            mBackgroundFruitPapaya[q] = createPanel("Background_FruitPapaya" + Conversions::DMToString(q), background, "Test/Background_FruitPapaya" + Conversions::DMToString(q));
            mBackgroundFruitPapaya[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitPapaya[q]);
        }

        for(size_t q = 0; q < amountCars; ++q)
        {
            Ogre::Vector2 leftPoint;
            Ogre::Vector2 size(0.0f, 0.0f);

            switch(q)
            {
            case 0:
                leftPoint = Ogre::Vector2(259.0f, 205.0f);
                break;
            case 1:
                leftPoint = Ogre::Vector2(281.0f, 202.0f);
                break;
            case 2:
                leftPoint = Ogre::Vector2(300.0f, 204.0f);
                break;
            case 3:
                leftPoint = Ogre::Vector2(319.0f, 201.0f);
                break;
            case 4:
                leftPoint = Ogre::Vector2(339.0f, 200.0f);
                break;
            case 5:
                leftPoint = Ogre::Vector2(353.0f, 201.0f);
                break;
            case 6:
                leftPoint = Ogre::Vector2(374.0f, 197.0f);
                break;
            }

            if(q == 0)
                size = Ogre::Vector2(22.0f, 43.0f);
            if(q >= 1 && q < 4)
                size = Ogre::Vector2(18.0f, 43.0f);
            if(q == 4)
                size = Ogre::Vector2(14.0f, 43.0f);
            if(q == 5)
                size = Ogre::Vector2(17.0f, 43.0f);
            if(q == 6)
                size = Ogre::Vector2(13.0f, 32.0f);

            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(leftPoint.x, leftPoint.y, leftPoint.x + size.x, leftPoint.y + size.y);

            mBackgroundFruitMango[q] = createPanel("Background_FruitMango" + Conversions::DMToString(q), background, "Test/Background_FruitMango" + Conversions::DMToString(q));
            mBackgroundFruitMango[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitMango[q]);
        }

        for(size_t q = 0; q < mAmountTracksWithFruits; ++q)
        {
            Ogre::Vector2 leftPoint;
            Ogre::Vector2 size(0.0f, 0.0f);

            switch(q)
            {
            case 0:
                leftPoint = Ogre::Vector2(255.0f, 167.0f);
                break;
            case 1:
                leftPoint = Ogre::Vector2(273.0f, 167.0f);
                break;
            case 2:
                leftPoint = Ogre::Vector2(289.0f, 164.0f);
                break;
            case 3:
                leftPoint = Ogre::Vector2(308.0f, 162.0f);
                break;
            case 4:
                leftPoint = Ogre::Vector2(324.0f, 162.0f);
                break;
            case 5:
                leftPoint = Ogre::Vector2(339.0f, 162.0f);
                break;
            case 6:
                leftPoint = Ogre::Vector2(355.0f, 157.0f);
                break;
            case 7:
                leftPoint = Ogre::Vector2(372.0f, 154.0f);
                break;
            }

            if(q == 0)
                size = Ogre::Vector2(19.0f, 34.0f);
            if(q >= 1 && q < 6)
                size = Ogre::Vector2(16.0f, 34.0f);
            if(q >= 6 && q < 8)
                size = Ogre::Vector2(14.0f, 34.0f);

            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(leftPoint.x, leftPoint.y, leftPoint.x + size.x, leftPoint.y + size.y);

            mBackgroundFruitRberry[q] = createPanel("Background_FruitRberry" + Conversions::DMToString(q), background, "Test/Background_FruitRberry" + Conversions::DMToString(q));
            mBackgroundFruitRberry[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitRberry[q]);
        }

        {
            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(150.0f, 335.0f, 150.0f + 17.0f, 335.0f + 18.0f);

            mBackgroundFruitBrussel = createPanel("Background_FruitBrussel", background, "Test/Background_FruitBrussel");
            mBackgroundFruitBrussel->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitBrussel);
        }

        {
            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(442.0f, 45.0f, 442.0f + 195.0f, 45.0f + 285.0f);

            mBackgroundFruitEnlarge = createPanel("Background_FruitEnlarge", background, "Test/Background_FruitBrussel+");
            mBackgroundFruitEnlarge->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            mMainBackground->addChild(mBackgroundFruitEnlarge);
        }
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
    mBackgroundCarS->setMaterialName("Test/Background_CarS_" + Conversions::DMToString(index));
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

void UIMainMenuBackground::showBackgroundCharacterSmall()
{

    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");

    std::vector<std::string> aiNames;
    for(size_t q = 0; q < mModeContext.getGameState().getAICountInRace(); ++q)
    {
        aiNames.push_back(mModeContext.getGameState().getAICar(q).getCharacterName());
    }

    for(size_t q = 0; q < mModeContext.getGameState().getAICountInRace() + 1; ++q)
    {
        if(q < aiNames.size())
        {
            std::vector<std::string>::const_iterator i = std::find(availableCharacters.begin(), availableCharacters.end(), aiNames[q]);
            size_t charIndex = i - availableCharacters.begin();
            mBackgroundCharacterSmall[q]->setMaterialName("Test/Background_Character_Small_" + Conversions::DMToString(charIndex));
        }
        else
        {
            std::vector<std::string>::const_iterator i = std::find(availableCharacters.begin(), availableCharacters.end(), mModeContext.getGameState().getPlayerCar().getCharacterName());
            size_t charIndex = i - availableCharacters.begin();
            mBackgroundCharacterSmall[q]->setMaterialName("Test/Background_Character_Small_" + Conversions::DMToString(charIndex));
        }

        mBackgroundCharacterSmall[q]->show();
    }

    for(size_t q = mModeContext.getGameState().getAICountInRace() + 1; q < GameState::mRaceGridCarsMax; ++q)
    {
        mBackgroundCharacterSmall[q]->setMaterialName("Test/Background_Grid_" + Conversions::DMToString(q));
        mBackgroundCharacterSmall[q]->show();
    }
}

void UIMainMenuBackground::showBackgroundCharacterSmallPodium(const finishBoardVec& finishBoard)
{
    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");

    size_t podiumChars = std::min(finishBoard.size(), mPodiumCharacters);

    if(podiumChars == 1)
    {
        mBackground2nd->hide();
        mBackground3rd->hide();
    }

    if(podiumChars == 2)
        mBackground3rd->hide();

    for(size_t q = 0; q < podiumChars; ++q)
    {
        std::vector<std::string>::const_iterator i = std::find(availableCharacters.begin(), availableCharacters.end(), finishBoard[q].mCharName);
        size_t charIndex = i - availableCharacters.begin();
        mBackgroundCharacterSmallPodium[q]->setMaterialName("Test/Background_Character_Small_" + Conversions::DMToString(charIndex));

        mBackgroundCharacterSmallPodium[q]->show();
    }
}

void UIMainMenuBackground::showBackgroundFruis(const STRPlayerSettings::PlayerData& playerData)
{
    const std::vector<bool>& fruits = playerData.fruit;

    if(fruits[Championship::mWinnerFruitOffset + 0])
        mBackgroundFruitApple->show();

    if(fruits[Championship::mWinnerFruitOffset + 1])
        mBackgroundFruitBanana->show();

    if(fruits[Championship::mWinnerFruitOffset + 2])
        mBackgroundFruitOrange->show();

    if(fruits[Championship::mWinnerFruitOffset + 3])
        mBackgroundFruitSberry->show();

    if(fruits[Championship::mEveryWinnerFruitOffset + 0])
        mBackgroundFruitTangelo->show();

    if(fruits[Championship::mEveryWinnerFruitOffset + 1])
        mBackgroundFruitPeach->show();

    if(fruits[Championship::mEveryWinnerFruitOffset + 2])
        mBackgroundFruitPassion->show();

    if(fruits[Championship::mEveryWinnerFruitOffset + 3])
        mBackgroundFruitCherries->show();

    for(size_t q = 0; q < amountCars; ++q)
    {
        if(fruits[Championship::mExpertCarFruitOffset + q])
            mBackgroundFruitPapaya[q]->show();
    }

    for(size_t q = 0; q < amountCars; ++q)
    {
        if(fruits[Championship::mInsaneCarFruitOffset + q])
            mBackgroundFruitMango[q]->show();
    }

    for(size_t q = 0; q < mAmountTracksWithFruits; ++q)
    {
        if(fruits[Championship::mBeatEmergentGunFruitOffset + q])
            mBackgroundFruitRberry[q]->show();
    }

    if(fruits[Championship::mBrusselFruitOffset + 0])
        mBackgroundFruitBrussel->show();

}

void UIMainMenuBackground::hideAllBackgrounds()
{
    mBackgroundA->hide(); 
    mBackgroundB->hide();

    mBackgroundTrack->hide();
    mBackgroundCar->hide();
    mBackgroundCarS->hide();
    mBackgroundCharacter->hide();

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        mBackgroundCharacterSmall[q]->hide();
    }

    mBackgroundExitSign->hide();

    mBackground1st->hide();
    mBackground2nd->hide();
    mBackground3rd->hide();

    for(size_t q = 0; q < mPodiumCharacters; ++q)
    {
        mBackgroundCharacterSmallPodium[q]->hide();
    }

    mBackgroundFruitApple->hide();
    mBackgroundFruitBanana->hide();
    mBackgroundFruitOrange->hide();
    mBackgroundFruitSberry->hide();
    mBackgroundFruitTangelo->hide();
    mBackgroundFruitPeach->hide();
    mBackgroundFruitPassion->hide();
    mBackgroundFruitCherries->hide();
    for(size_t q = 0; q < amountCars; ++q)
    {
        mBackgroundFruitPapaya[q]->hide();
        mBackgroundFruitMango[q]->hide();
    }
    for(size_t q = 0; q < mAmountTracksWithFruits; ++q)
    {
        mBackgroundFruitRberry[q]->hide();
    }
    mBackgroundFruitBrussel->hide();
    mBackgroundFruitEnlarge->hide();
}