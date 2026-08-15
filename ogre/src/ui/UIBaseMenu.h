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
    virtual void mouseReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id);
    virtual void mouseMoved(const Ogre::Vector2& pos);

    virtual void panelHit(Ogre::PanelOverlayElement* panel) = 0;

    void setControlActive(size_t index, bool isActive);
    void setControlShow(size_t index, bool isShow);

    //apply one of the four vertical UV state-rows (normal/pressed/hover/active)
    //to a control, honoring that control's horizontal-mirror flag. Centralised
    //so every state transition (mouse move, press, activate, selectMode) keeps a
    //mirrored control mirrored instead of resetting U to 0..1.
    void setControlUVState(size_t index, Ogre::Real v1, Ogre::Real v2);

    static const int mControlsCount = 6;
    Ogre::PanelOverlayElement* mControls[mControlsCount];
    Ogre::TextAreaOverlayElement * mControlsText[mControlsCount];

    std::map<std::string, size_t> mRemapTrack;
    std::map<std::string, size_t> mRemapCar;

    ModeContext mModeContext;

    static const size_t mPodiumCharacters = 3;

    GameMode mGameModeSelected;//ModeMenu, ModeMenuChampionship, ModeMenuTimetrial, ModeMenuDeathmatch

private:

    void loadColorTextures();
    void loadUITextures(const PFLoader& pfLoaderGameshell);
    void createColorMaterials();

    bool mControlActivated[mControlsCount];
    bool mControlClicked[mControlsCount];
    bool mControlOver[mControlsCount];

    //per-control horizontal-mirror flag. Set on mControls[0] (the Mode button)
    //when deathmatch is selected, so the single-race texture is drawn flipped
    //and the active mode reads as distinct from a normal single race.
    bool mControlMirrored[mControlsCount];
};

#endif