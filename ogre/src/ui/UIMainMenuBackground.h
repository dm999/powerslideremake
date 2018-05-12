#ifndef UIMAINMENUBACKGROUND_H
#define UIMAINMENUBACKGROUND_H

#include "UIBaseMenu.h"

#include "../gamelogic/FinishBoard.h"

class LoaderListener;

class UIMainMenuBackground : public UIBaseMenu
{
public:
    UIMainMenuBackground(const ModeContext& modeContext, const GameMode gameMode) : UIBaseMenu(modeContext, gameMode){}
    ~UIMainMenuBackground(){}

protected:

    void mouseMoved(const Ogre::Vector2& pos) override;

    void createBackgroundTextures(const PFLoader& pfLoaderGameshell, LoaderListener* loaderListener);
    void createBackgroundMaterials();
    void createBackgroundUI(const Ogre::Matrix4& screenAdaptionRelative, CustomTrayManager* trayMgr);

    void setCurrentTrackLogo();
    void setTrackLogo(size_t index);
    void setCurrentCarLogo();
    void setCarLogo(size_t index);
    void setCharacterLogo(size_t index);

    void selectTrack(const std::string& trackName);
    void selectCar(const std::string& carName);

    Ogre::PanelOverlayElement* getMainBackground(){return mMainBackground;}

    void showBackgrounds(){mBackgroundA->show(); mBackgroundB->show();}
    void showBackgroundTrack(){mBackgroundTrack->show();}
    void showBackgroundCar(){mBackgroundCar->show();}
    void showBackgroundCharacter();
    void showBackgroundCharacterSmall();
    void showBackgroundExitSign(){mBackgroundExitSign->show();}
    void showBackgroundFirstSecondThird(){mBackground1st->show();mBackground2nd->show();mBackground3rd->show();}
    void showBackgroundCharacterSmallPodium(const finishBoardVec& finishBoard);
    void showBackgroundFruis(const STRPlayerSettings::PlayerData& playerData);
    void hideAllBackgrounds();
    void hideBackgroundTrack(){mBackgroundTrack->hide();}
    void hideBackgroundCharacter(){mBackgroundCharacter->hide();}

    void setMainBackgroundMaterial(const std::string& material){mMainBackground->setMaterialName(material);}

    static const size_t mAmountTracksWithFruits = 8;

private:

    void clone2DMaterial(const std::string& textureName, const std::string& newMaterialName) const;

    Ogre::PanelOverlayElement* mMainBackground;

    Ogre::PanelOverlayElement* mBackgroundA;
    Ogre::PanelOverlayElement* mBackgroundB;

    Ogre::PanelOverlayElement* mBackgroundTrack;
    Ogre::PanelOverlayElement* mBackgroundCar;
    Ogre::PanelOverlayElement* mBackgroundCharacter;

    Ogre::PanelOverlayElement* mBackgroundCharacterSmall[GameState::mRaceGridCarsMax];

    Ogre::PanelOverlayElement* mBackgroundExitSign;

    Ogre::PanelOverlayElement* mBackground1st;
    Ogre::PanelOverlayElement* mBackground2nd;
    Ogre::PanelOverlayElement* mBackground3rd;

    Ogre::PanelOverlayElement* mBackgroundCharacterSmallPodium[mPodiumCharacters];

    Ogre::PanelOverlayElement* mBackgroundFruitApple;
    Ogre::PanelOverlayElement* mBackgroundFruitBanana;
    Ogre::PanelOverlayElement* mBackgroundFruitOrange;
    Ogre::PanelOverlayElement* mBackgroundFruitSberry;
    Ogre::PanelOverlayElement* mBackgroundFruitTangelo;
    Ogre::PanelOverlayElement* mBackgroundFruitPeach;
    Ogre::PanelOverlayElement* mBackgroundFruitPassion;
    Ogre::PanelOverlayElement* mBackgroundFruitCherries;
    Ogre::PanelOverlayElement* mBackgroundFruitPapaya[amountCars];
    Ogre::PanelOverlayElement* mBackgroundFruitMango[amountCars];
    Ogre::PanelOverlayElement* mBackgroundFruitRberry[mAmountTracksWithFruits];
    Ogre::PanelOverlayElement* mBackgroundFruitBrussel;
    Ogre::PanelOverlayElement* mBackgroundFruitEnlarge;

};

#endif