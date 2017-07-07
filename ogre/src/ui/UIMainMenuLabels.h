#ifndef UIMAINMENULABELS_H
#define UIMAINMENULABELS_H

#include "UIMainMenuBackground.h"

class UIMainMenuLabels : public UIMainMenuBackground
{
public:
    UIMainMenuLabels(const ModeContext& modeContext) : UIMainMenuBackground(modeContext){}
    ~UIMainMenuLabels(){}

protected:

    void createLabels(const Ogre::Matrix4& screenAdaptionRelative);

    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

    virtual void switchState(const SinglePlayerMenuStates& state) = 0;
    virtual void startRace() = 0;

    void setWindowTitle(const std::string& title){mWindowTitle->setCaption(title);}
    void setStartingGridTime(const std::string& title){mStartingGridTimeLabel->setCaption(title);}

    void showModeSingle(){mModeSingle->show();}
    void showModeDifficulty();
    void showTrackLabels();
    void showCarLabels();
    void showCharacterLabels();
    void showStartingGridTimer(){mStartingGridTimeLabel->show();}
    void showGameExitLabels();
    void showRaceGridCharactersLabels();
    void hideAllLabels();

private:

    bool checkCursorOverLabel(const Ogre::Vector2& pos, Ogre::TextAreaOverlayElement * label);

    Ogre::TextAreaOverlayElement * mWindowTitle;

    Ogre::TextAreaOverlayElement * mModeSingle;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyNovice;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyAdvanced;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyExpert;
    Ogre::TextAreaOverlayElement * mModeSingleDifficultyInsane;

    std::vector<Ogre::TextAreaOverlayElement *> mTracksLabels;
    std::vector<Ogre::TextAreaOverlayElement *> mCarsLabels;
    std::vector<Ogre::TextAreaOverlayElement *> mCharactersLabels;

    Ogre::TextAreaOverlayElement * mStartingGridTimeLabel;

    Ogre::TextAreaOverlayElement * mGameExitLabel;
    Ogre::TextAreaOverlayElement * mGameExitYesLabel;
    Ogre::TextAreaOverlayElement * mGameExitNoLabel;

    Ogre::TextAreaOverlayElement * mRaceGridCharactersLabel[GameState::mRaceGridCarsMax];
};

#endif