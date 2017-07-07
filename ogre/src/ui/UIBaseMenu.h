#ifndef UIBASEMENU_H
#define UIBASEMENU_H

#include "UIBase.h"

#include "../gamemodes/ModeContext.h"

class PFLoader;

namespace Ogre{
    class PanelOverlayElement;
}

struct finishBoardElement
{
    size_t mPos;
    bool mIsPlayer;
    float mRaceTime;
    std::string mCharName;

    finishBoardElement(size_t pos, bool isPlayer, float raceTime, const std::string& charName)
        : mPos(pos), mIsPlayer(isPlayer), mRaceTime(raceTime), mCharName(charName){}

    bool operator< (const finishBoardElement& other)const{return mPos < other.mPos;}
};

typedef std::vector<finishBoardElement> finishBoard_v;

class UIBaseMenu : public UIBase
{
public:
    UIBaseMenu(const ModeContext& modeContext);
    virtual ~UIBaseMenu(){}

    virtual void frameStarted(const Ogre::FrameEvent &evt) = 0;

    static const size_t amountTracks = 12;
    static const size_t amountCars = 7;

protected:

    void selectTrack(const std::string& trackName);
    void selectCar(const std::string& carName);

    void loadCommonTextures(const PFLoader& pfLoaderGameshell);
    void createCommonMaterials();
    void createControls(const Ogre::Matrix4& screenAdaptionRelative, Ogre::PanelOverlayElement* base);

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

    virtual void panelHit(Ogre::PanelOverlayElement* panel) = 0;

    static const int mControlsCount = 6;
    Ogre::PanelOverlayElement* mControls[mControlsCount];
    Ogre::TextAreaOverlayElement * mControlsText[mControlsCount];

    std::map<std::string, size_t> mRemapTrack;
    std::map<std::string, size_t> mRemapCar;

    ModeContext mModeContext;

    static const size_t mPodiumCharacters = 3;

private:
    bool mControlClicked[mControlsCount];
};

#endif