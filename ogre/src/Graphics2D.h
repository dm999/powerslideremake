
#ifndef GRAPHICS2D_H
#define GRAPHICS2D_H

#include <vector>

#include "OgreInclude.h"

#include "SdkTrays.h"

#include "LinearController.h"

#include "GameCars.h"

#include "loaders/PFLoader.h"
#include "OriginalSettings.h"
#include "GameState.h"

class CustomTrayManager;

class Graphics2D
{
public:
    Graphics2D();
    ~Graphics2D(){}

    void load(  CustomTrayManager* trayMgr, 
                const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell,
                const STRPowerslide& strPowerslide,
                const STRRacecrud& strRacecrud,
                const GameState& gameState);

    void showBeforeStart1();
    void showBeforeStart2();
    void showBeforeStart3();
    void hideAllStart();

    void setEngineRPM(Ogre::Real rpm);
    void setCarSpeed(Ogre::Real speed);

    void setCurrentLap(unsigned short lap, unsigned short totallap);
    void setCarGear(unsigned char gear);

    void setCarPos(unsigned char pos, unsigned char totalcars);

    void hideAIDashboardCars();
    void setPlayerDashBoardSkin(GameCars carEnum);
    void setAIDashBoardSkin(size_t aiDashIndex, GameCars carEnum);
    void setDashCarPos(size_t aiDashIndex, size_t playerLap, Ogre::Real playerLapPos, size_t aiLap, Ogre::Real aiLapPos);

    void setRaceTime(const std::string& time);

    void setShowPausedPanel(bool isShow);

    void setShowMiscText(bool isShow);
    void setMiscText(const std::string& text);
    void addMiscPanelText(const std::string& text, const Ogre::ColourValue& col = Ogre::ColourValue::White);
    void clearMiscPanelText();

    void destroy();

    void createRearViewMirrorPanelTexture(Ogre::Root * root, int width, int height);
    void rearViewMirrorPanelTextureRemoveAllViewports();
    Ogre::Viewport * rearViewMirrorPanelTextureAddViewport(Ogre::Camera* camera);

    void createRearViewMirrorPanel(CustomTrayManager* trayMgr, bool isPanelEnabled);
    void setRearViewMirrorPanelMaterial(const Ogre::String& material);
    void setRearViewMirrorPanelShow(bool isShow);

    static Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top, const Ogre::String& material);
    static Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, const Ogre::Vector4& pos, const Ogre::String& material);
    static Ogre::TextAreaOverlayElement* createTextArea(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top);

    void reloadTextures(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell);

private:

    bool mLoaded;

    void loadDashboardCars(const PFLoader& pfLoaderGameshell);
    void loadMisc(const PFLoader& pfLoaderData);

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

    Ogre::Overlay* mNeedleLayer;
    LinearController<float> mEngineRPMToRotation;
    LinearController<float> mNeedleScaleCorrection;

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
    Ogre::Real mDashDigitLeftTotalCars1;

    Ogre::PanelOverlayElement* mDashLap1;
    Ogre::PanelOverlayElement* mDashLap2;
    Ogre::PanelOverlayElement* mDashTotalLap1;
    Ogre::PanelOverlayElement* mDashTotalLap2;
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
    static const int mMiscTextArraySize = 5;
    Ogre::TextAreaOverlayElement * mMiscTextArray[mMiscTextArraySize];

};

#endif