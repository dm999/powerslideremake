#ifndef UIRACE_H
#define UIRACE_H

#include <vector>

#include "UIBase.h"

#include "../tools/LinearController.h"

#include "../loaders/PFLoader.h"
#include "../OriginalSettings.h"
#include "../GameState.h"

#include "../gamemodes/ModeContext.h"

class CustomTrayManager;
class BaseRaceMode;

class UIRace : public UIBase
{
public:
    UIRace(const ModeContext& modeContext);
    ~UIRace(){}

    void load(CustomTrayManager* trayMgr, const GameState& gameState);

    void showBeforeStart1();
    void showBeforeStart2();
    void showBeforeStart3();
    void hideAllStart();
#if defined(__ANDROID__)
    void hideAllHelpers();
#endif

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

    void setEngineRPM(Ogre::Real rpm);
    void setCarSpeed(Ogre::Real speed);

    void setCurrentLap(unsigned short lap, unsigned short totallap);
    void setCarGear(unsigned char gear);

    void setCarPos(unsigned char pos, unsigned char totalcars);

    void hideAIDashboardCars();
    void setPlayerDashBoardSkin(const GameState& gameState);
    void setAIDashBoardSkin(const GameState& gameState, size_t aiDashIndex, const std::string& characterName);
    void setDashCarPos(size_t aiDashIndex, size_t playerLap, Ogre::Real playerLapPos, size_t aiLap, Ogre::Real aiLapPos);

    void setRaceTime(const std::string& time);

    void setShowPausedPanel(bool isShow);

    void setShowMiscText(bool isShow);
    void setMiscText(const std::string& text, const Ogre::ColourValue& col = Ogre::ColourValue::White);
    void setShowMiscTextRight(bool isShow);
    void setMiscTextRight(const std::string& text);
    void addMiscPanelText(const std::string& text, const Ogre::ColourValue& col = Ogre::ColourValue::White);
    void clearMiscPanelText();

    void destroy(CustomTrayManager* trayMgr);

    void createRearViewMirrorPanelTexture(BaseRaceMode* baseRaceMode, Ogre::Root * root, int width, int height);
    void rearViewMirrorPanelTextureRemoveAllViewports();
    Ogre::Viewport * rearViewMirrorPanelTextureAddViewport(Ogre::Camera* camera);

    void createRearViewMirrorPanel(CustomTrayManager* trayMgr, bool isPanelEnabled);
    void setRearViewMirrorPanelMaterial(const Ogre::String& material);
    void setRearViewMirrorPanelShow(bool isShow);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

    void setVisibleTachoNeedleAndPointer(bool isVisible);
    void initTachoNeedleAndPointer(Ogre::SceneManager * sceneManager, const GameState& gameState);
    void setPointerPosition(Ogre::Real steering, bool isBreaking);

    void setVisibleFinishSign(bool isVisible, size_t finishPos = 0);

    void setVisibleCheat(bool isVisible);

    static void loadDashboardCars(const GameState& gameState);

private:

    ModeContext mModeContext;

    bool mLoaded;

    void loadMisc(const GameState& gameState, const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell);

#if defined(__ANDROID__)
    void panelHit(Ogre::PanelOverlayElement* panel);

    Ogre::PanelOverlayElement* mPanelBurn;
    Ogre::PanelOverlayElement* mPanelBomb;
    Ogre::PanelOverlayElement* mPanelNitro;
#endif

    Ogre::PanelOverlayElement* mPanelSticky;

    Ogre::PanelOverlayElement* mBeforeStartPanelReadyL;
    Ogre::PanelOverlayElement* mBeforeStartPanelReadyC;
    Ogre::PanelOverlayElement* mBeforeStartPanelReadyR;
    Ogre::PanelOverlayElement* mBeforeStartPanelSetL;
    Ogre::PanelOverlayElement* mBeforeStartPanelSetC;
    Ogre::PanelOverlayElement* mBeforeStartPanelSetR;
    Ogre::PanelOverlayElement* mBeforeStartPanelGoL;
    Ogre::PanelOverlayElement* mBeforeStartPanelGoC;
    Ogre::PanelOverlayElement* mBeforeStartPanelGoR;

    Ogre::PanelOverlayElement* mPlayerDashboardCar;
    static const int mDashboardCarsCount = 11;
    Ogre::PanelOverlayElement* mDashboardCars[mDashboardCarsCount];

    Ogre::SceneNode * mChildNeedle;
    Ogre::ManualObject* mNeedle;
    LinearController<float> mEngineRPMToRotation;

    Ogre::SceneNode * mChildPointer;
    Ogre::ManualObject* mPointer;
    Ogre::ManualObject* mPointerArrow;
    bool mIsBreakInProgress;

    std::vector<Ogre::PanelOverlayElement*> mTachoLamps;
    std::vector<size_t> mTachoRange;
    bool mIsPickup;

    Ogre::PanelOverlayElement* mTachoSpeedDigit1;
    Ogre::PanelOverlayElement* mTachoSpeedDigit2;
    Ogre::PanelOverlayElement* mTachoSpeedDigit3;

    Ogre::PanelOverlayElement* mTachoTotalCarsDigit1;
    Ogre::PanelOverlayElement* mTachoTotalCarsDigit2;
    Ogre::PanelOverlayElement* mTachoPosDigit1;
    Ogre::PanelOverlayElement* mTachoPosDigit2;
    Ogre::PanelOverlayElement* mTachoTotalCarsDigitDiv;
    Ogre::Real mDashDigitLeftTotalCars1;

    Ogre::PanelOverlayElement* mDashLap1;
    Ogre::PanelOverlayElement* mDashLap2;
    Ogre::PanelOverlayElement* mDashTotalLap1;
    Ogre::PanelOverlayElement* mDashTotalLap2;
    Ogre::PanelOverlayElement* mTachoLapDigitDiv;
    Ogre::Real mDashDigitLeftTotalLap1;

    Ogre::PanelOverlayElement* mDashLapTime1;
    Ogre::PanelOverlayElement* mDashLapTime2;
    Ogre::PanelOverlayElement* mDashLapTime3;
    Ogre::PanelOverlayElement* mDashLapTime4;
    Ogre::PanelOverlayElement* mDashLapTime5;
    Ogre::PanelOverlayElement* mDashLapTime6;

    Ogre::PanelOverlayElement* mDashGear;
    

    std::pair<Ogre::Real, Ogre::Real> getTachoDigitOffset(unsigned char digit)const;
    std::pair<Ogre::Real, Ogre::Real> getDashDigitOffsetX(unsigned char digit)const;
    std::pair<Ogre::Real, Ogre::Real> getDashDigitOffsetY(unsigned char digit)const;
    Ogre::Vector4 mTachoDigitsTexture;

    Ogre::RenderTexture *mRearCamTexture;
    Ogre::PanelOverlayElement* mRearViewMirrorPanel;

    Ogre::PanelOverlayElement* mPausedPanel;

    Ogre::TextAreaOverlayElement * mMiscText;
    Ogre::TextAreaOverlayElement * mMiscTextRight;
    static const int mMiscTextArraySize = 5;
    Ogre::TextAreaOverlayElement * mMiscTextArray[mMiscTextArraySize];

#if defined(__ANDROID__)
    //Ogre::TextAreaOverlayElement * mArrowHelperLeft;
    //Ogre::TextAreaOverlayElement * mArrowHelperRight;
    Ogre::TextAreaOverlayElement * mArrowHelperBottom;
#endif

    Ogre::PanelOverlayElement* mFinishSignPanel;

};

#endif