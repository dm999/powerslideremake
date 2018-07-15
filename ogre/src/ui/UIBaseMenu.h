#ifndef UIBASEMENU_H
#define UIBASEMENU_H

#include "UIBase.h"

#include "../gamemodes/ModeContext.h"

class PFLoader;

namespace Ogre{
    class PanelOverlayElement;
}

class UIBaseMenu : public UIBase
{
public:
    UIBaseMenu(const ModeContext& modeContext, const GameMode gameMode);
    virtual ~UIBaseMenu(){}

    virtual void frameStarted(const Ogre::FrameEvent &evt) = 0;

    static const size_t amountTracks = 12;
    static const size_t amountCars = 7;

protected:

    void selectMode();
    void selectTrack(const std::string& trackName);
    void selectCar(const std::string& carName);

    void loadCommonTextures(const PFLoader& pfLoaderGameshell);
    void createCommonMaterials();
    void createControls(const Ogre::Matrix4& screenAdaptionRelative, Ogre::PanelOverlayElement* base);

    virtual void mousePressed(const Ogre::Vector2& pos);
    virtual void mouseReleased(const Ogre::Vector2& pos);
    virtual void mouseMoved(const Ogre::Vector2& pos);

    virtual void panelHit(Ogre::PanelOverlayElement* panel) = 0;

    void setControlActive(size_t index, bool isActive);
    void setControlShow(size_t index, bool isShow);

    static const int mControlsCount = 6;
    Ogre::PanelOverlayElement* mControls[mControlsCount];
    Ogre::TextAreaOverlayElement * mControlsText[mControlsCount];

    std::map<std::string, size_t> mRemapTrack;
    std::map<std::string, size_t> mRemapCar;

    ModeContext mModeContext;

    static const size_t mPodiumCharacters = 3;

    GameMode mGameModeSelected;//ModeMenu, ModeMenuChampionship, ModeMenuTimetrial

private:

    void loadColorTextures();
    void loadUITextures(const PFLoader& pfLoaderGameshell);
    void createColorMaterials();

    bool mControlActivated[mControlsCount];
    bool mControlClicked[mControlsCount];
    bool mControlOver[mControlsCount];
};

#endif