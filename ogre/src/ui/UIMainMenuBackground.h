#ifndef UIMAINMENUBACKGROUND_H
#define UIMAINMENUBACKGROUND_H

#include "UIBaseMenu.h"

class LoaderListener;

class UIMainMenuBackground : public UIBaseMenu
{
public:
    UIMainMenuBackground(const ModeContext& modeContext) : UIBaseMenu(modeContext){}
    ~UIMainMenuBackground(){}

protected:

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
    void hideAllBackgrounds();

private:

    Ogre::PanelOverlayElement* mMainBackground;

    Ogre::PanelOverlayElement* mBackgroundA;
    Ogre::PanelOverlayElement* mBackgroundB;

    Ogre::PanelOverlayElement* mBackgroundTrack;
    Ogre::PanelOverlayElement* mBackgroundCar;
    Ogre::PanelOverlayElement* mBackgroundCharacter;

};

#endif