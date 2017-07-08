
#include "UIMainMenuLabels.h"

#include "../tools/Conversions.h"

#include "../BaseApp.h"

namespace{
    Ogre::ColourValue inactiveLabel(0.51f, 0.51f, 0.51f);
}

void UIMainMenuLabels::createLabels(const Ogre::Matrix4& screenAdaptionRelative)
{
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f; 

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 10.0f, 0.0f, 0.0f);
        mWindowTitle = createTextArea("MainWindowTitle", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mWindowTitle->setCaption("Game Mode");
        mWindowTitle->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mWindowTitle->setSpaceWidth(9.0f);
        mWindowTitle->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mWindowTitle->setFontName("SdkTrays/Caption");
        mWindowTitle->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mWindowTitle);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 107.0f, 0.0f, 0.0f);
        mModeSingle = createTextArea("MainWindowSingle", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingle->setCaption("Single Player");
        mModeSingle->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mModeSingle->setSpaceWidth(9.0f);
        mModeSingle->setHeight(46.0f * viewportHeight / 1024.0f);
        mModeSingle->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingle->setFontName("SdkTrays/Caption");
        mModeSingle->setColour(inactiveLabel);
        getMainBackground()->addChild(mModeSingle);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 193.0f, 0.0f, 0.0f);
        mModeSingleDifficultyNovice = createTextArea("MainWindowSingleDiffNovice", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleDifficultyNovice->setCaption("Novice");
        mModeSingleDifficultyNovice->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyNovice->setSpaceWidth(9.0f);
        mModeSingleDifficultyNovice->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyNovice->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyNovice->setFontName("SdkTrays/Caption");
        mModeSingleDifficultyNovice->setColour(inactiveLabel);
        getMainBackground()->addChild(mModeSingleDifficultyNovice);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 213.0f, 0.0f, 0.0f);
        mModeSingleDifficultyAdvanced = createTextArea("MainWindowSingleDiffAdv", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleDifficultyAdvanced->setCaption("Advanced");
        mModeSingleDifficultyAdvanced->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyAdvanced->setSpaceWidth(9.0f);
        mModeSingleDifficultyAdvanced->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyAdvanced->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyAdvanced->setFontName("SdkTrays/Caption");
        mModeSingleDifficultyAdvanced->setColour(inactiveLabel);
        getMainBackground()->addChild(mModeSingleDifficultyAdvanced);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 233.0f, 0.0f, 0.0f);
        mModeSingleDifficultyExpert = createTextArea("MainWindowSingleDiffExpert", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleDifficultyExpert->setCaption("Expert");
        mModeSingleDifficultyExpert->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyExpert->setSpaceWidth(9.0f);
        mModeSingleDifficultyExpert->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyExpert->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyExpert->setFontName("SdkTrays/Caption");
        mModeSingleDifficultyExpert->setColour(inactiveLabel);
        getMainBackground()->addChild(mModeSingleDifficultyExpert);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 253.0f, 0.0f, 0.0f);
        mModeSingleDifficultyInsane = createTextArea("MainWindowSingleDiffInsane", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleDifficultyInsane->setCaption("Insane");
        mModeSingleDifficultyInsane->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyInsane->setSpaceWidth(9.0f);
        mModeSingleDifficultyInsane->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyInsane->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyInsane->setFontName("SdkTrays/Caption");
        mModeSingleDifficultyInsane->setColour(inactiveLabel);
        getMainBackground()->addChild(mModeSingleDifficultyInsane);
    }

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
    std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");

    mTracksLabels.clear();

    for(size_t q = 0; q < availTracks.size(); ++q)
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(333.0f, 56.0f + q * 25.0f, 0.0f, 0.0f);
        mTracksLabels.push_back(NULL);
        mTracksLabels[q] = createTextArea("MainWindowTrackLabel_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mTracksLabels[q]->setCaption(strPowerslide.getTrackTitle(availTracks[q]));
        mTracksLabels[q]->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mTracksLabels[q]->setSpaceWidth(9.0f);
        mTracksLabels[q]->setHeight(46.0f * viewportHeight / 1024.0f);
        mTracksLabels[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mTracksLabels[q]->setFontName("SdkTrays/Caption");
        mTracksLabels[q]->setColour(inactiveLabel);
        getMainBackground()->addChild(mTracksLabels[q]);
    }

    mCarsLabels.clear();

    for(size_t q = 0; q < availCars.size(); ++q)
    {
        Ogre::Vector4 pos(333.0f, 64.0f + q * 25.0f, 0.0f, 0.0f);
            if(q > 2)
                pos.y += 15;
            if(q > 5)
                pos.y += 15;
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * pos;
        mCarsLabels.push_back(NULL);
        mCarsLabels[q] = createTextArea("MainWindowCarLabel_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mCarsLabels[q]->setCaption(strPowerslide.getCarTitle(availCars[q]));
        mCarsLabels[q]->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mCarsLabels[q]->setSpaceWidth(9.0f);
        mCarsLabels[q]->setHeight(46.0f * viewportHeight / 1024.0f);
        mCarsLabels[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mCarsLabels[q]->setFontName("SdkTrays/Caption");
        mCarsLabels[q]->setColour(inactiveLabel);
        getMainBackground()->addChild(mCarsLabels[q]);
    }

    mCharactersLabels.clear();

    for(size_t q = 0; q < 7; ++q)//maximum available is 7
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(333.0f, 64.0f + q * 25.0f, 0.0f, 0.0f);;
        mCharactersLabels.push_back(NULL);
        mCharactersLabels[q] = createTextArea("MainWindowCharacterLabel_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mCharactersLabels[q]->setCaption("");
        mCharactersLabels[q]->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mCharactersLabels[q]->setSpaceWidth(9.0f);
        mCharactersLabels[q]->setHeight(46.0f * viewportHeight / 1024.0f);
        mCharactersLabels[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mCharactersLabels[q]->setFontName("SdkTrays/Caption");
        mCharactersLabels[q]->setColour(inactiveLabel);
        getMainBackground()->addChild(mCharactersLabels[q]);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(310.0f, 361.0f, 0.0f, 0.0f);
        mStartingGridTimeLabel = createTextArea("MainWindowTimer", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mStartingGridTimeLabel->setCaption("5");
        mStartingGridTimeLabel->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mStartingGridTimeLabel->setSpaceWidth(9.0f);
        mStartingGridTimeLabel->setHeight(26.0f * viewportHeight / 1024.0f);
        mStartingGridTimeLabel->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mStartingGridTimeLabel->setFontName("SdkTrays/Caption");
        mStartingGridTimeLabel->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mStartingGridTimeLabel);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 198.0f, 0.0f, 0.0f);
        mGameExitLabel = createTextArea("MainWindowExitGame", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mGameExitLabel->setCaption("Exit the game?");
        mGameExitLabel->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitLabel->setSpaceWidth(9.0f);
        mGameExitLabel->setHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitLabel->setAlignment(Ogre::TextAreaOverlayElement::Center);
        mGameExitLabel->setFontName("SdkTrays/Caption");
        mGameExitLabel->setColour(inactiveLabel);
        getMainBackground()->addChild(mGameExitLabel);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(270.0f, 252.0f, 0.0f, 0.0f);
        mGameExitYesLabel = createTextArea("MainWindowExitGameYes", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mGameExitYesLabel->setCaption("YES");
        mGameExitYesLabel->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitYesLabel->setSpaceWidth(9.0f);
        mGameExitYesLabel->setHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitYesLabel->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mGameExitYesLabel->setFontName("SdkTrays/Caption");
        mGameExitYesLabel->setColour(inactiveLabel);
        getMainBackground()->addChild(mGameExitYesLabel);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(334.0f, 252.0f, 0.0f, 0.0f);
        mGameExitNoLabel = createTextArea("MainWindowExitGameNo", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mGameExitNoLabel->setCaption("NO");
        mGameExitNoLabel->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitNoLabel->setSpaceWidth(9.0f);
        mGameExitNoLabel->setHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitNoLabel->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mGameExitNoLabel->setFontName("SdkTrays/Caption");
        mGameExitNoLabel->setColour(inactiveLabel);
        getMainBackground()->addChild(mGameExitNoLabel);
    }

    for(size_t q = 0, u = 0, l = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        Ogre::Vector4 textBoxPos;
        if(q % 2 == 0)
        {
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(u * 100.0f + 1.0f, 59.0f, 0.0f, 0.0f);
            ++u;
        }
        else
        {
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(l * 100.0f + 1.0f + 40.0f, 209.0f, 0.0f, 0.0f);
            ++l;
        }
        textBoxPos.y -= 11.0f * viewportHeight / 1024.0f;
        mRaceGridCharactersLabel[q] = createTextArea("MainWindowStartingGrid_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mRaceGridCharactersLabel[q]->setCaption(Conversions::DMToString(q + 1));
        mRaceGridCharactersLabel[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mRaceGridCharactersLabel[q]->setSpaceWidth(9.0f);
        mRaceGridCharactersLabel[q]->setHeight(26.0f * viewportHeight / 1024.0f);
        mRaceGridCharactersLabel[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mRaceGridCharactersLabel[q]->setFontName("SdkTrays/Caption");
        mRaceGridCharactersLabel[q]->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mRaceGridCharactersLabel[q]);
    }

    for(size_t q = 0; q < mPodiumCharacters; ++q)
    {
        Ogre::Vector4 textBoxPos;

        if(q == 0)
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(115.0f, 181.0f, 0.0f, 0.0f);
        if(q == 1)
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(231.0f, 281.0f, 0.0f, 0.0f);
        if(q == 2)
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(141.0f, 332.0f, 0.0f, 0.0f);

        mPodiumCharactersFirstSecondThirdLabel[q] = createTextArea("MainWindowPodium123_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mPodiumCharactersFirstSecondThirdLabel[q]->setCaption("");
        mPodiumCharactersFirstSecondThirdLabel[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mPodiumCharactersFirstSecondThirdLabel[q]->setSpaceWidth(9.0f);
        mPodiumCharactersFirstSecondThirdLabel[q]->setHeight(26.0f * viewportHeight / 1024.0f);
        mPodiumCharactersFirstSecondThirdLabel[q]->setAlignment(Ogre::TextAreaOverlayElement::Center);
        mPodiumCharactersFirstSecondThirdLabel[q]->setFontName("SdkTrays/Caption");
        mPodiumCharactersFirstSecondThirdLabel[q]->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mPodiumCharactersFirstSecondThirdLabel[q]);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(335.0f, 57.0f, 0.0f, 0.0f);
        mPodiumTableTitle1Label = createTextArea("MainWindowTableTitle1", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mPodiumTableTitle1Label->setCaption("Pos");
        mPodiumTableTitle1Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle1Label->setSpaceWidth(9.0f);
        mPodiumTableTitle1Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle1Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mPodiumTableTitle1Label->setFontName("SdkTrays/Caption");
        mPodiumTableTitle1Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mPodiumTableTitle1Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(360.0f, 57.0f, 0.0f, 0.0f);
        mPodiumTableTitle2Label = createTextArea("MainWindowTableTitle2", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mPodiumTableTitle2Label->setCaption("Name");
        mPodiumTableTitle2Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle2Label->setSpaceWidth(9.0f);
        mPodiumTableTitle2Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle2Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mPodiumTableTitle2Label->setFontName("SdkTrays/Caption");
        mPodiumTableTitle2Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mPodiumTableTitle2Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(490.0f, 57.0f, 0.0f, 0.0f);
        mPodiumTableTitle3Label = createTextArea("MainWindowTableTitle3", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mPodiumTableTitle3Label->setCaption("Race Time");
        mPodiumTableTitle3Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle3Label->setSpaceWidth(9.0f);
        mPodiumTableTitle3Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle3Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mPodiumTableTitle3Label->setFontName("SdkTrays/Caption");
        mPodiumTableTitle3Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mPodiumTableTitle3Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(560.0f, 57.0f, 0.0f, 0.0f);
        mPodiumTableTitle4Label = createTextArea("MainWindowTableTitle4", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mPodiumTableTitle4Label->setCaption("Best Lap");
        mPodiumTableTitle4Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle4Label->setSpaceWidth(9.0f);
        mPodiumTableTitle4Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mPodiumTableTitle4Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mPodiumTableTitle4Label->setFontName("SdkTrays/Caption");
        mPodiumTableTitle4Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mPodiumTableTitle4Label);
    }

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(335.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mPodiumTable1Label[q] = createTextArea("MainWindowTable1_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mPodiumTable1Label[q]->setCaption(Conversions::DMToString(q + 1));
            mPodiumTable1Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable1Label[q]->setSpaceWidth(9.0f);
            mPodiumTable1Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable1Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mPodiumTable1Label[q]->setFontName("SdkTrays/Caption");
            mPodiumTable1Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mPodiumTable1Label[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(360.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mPodiumTable2Label[q] = createTextArea("MainWindowTable2_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mPodiumTable2Label[q]->setCaption("");
            mPodiumTable2Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable2Label[q]->setSpaceWidth(9.0f);
            mPodiumTable2Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable2Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mPodiumTable2Label[q]->setFontName("SdkTrays/Caption");
            mPodiumTable2Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mPodiumTable2Label[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(490.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mPodiumTable3Label[q] = createTextArea("MainWindowTable3_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mPodiumTable3Label[q]->setCaption("");
            mPodiumTable3Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable3Label[q]->setSpaceWidth(9.0f);
            mPodiumTable3Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable3Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mPodiumTable3Label[q]->setFontName("SdkTrays/Caption");
            mPodiumTable3Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mPodiumTable3Label[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(560.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mPodiumTable4Label[q] = createTextArea("MainWindowTable4_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
            mPodiumTable4Label[q]->setCaption("");
            mPodiumTable4Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable4Label[q]->setSpaceWidth(9.0f);
            mPodiumTable4Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mPodiumTable4Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mPodiumTable4Label[q]->setFontName("SdkTrays/Caption");
            mPodiumTable4Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mPodiumTable4Label[q]);
        }
    }
}

void UIMainMenuLabels::mouseReleased(const Ogre::Vector2& pos)
{
    UIBaseMenu::mouseReleased(pos);

    if(mModeSingle->isVisible() && OgreBites::Widget::isCursorOver(mModeSingle, pos, 0))
    {
        switchState(State_Difficulty);
        return;
    }

    if(mModeSingleDifficultyNovice->isVisible() && OgreBites::Widget::isCursorOver(mModeSingleDifficultyNovice, pos, 0))
    {
        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), Easy, mModeContext.getGameState().getLapsCount());
        switchState(State_Track);
        return;
    }

    if(mModeSingleDifficultyAdvanced->isVisible() && OgreBites::Widget::isCursorOver(mModeSingleDifficultyAdvanced, pos, 0))
    {
        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), Medium, mModeContext.getGameState().getLapsCount());
        switchState(State_Track);
        return;
    }

    if(mModeSingleDifficultyExpert->isVisible() && OgreBites::Widget::isCursorOver(mModeSingleDifficultyExpert, pos, 0))
    {
        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), Hard, mModeContext.getGameState().getLapsCount());
        switchState(State_Track);
        return;
    }

    if(mModeSingleDifficultyInsane->isVisible() && OgreBites::Widget::isCursorOver(mModeSingleDifficultyInsane, pos, 0))
    {
        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), Insane, mModeContext.getGameState().getLapsCount());
        switchState(State_Track);
        return;
    }

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
    std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");

    for(size_t q = 0; q < mTracksLabels.size(); ++q)
    {
        if(mTracksLabels[q]->isVisible() && OgreBites::Widget::isCursorOver(mTracksLabels[q], pos, 0))
        {
            mModeContext.getGameState().setRaceParameters(availTracks[q], mModeContext.getGameState().getAIStrength());
            if(
                mModeContext.getGameState().getTrackName() == "stunt track"         ||
                mModeContext.getGameState().getTrackName() == "luge track"          ||
                mModeContext.getGameState().getTrackName() == "Foxnhound1 track"    ||
                mModeContext.getGameState().getTrackName() == "Foxnhound2 track"
                )
                mModeContext.getGameState().setAICount(0);

            selectTrack(mModeContext.getGameState().getTrackNameAsOriginal());

            switchState(State_Car);
            return;
        }
    }

    for(size_t q = 0; q < mCarsLabels.size(); ++q)
    {
        if(mCarsLabels[q]->isVisible() && OgreBites::Widget::isCursorOver(mCarsLabels[q], pos, 0))
        {
            std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(availCars[q]);
            mModeContext.getGameState().getPlayerCar().setCharacterName(availChars[0]);

            std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
            characterCar = strPowerslide.getBaseCarFromCar(characterCar);
            selectCar(characterCar);

            for(size_t q = 0; q < 7; ++q)
            {
                if(q < availChars.size())
                    mCharactersLabels[q]->setCaption(STRPowerslide::getCharacterTitle(availChars[q]));
                else
                    mCharactersLabels[q]->setCaption("");
            }

            switchState(State_Character);
            return;
        }
    }

    for(size_t q = 0; q < mCharactersLabels.size(); ++q)
    {
        if(mCharactersLabels[q]->isVisible() && OgreBites::Widget::isCursorOver(mCharactersLabels[q], pos, 0))
        {
            std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
            characterCar = strPowerslide.getBaseCarFromCar(characterCar);

            std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

            mModeContext.getGameState().getPlayerCar().setCharacterName(availChars[q]);

            switchState(State_StartingGrid);
            return;
        }
    }

    if(mGameExitYesLabel->isVisible() && OgreBites::Widget::isCursorOver(mGameExitYesLabel, pos, 0))
    {
        mModeContext.getBaseApp()->setShutdown(false);
        return;
    }

    if(mGameExitNoLabel->isVisible() && OgreBites::Widget::isCursorOver(mGameExitNoLabel, pos, 0))
    {
        switchState(State_SingleMulti);
        return;
    }
}

void UIMainMenuLabels::mouseMoved(const Ogre::Vector2& pos)
{
    UIBaseMenu::mouseMoved(pos);

    checkCursorOverLabel(pos, mModeSingle);
    checkCursorOverLabel(pos, mModeSingleDifficultyNovice);
    checkCursorOverLabel(pos, mModeSingleDifficultyAdvanced);
    checkCursorOverLabel(pos, mModeSingleDifficultyExpert);
    checkCursorOverLabel(pos, mModeSingleDifficultyInsane);

    for(size_t q = 0; q < mTracksLabels.size(); ++q)
    {
        bool isOver = checkCursorOverLabel(pos, mTracksLabels[q]);
        if(isOver)
        {
            setTrackLogo(q);
        }
    }

    for(size_t q = 0; q < mCarsLabels.size(); ++q)
    {
        bool isOver = checkCursorOverLabel(pos, mCarsLabels[q]);
        if(isOver)
        {
            setCarLogo(q);
        }
    }

    for(size_t q = 0; q < mCharactersLabels.size(); ++q)
    {
        bool isOver = checkCursorOverLabel(pos, mCharactersLabels[q]);
        if(isOver)
        {
            const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
            std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
            for (size_t w = 0; w < availableCharacters.size(); ++w)
            {
                if(STRPowerslide::getCharacterTitle(availableCharacters[w]) == mCharactersLabels[q]->getCaption().asUTF8())
                    setCharacterLogo(w);
            }
        }
    }

    checkCursorOverLabel(pos, mGameExitYesLabel);
    checkCursorOverLabel(pos, mGameExitNoLabel);
}

void UIMainMenuLabels::showModeDifficulty()
{
    mModeSingleDifficultyNovice->show();
    mModeSingleDifficultyAdvanced->show();
    mModeSingleDifficultyExpert->show();
    mModeSingleDifficultyInsane->show();
}

void UIMainMenuLabels::showTrackLabels()
{
    for(size_t q = 0; q < mTracksLabels.size(); ++q)
        mTracksLabels[q]->show();
}

void UIMainMenuLabels::showCarLabels()
{
    for(size_t q = 0; q < mCarsLabels.size(); ++q)
        mCarsLabels[q]->show();
}

void UIMainMenuLabels::showCharacterLabels()
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::string character = mModeContext.getGameState().getPlayerCar().getCharacterName();
    std::string characterCar = strPowerslide.getCarFromCharacter(character);
    std::vector<std::string> availableCharacters = mModeContext.getGameState().getSTRPowerslide().getArrayValue("", "available characters");
    characterCar = strPowerslide.getBaseCarFromCar(characterCar);

    for(size_t q = 0; q < mCharactersLabels.size(); ++q)
    {
        if(mCharactersLabels[q]->getCaption() != "")
            mCharactersLabels[q]->show();
    }
    for (size_t q = 0; q < availableCharacters.size(); ++q)
    {
        if(STRPowerslide::getCharacterTitle(availableCharacters[q]) == mCharactersLabels[0]->getCaption().asUTF8())
            setCharacterLogo(q);
    }
}

void UIMainMenuLabels::showGameExitLabels()
{
    mGameExitLabel->show();
    mGameExitYesLabel->show();
    mGameExitNoLabel->show();
}

void UIMainMenuLabels::showRaceGridCharactersLabels()
{
    for(size_t q = 0; q < mModeContext.getGameState().getAICount() + 1; ++q)
    {
        mRaceGridCharactersLabel[q]->show();
    }
}

void UIMainMenuLabels::showPodiumLabels(const finishBoard_v& finishBoard)
{
    std::string playerName = "Dima";//d.polubotko: FIXME - add player name

    size_t podiumChars = mPodiumCharacters;
    if(finishBoard.size() < podiumChars) podiumChars = finishBoard.size();

    for(size_t q = 0; q < podiumChars; ++q)
    {
        if(finishBoard[q].mIsPlayer)
            mPodiumCharactersFirstSecondThirdLabel[q]->setCaption(playerName);
        else
            mPodiumCharactersFirstSecondThirdLabel[q]->setCaption(STRPowerslide::getCharacterTitle(finishBoard[q].mCharName));
        mPodiumCharactersFirstSecondThirdLabel[q]->show();
    }

    mPodiumTableTitle1Label->show();
    mPodiumTableTitle2Label->show();
    mPodiumTableTitle3Label->show();
    mPodiumTableTitle4Label->show();

    for(size_t q = 0; q < mModeContext.getGameState().getAICount() + 1; ++q)
    {
        if(finishBoard[q].mIsPlayer)
            mPodiumTable2Label[q]->setCaption(playerName);
        else
            mPodiumTable2Label[q]->setCaption(STRPowerslide::getCharacterTitle(finishBoard[q].mCharName));

        if(finishBoard[q].mRaceTime != 0.0f)
            mPodiumTable3Label[q]->setCaption(Tools::SecondsToString(finishBoard[q].mRaceTime));
        else
            mPodiumTable3Label[q]->setCaption("DNF");

        if(finishBoard[q].mBestLapTime != 0.0f)
            mPodiumTable4Label[q]->setCaption(Tools::SecondsToString(finishBoard[q].mBestLapTime));
        else
            mPodiumTable4Label[q]->setCaption("N/A");

        mPodiumTable1Label[q]->show();
        mPodiumTable2Label[q]->show();
        mPodiumTable3Label[q]->show();
        mPodiumTable4Label[q]->show();
    }
}

void UIMainMenuLabels::hideAllLabels()
{
    mModeSingle->hide();
    mModeSingleDifficultyNovice->hide();
    mModeSingleDifficultyAdvanced->hide();
    mModeSingleDifficultyExpert->hide();
    mModeSingleDifficultyInsane->hide();

    for(size_t q = 0; q < mTracksLabels.size(); ++q)
        mTracksLabels[q]->hide();

    for(size_t q = 0; q < mCarsLabels.size(); ++q)
        mCarsLabels[q]->hide();

    for(size_t q = 0; q < mCharactersLabels.size(); ++q)
        mCharactersLabels[q]->hide();

    mStartingGridTimeLabel->hide();

    mGameExitLabel->hide();
    mGameExitYesLabel->hide();
    mGameExitNoLabel->hide();

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        mRaceGridCharactersLabel[q]->hide();
    }

    for(size_t q = 0; q < mPodiumCharacters; ++q)
    {
        mPodiumCharactersFirstSecondThirdLabel[q]->hide();
    }

    mPodiumTableTitle1Label->hide();
    mPodiumTableTitle2Label->hide();
    mPodiumTableTitle3Label->hide();
    mPodiumTableTitle4Label->hide();
    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        mPodiumTable1Label[q]->hide();
        mPodiumTable2Label[q]->hide();
        mPodiumTable3Label[q]->hide();
        mPodiumTable4Label[q]->hide();
    }
}

bool UIMainMenuLabels::checkCursorOverLabel(const Ogre::Vector2& pos, Ogre::TextAreaOverlayElement * label)
{
    bool ret = false;
    if(label->isVisible() && OgreBites::Widget::isCursorOver(label, pos, 0))
    {
        ret = true;
        label->setColour(Ogre::ColourValue::White);
    }
    else
    {
        label->setColour(inactiveLabel);
    }

    return ret;
}
