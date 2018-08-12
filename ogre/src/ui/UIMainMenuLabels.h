#ifndef UIMAINMENULABELS_H
#define UIMAINMENULABELS_H

#include "UIMainMenuBackground.h"

#include "elements/UIManager.h"
#include "elements/UIButton.h"
#include "elements/UITable.h"
#include "elements/UILabel.h"

#include "../includes/MyGUI_KeyCode.h"

class UIMainMenuLabels : public UIMainMenuBackground, public UIButtonOnAction, public UILabelOnAction
{
public:
    UIMainMenuLabels(const ModeContext& modeContext, const GameMode gameMode);
    ~UIMainMenuLabels(){}

    //UIButtonOnAction
    void onButtonPressed(UIButton * button) override {}
    void onButtonReleased(UIButton * button) override;
    void onButtonOver(UIButton * button) override;

    //UILabelOnAction
    void onLabelPressed(UILabel * label) override {}
    void onLabelReleased(UILabel * label) override;
    void onLabelOver(UILabel * label) override;

    static bool checkCursorOverLabel(const Ogre::Vector2& pos, Ogre::TextAreaOverlayElement * label);

    static void wrapText(std::string& text, Ogre::Real rightBoundary, const Ogre::Font* font, Ogre::Real spaceWidth, Ogre::Real characterHeight);

protected:

    void createLabels(const Ogre::Matrix4& screenAdaptionRelative);

    void keyUp(MyGUI::KeyCode _key, wchar_t _char);

    void mousePressed(const Ogre::Vector2& pos) override;
    void mouseReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id) override;
    void mouseMoved(const Ogre::Vector2& pos) override;

    void destroy(CustomTrayManager* trayMgr)override;

    virtual void switchState(const SinglePlayerMenuStates& state) = 0;
    virtual void startRace() = 0;
    virtual void connectToServer() = 0;

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
    void showOptionTrophies(){mWindowTitleTrophies->show();}
    void showStartingGridTimer(){mStartingGridTimeLabel->show();}
    void showExitLabels(const std::string& title);
    void showRaceGridCharactersLabels();
    void showPodiumLabels(const finishBoardVec& finishBoard);
    void showLeaderboardLabels(const finishBoardVec& finishBoard);
    void showChampionshipResultsLabels();
    void showMultiIPLabels();
    void showMultiJoin();
    void hideMultiJoin();
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

#if !defined(__ANDROID__)
    void setKeyText();
#endif

    Ogre::TextAreaOverlayElement * mWindowTitle;
    Ogre::TextAreaOverlayElement * mWindowTitleTrophies;

    UILabel* mModeSingle;
    UILabel* mModeMulti;
    UILabel* mModeSingleTypeRace;
    UILabel* mModeSingleTypeChampionship;
    UILabel* mModeSingleTypeTimetrial;
    UILabel* mModeSingleDifficultyNovice;
    UILabel* mModeSingleDifficultyAdvanced;
    UILabel* mModeSingleDifficultyExpert;
    UILabel* mModeSingleDifficultyInsane;

    UILabel* mSingleTrackDescription;
    UILabel* mSingleTrackBestTime;
    UILabel* mSingleTrackViewBy;
    UILabel* mSingleTrackViewBySelection;
    bool mIsViewByDescription;

    UILabel* mSingleBioDescription;
    UILabel* mSingleBioViewBy;
    UILabel* mSingleBioViewBySelection;
    bool mIsBioByDescription;


    std::vector<UILabel*> mTracksLabels;
    std::vector<UILabel*> mCarsLabels;
    std::vector<UILabel*> mCharactersLabels;
    std::vector<UILabel*> mOptionLabels;

    UILabel* mOptionGraphicsLabel_Resolution_Val;
    UILabel* mOptionGraphicsLabel_Resolution_Apply;
    
    UILabel* mOptionInputLabel_Type;
    UIButton* mInputTypeValLeft;
    UIButton* mInputTypeValRight;
#if !defined(__ANDROID__)
    std::vector<UILabel*> mOptionInputLabels_Keys;
    UILabel* mOptionInputLabel_ResetDefaults;
    bool mIsInKeyInsertMode;
    UILabel* mSelectedKeyLabel;
    InputKeyMapping mSelectedKey;
#endif

    UILabel* mOptionSoundLabel_VolumeFX_Val;
    UIButton* mSoundVolumeFXValLeft;
    UIButton* mSoundVolumeFXValRight;

    UILabel* mOptionSoundLabel_VolumeMusic_Val;
    UIButton* mSoundVolumeMusicValLeft;
    UIButton* mSoundVolumeMusicValRight;

    UILabel* mOptionRaceLabel_Opponents_Val;
    UILabel* mOptionRaceLabel_Transmission_Val;
    UILabel* mOptionRaceLabel_KMPH_Val;
    UIButtonTick* mShadowVal;
    UIButtonTick* mVSyncVal;
    UIButtonTick* mFulscreenVal;
    UIButton* mOpponentsValLeft;
    UIButton* mOpponentsValRight;
    UIButtonTick* mMirrorVal;
    UIButtonTick* mGhostVal;
    UIButtonTick* mAdvLightingPlayerVal;
    UIButtonTick* mAdvLightingAIVal;

    UIButton* mHighScoresTrackLeft;
    UIButton* mHighScoresTrackRight;
    UILabel* mOptionHighScoresTrackLabel;
    UILabel* mOptionHighScoresReplayLabel;
    Ogre::PanelOverlayElement* mOptionHighScoresReplayIcon;
    UILabel* mOptionHighScoresTable2Label[GameState::mRaceGridCarsMax];
    UILabel* mOptionHighScoresTable3Label[GameState::mRaceGridCarsMax];
    Ogre::PanelOverlayElement* mOptionHighScoresTable4Icon[GameState::mRaceGridCarsMax];
    UILabel* mOptionHighScoresEmergentGunLabel;
    size_t mHighScoreTrackIndex;
    void fillHighScoreTable();
    void fillReplayStat();

    Ogre::TextAreaOverlayElement * mOptionVersionLabel;


    Ogre::TextAreaOverlayElement * mStartingGridTimeLabel;

    UILabel* mExitLabel;
    UILabel* mGameExitYesLabel;
    UILabel* mGameExitNoLabel;

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

    static const size_t mChampionshipResultsLabelsAmountMax = 12;//2 - unlock, 1 win all, 1 win expert/insane with car, 8 beat emergent gun on track
    Ogre::TextAreaOverlayElement * mChampionshipResultsLabel[mChampionshipResultsLabelsAmountMax];

    UILabel* mVideoTitleLabel;

    UILabel* mLabelCredits;
    UILabel* mLabelAbout;

    private:

    void createLabelsOptionsMain(const Ogre::Matrix4& screenAdaptionRelative);
    void createLabelsOptionsGraphics(const Ogre::Matrix4& screenAdaptionRelative);
    void createLabelsOptionsInput(const Ogre::Matrix4& screenAdaptionRelative);
    void createLabelsOptionsSound(const Ogre::Matrix4& screenAdaptionRelative);
    void createLabelsOptionsRace(const Ogre::Matrix4& screenAdaptionRelative);
    void createLabelsOptionsHiscores(const Ogre::Matrix4& screenAdaptionRelative);
    void createLabelsOptionsCreditsAbout(const Ogre::Matrix4& screenAdaptionRelative);
    void createLabelsOptionsOther(const Ogre::Matrix4& screenAdaptionRelative);

    Ogre::TextAreaOverlayElement * mModeMultiIP;
    UILabel* mModeMultiListOfRooms;
    Ogre::TextAreaOverlayElement * mModeMultiUserName;
    Ogre::TextAreaOverlayElement * mModeMultiRoomName;
    UILabel* mModeMultiCreateRoom;
    UILabel* mModeMultiJoinRoom;

    std::map<std::string, std::string> mTrackDesc;
    std::map<std::string, std::string> mCharacterDesc;

    UIManager<UIButton> mUIButtonsManager;
    UIManager<UIButtonTick> mUIButtonTicksManager;
    UIManager<UILabel> mUILabelsManager;
};

#endif