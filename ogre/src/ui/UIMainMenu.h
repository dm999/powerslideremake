#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#include "UIBaseMenu.h"

#include "../gamemodes/ModeContext.h"

#include "../GameState.h"

class MenuMode;
class LoaderListener;

enum SinglePlayerMenuStates{
    State_SingleMulti,
    State_Difficulty,
    State_Track,
    State_Car,
    State_Character
};

class UIMainMenu : public UIBaseMenu
{
public:

    UIMainMenu(const ModeContext& modeContext, MenuMode * menuMode);
    ~UIMainMenu();

    void load(CustomTrayManager* trayMgr, const GameState& gameState, LoaderListener* loaderListener);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

    bool isTopmostSubmenu()const;
    void setTopmostSubmenu();

protected:

    void selectTrack(const std::string& trackName);
    void selectCar(const std::string& carName);

private:

    void createSpecificMaterials();
    void createSpecificControls(const Ogre::Matrix4& screenAdaptionRelative, CustomTrayManager* trayMgr);

    void switchState(const SinglePlayerMenuStates& state);
    void hideAll();

    void panelHit(Ogre::PanelOverlayElement* panel) override;

    void startRace();

    void setTrackLogo(size_t index);
    void setCarLogo(size_t index);
    void setCharacterLogo(size_t index);

    ModeContext mModeContext;

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell, LoaderListener* loaderListener);

    MenuMode * mMenuMode;

    Ogre::PanelOverlayElement* mMainBackground;

    Ogre::PanelOverlayElement* mBackgroundA;
    Ogre::PanelOverlayElement* mBackgroundB;

    Ogre::PanelOverlayElement* mBackgroundTrack;
    Ogre::PanelOverlayElement* mBackgroundCar;
    Ogre::PanelOverlayElement* mBackgroundCharacter;

    Ogre::TextAreaOverlayElement * mWindowTitle;

    Ogre::TextAreaOverlayElement * mModeSingle;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyNovice;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyAdvanced;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyExpert;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyInsane;

    std::vector<Ogre::TextAreaOverlayElement *> mTracksLabels;
    std::vector<Ogre::TextAreaOverlayElement *> mCarsLabels;
    std::vector<Ogre::TextAreaOverlayElement *> mCharactersLabels;

    bool checkCursorOverLabel(const Ogre::Vector2& pos, Ogre::TextAreaOverlayElement * label);

    SinglePlayerMenuStates mCurrentState;
};

#endif