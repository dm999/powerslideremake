#ifndef UIMAINMENULABELS_H
#define UIMAINMENULABELS_H

#include "UIMainMenuBackground.h"

#include "elements/UIButton.h"

class UIMainMenuLabels : public UIMainMenuBackground, public UIButtonOnAction
{
public:
    UIMainMenuLabels(const ModeContext& modeContext) : UIMainMenuBackground(modeContext){}
    ~UIMainMenuLabels(){}

    void onButtonPressed(UIButton * button) override {}
    void onButtonReleased(UIButton * button) override;

    static const Ogre::ColourValue mInactiveLabel;

    static bool checkCursorOverLabel(const Ogre::Vector2& pos, Ogre::TextAreaOverlayElement * label);

protected:

    void createLabels(const Ogre::Matrix4& screenAdaptionRelative);

    void mousePressed(const Ogre::Vector2& pos) override;
    void mouseReleased(const Ogre::Vector2& pos) override;
    void mouseMoved(const Ogre::Vector2& pos) override;

    void destroy(CustomTrayManager* trayMgr)override;

    virtual void switchState(const SinglePlayerMenuStates& state) = 0;
    virtual void startRace() = 0;

    void setWindowTitle(const std::string& title){mWindowTitle->setCaption(title);}
    void setStartingGridTime(const std::string& title){mStartingGridTimeLabel->setCaption(title);}

    void showModeSingleMulti();
    void showModeSingle(){mModeSingle->show();}
    void showModeMulti(){mModeMulti->show();}
    void showModeDifficulty();
    void showTrackLabels();
    void showCarLabels();
    void showCharacterLabels();
    void showOptionLabels();
    void showOptionGraphicsLabels();
    void showOptionRaceLabels();
    void showStartingGridTimer(){mStartingGridTimeLabel->show();}
    void showGameExitLabels();
    void showRaceGridCharactersLabels();
    void showPodiumLabels(const finishBoard_v& finishBoard);
    void showMultiIPLabels();
    void hideAllLabels();

    void setColorMultiIP(const Ogre::ColourValue& val){mModeMultiIP->setColour(val);}
    void setColorMultiUserName(const Ogre::ColourValue& val){mModeMultiUserName->setColour(val);}
    void setColorMultiRoomName(const Ogre::ColourValue& val){mModeMultiRoomName->setColour(val);}

private:

    Ogre::TextAreaOverlayElement * mWindowTitle;

    Ogre::TextAreaOverlayElement * mModeSingle;
    Ogre::TextAreaOverlayElement * mModeMulti;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyNovice;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyAdvanced;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyExpert;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyInsane;

    std::vector<Ogre::TextAreaOverlayElement *> mTracksLabels;
    std::vector<Ogre::TextAreaOverlayElement *> mCarsLabels;
    std::vector<Ogre::TextAreaOverlayElement *> mCharactersLabels;
    std::vector<Ogre::TextAreaOverlayElement *> mOptionLabels;
    Ogre::TextAreaOverlayElement * mOptionGraphicsLabel_Vendor;
    Ogre::TextAreaOverlayElement * mOptionGraphicsLabel_Vendor_Val;
    Ogre::TextAreaOverlayElement * mOptionGraphicsLabel_Renderer;
    Ogre::TextAreaOverlayElement * mOptionGraphicsLabel_Renderer_Val;
    Ogre::TextAreaOverlayElement * mOptionGraphicsLabel_Shadow;
    Ogre::TextAreaOverlayElement * mOptionGraphicsLabel_VSync;
    Ogre::TextAreaOverlayElement * mOptionGraphicsLabel_Fulscreen;
    Ogre::TextAreaOverlayElement * mOptionGraphicsLabel_Resolution;
    Ogre::TextAreaOverlayElement * mOptionGraphicsLabel_Resolution_Val;
    Ogre::TextAreaOverlayElement * mOptionGraphicsLabel_Resolution_Apply;
    Ogre::TextAreaOverlayElement * mOptionRaceLabel_Opponents;
    Ogre::TextAreaOverlayElement * mOptionRaceLabel_Opponents_Val;
    Ogre::TextAreaOverlayElement * mOptionRaceLabel_Transmission;
    Ogre::TextAreaOverlayElement * mOptionRaceLabel_Transmission_Val;
    Ogre::TextAreaOverlayElement * mOptionRaceLabel_KMPH;
    Ogre::TextAreaOverlayElement * mOptionRaceLabel_KMPH_Val;
    Ogre::TextAreaOverlayElement * mOptionRaceLabel_Mirror;
    UIButtonTick mShadowVal;
    UIButtonTick mVSyncVal;
    UIButtonTick mFulscreenVal;
    UIButton mOpponentsValLeft;
    UIButton mOpponentsValRight;
    UIButtonTick mMirrorVal;

    Ogre::TextAreaOverlayElement * mStartingGridTimeLabel;

    Ogre::TextAreaOverlayElement * mGameExitLabel;
    Ogre::TextAreaOverlayElement * mGameExitYesLabel;
    Ogre::TextAreaOverlayElement * mGameExitNoLabel;

    Ogre::TextAreaOverlayElement * mRaceGridCharactersLabel[GameState::mRaceGridCarsMax];

    Ogre::TextAreaOverlayElement * mPodiumCharactersFirstSecondThirdLabel[mPodiumCharacters];
    Ogre::TextAreaOverlayElement * mPodiumTableTitle1Label;
    Ogre::TextAreaOverlayElement * mPodiumTableTitle2Label;
    Ogre::TextAreaOverlayElement * mPodiumTableTitle3Label;
    Ogre::TextAreaOverlayElement * mPodiumTableTitle4Label;
    Ogre::TextAreaOverlayElement * mPodiumTable1Label[GameState::mRaceGridCarsMax];
    Ogre::TextAreaOverlayElement * mPodiumTable2Label[GameState::mRaceGridCarsMax];
    Ogre::TextAreaOverlayElement * mPodiumTable3Label[GameState::mRaceGridCarsMax];
    Ogre::TextAreaOverlayElement * mPodiumTable4Label[GameState::mRaceGridCarsMax];

    Ogre::TextAreaOverlayElement * mModeMultiIP;
    Ogre::TextAreaOverlayElement * mModeMultiConnect;
    Ogre::TextAreaOverlayElement * mModeMultiUserName;
    Ogre::TextAreaOverlayElement * mModeMultiRoomName;
    Ogre::TextAreaOverlayElement * mModeMultiCreateRoom;
    Ogre::TextAreaOverlayElement * mModeMultiJoinRoom;
};

#endif