#ifndef UIMAINMENULABELS_H
#define UIMAINMENULABELS_H

#include "UIMainMenuBackground.h"

#include "elements/UIButton.h"

class UIMainMenuLabels : public UIMainMenuBackground, public UIButtonOnAction
{
public:
    UIMainMenuLabels(const ModeContext& modeContext, const GameMode gameMode);
    ~UIMainMenuLabels(){}

    void onButtonPressed(UIButton * button) override {}
    void onButtonReleased(UIButton * button) override;

    static const Ogre::ColourValue mDisabledLabel;
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

    void setWindowTitle(const std::string& title);
    void setStartingGridTime(const std::string& title){mStartingGridTimeLabel->setCaption(title);}

    void showModeSingleMulti();
    void showModeSingle(){mModeSingle->show();}
    void showModeMulti(){mModeMulti->show();}
    void showModeSingleType();
    void showModeDifficulty();
    void showTrackLabels();
    void showCarLabels();
    void showCharacterLabels();
    void showOptionLabels();
    void showOptionGraphicsLabels();
    void showOptionInputLabels();
    void showOptionSoundLabels();
    void showOptionRaceLabels();
    void showOptionHiscoreLabels();
    void showOptionNameLabels();
    void showOptionTrophies(){mWindowTitleTrophies->show();}
    void showStartingGridTimer(){mStartingGridTimeLabel->show();}
    void showExitLabels(const std::string& title);
    void showRaceGridCharactersLabels();
    void showPodiumLabels(const finishBoardVec& finishBoard);
    void showLeaderboardLabels(const finishBoardVec& finishBoard);
    void showChampionshipResultsLabels();
    void showMultiIPLabels();
    void hideAllLabels();

    void setCurrentTrackBestTime();
    void setTrackBestTime(size_t index);

    void setCurrentTrackDescription();
    void setCurrentBioDescription();

    void setColorMultiIP(const Ogre::ColourValue& val){mModeMultiIP->setColour(val);}
    void setColorMultiUserName(const Ogre::ColourValue& val){mModeMultiUserName->setColour(val);}
    void setColorMultiRoomName(const Ogre::ColourValue& val){mModeMultiRoomName->setColour(val);}

private:

    Ogre::Real getTextWidth(const std::string& text, Ogre::TextAreaOverlayElement * element) const;

    void setTrackDescription(size_t index);
    void setBioDescription(size_t index);

    std::string getTextFileContent(const std::string& dir, const std::string& filename) const;

    virtual void onNameChange() = 0;

    Ogre::TextAreaOverlayElement * mWindowTitle;
    Ogre::TextAreaOverlayElement * mWindowTitleTrophies;

    Ogre::TextAreaOverlayElement * mModeSingle;
    Ogre::TextAreaOverlayElement * mModeMulti;
    Ogre::TextAreaOverlayElement * mModeSingleTypeRace;
    Ogre::TextAreaOverlayElement * mModeSingleTypeChampionship;
    Ogre::TextAreaOverlayElement * mModeSingleTypeTimetrial;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyNovice;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyAdvanced;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyExpert;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyInsane;

    Ogre::TextAreaOverlayElement * mSingleTrackDescription;
    Ogre::TextAreaOverlayElement * mSingleTrackBestTime;
    Ogre::TextAreaOverlayElement * mSingleTrackViewBy;
    Ogre::TextAreaOverlayElement * mSingleTrackViewBySelection;
    bool mIsViewByDescription;

    Ogre::TextAreaOverlayElement * mSingleBioDescription;
    Ogre::TextAreaOverlayElement * mSingleBioViewBy;
    Ogre::TextAreaOverlayElement * mSingleBioViewBySelection;
    bool mIsBioByDescription;


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
    
    Ogre::TextAreaOverlayElement * mOptionInputLabel_Type;
    UIButton mInputTypeValLeft;
    UIButton mInputTypeValRight;

    Ogre::TextAreaOverlayElement * mOptionSoundLabel_VolumeFX;
    Ogre::TextAreaOverlayElement * mOptionSoundLabel_VolumeFX_Val;
    UIButton mSoundVolumeFXValLeft;
    UIButton mSoundVolumeFXValRight;

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

    UIButton mHighScoresTrackLeft;
    UIButton mHighScoresTrackRight;
    Ogre::TextAreaOverlayElement * mOptionHighScoresButtonLabel;
    Ogre::TextAreaOverlayElement * mOptionHighScoresTrackLabel;
    Ogre::TextAreaOverlayElement * mOptionHighScoresTable1Label[GameState::mRaceGridCarsMax];
    Ogre::TextAreaOverlayElement * mOptionHighScoresTable2Label[GameState::mRaceGridCarsMax];
    Ogre::TextAreaOverlayElement * mOptionHighScoresTable3Label[GameState::mRaceGridCarsMax];
    Ogre::PanelOverlayElement* mOptionHighScoresTable4Icon[GameState::mRaceGridCarsMax];
    Ogre::TextAreaOverlayElement * mOptionHighScoresEmergentGunLabel;
    size_t mHighScoreTrackIndex;
    void fillHighScoreTable();

    Ogre::TextAreaOverlayElement * mOptionNameLabel;
    Ogre::TextAreaOverlayElement * mOptionNameLabel_Save;

    Ogre::TextAreaOverlayElement * mOptionVersionLabel;


    Ogre::TextAreaOverlayElement * mStartingGridTimeLabel;

    Ogre::TextAreaOverlayElement * mExitLabel;
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

    Ogre::TextAreaOverlayElement * mLeaderboardTableTitle1Label;
    Ogre::TextAreaOverlayElement * mLeaderboardTableTitle2Label;
    Ogre::TextAreaOverlayElement * mLeaderboardTableTitle3Label;
    Ogre::TextAreaOverlayElement * mLeaderboardTableTitle4Label;
    Ogre::TextAreaOverlayElement * mLeaderboardTable1Label[GameState::mRaceGridCarsMax];
    Ogre::TextAreaOverlayElement * mLeaderboardTable2Label[GameState::mRaceGridCarsMax];
    Ogre::PanelOverlayElement* mLeaderboardTable3Icon[GameState::mRaceGridCarsMax];
    Ogre::TextAreaOverlayElement * mLeaderboardTable4Label[GameState::mRaceGridCarsMax];

    protected:

    static const int mChampionshipResultsLabelsAmountMax = 12;//2 - unlock, 1 win all, 1 win expert/insane with car, 8 beat emergent gun on track
    Ogre::TextAreaOverlayElement * mChampionshipResultsLabel[mChampionshipResultsLabelsAmountMax];

    private:

    Ogre::TextAreaOverlayElement * mModeMultiIP;
    Ogre::TextAreaOverlayElement * mModeMultiConnect;
    Ogre::TextAreaOverlayElement * mModeMultiUserName;
    Ogre::TextAreaOverlayElement * mModeMultiRoomName;
    Ogre::TextAreaOverlayElement * mModeMultiCreateRoom;
    Ogre::TextAreaOverlayElement * mModeMultiJoinRoom;

    std::map<std::string, std::string> mTrackDesc;
    std::map<std::string, std::string> mCharacterDesc;
};

#endif