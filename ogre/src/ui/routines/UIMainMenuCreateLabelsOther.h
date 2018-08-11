void UIMainMenuLabels::createLabelsOptionsOther(const Ogre::Matrix4& screenAdaptionRelative)
{
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f;


    //Options Version
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(630.0f, 360.0f, 0.0f, 0.0f);
        mOptionVersionLabel = createTextArea("MainWindowOptionVersionLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mOptionVersionLabel->setCaption(mModeContext.getGameState().getVersion());
        mOptionVersionLabel->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionVersionLabel->setSpaceWidth(9.0f);
        mOptionVersionLabel->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionVersionLabel->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionVersionLabel->setFontName("SdkTrays/Caption");
        mOptionVersionLabel->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionVersionLabel);
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
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 20.0f, 0.0f, 0.0f);
        mVideoTitleLabel = mUILabelsManager.add();
        mVideoTitleLabel->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mVideoTitleLabel->setFixed(true);
        mVideoTitleLabel->getTextArea()->setCaption("");
        mVideoTitleLabel->getTextArea()->setCharHeight(30.0f * viewportHeight / 1024.0f);
        mVideoTitleLabel->getTextArea()->setSpaceWidth(9.0f);
        mVideoTitleLabel->getTextArea()->setHeight(30.0f * viewportHeight / 1024.0f);
        mVideoTitleLabel->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Center);
        //mVideoTitleLabel->getTextArea()->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mVideoTitleLabel->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 198.0f, 0.0f, 0.0f);
        mExitLabel = mUILabelsManager.add("mGameExit");
        mExitLabel->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mExitLabel->setFixed(true);
        mExitLabel->getTextArea()->setCaption("Exit the game?");
        mExitLabel->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mExitLabel->getTextArea()->setSpaceWidth(9.0f);
        mExitLabel->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mExitLabel->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Center);
        mExitLabel->getTextArea()->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mExitLabel->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(270.0f, 252.0f, 0.0f, 0.0f);
        mGameExitYesLabel = mUILabelsManager.add("mGameExit");
        mGameExitYesLabel->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mGameExitYesLabel->getTextArea()->setCaption("YES");
        mGameExitYesLabel->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitYesLabel->getTextArea()->setSpaceWidth(9.0f);
        mGameExitYesLabel->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitYesLabel->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mGameExitYesLabel->getTextArea()->setColour(UILabel::mInactiveLabel);
        mGameExitYesLabel->setLabelOnAction(this);
        getMainBackground()->addChild(mGameExitYesLabel->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(334.0f, 252.0f, 0.0f, 0.0f);
        mGameExitNoLabel = mUILabelsManager.add("mGameExit");
        mGameExitNoLabel->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mGameExitNoLabel->getTextArea()->setCaption("NO");
        mGameExitNoLabel->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitNoLabel->getTextArea()->setSpaceWidth(9.0f);
        mGameExitNoLabel->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mGameExitNoLabel->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mGameExitNoLabel->getTextArea()->setColour(UILabel::mInactiveLabel);
        mGameExitNoLabel->setLabelOnAction(this);
        getMainBackground()->addChild(mGameExitNoLabel->getTextArea());
    }

    for (size_t q = 0, u = 0, l = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        Ogre::Vector4 textBoxPos;
        if (q % 2 == 0)
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

    for (size_t q = 0; q < mPodiumCharacters; ++q)
    {
        Ogre::Vector4 textBoxPos;

        if (q == 0)
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(115.0f, 181.0f, 0.0f, 0.0f);
        if (q == 1)
            textBoxPos = screenAdaptionRelative * Ogre::Vector4(231.0f, 281.0f, 0.0f, 0.0f);
        if (q == 2)
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


    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(335.0f, 57.0f, 0.0f, 0.0f);
        mLeaderboardTableTitle1Label = createTextArea("MainWindowTableLeaderboardTitle1", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mLeaderboardTableTitle1Label->setCaption("Rank");
        mLeaderboardTableTitle1Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle1Label->setSpaceWidth(9.0f);
        mLeaderboardTableTitle1Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle1Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mLeaderboardTableTitle1Label->setFontName("SdkTrays/Caption");
        mLeaderboardTableTitle1Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mLeaderboardTableTitle1Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(360.0f, 57.0f, 0.0f, 0.0f);
        mLeaderboardTableTitle2Label = createTextArea("MainWindowTableLeaderboardTitle2", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mLeaderboardTableTitle2Label->setCaption("Name");
        mLeaderboardTableTitle2Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle2Label->setSpaceWidth(9.0f);
        mLeaderboardTableTitle2Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle2Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mLeaderboardTableTitle2Label->setFontName("SdkTrays/Caption");
        mLeaderboardTableTitle2Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mLeaderboardTableTitle2Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(490.0f, 57.0f, 0.0f, 0.0f);
        mLeaderboardTableTitle3Label = createTextArea("MainWindowTableLeaderboardTitle3", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mLeaderboardTableTitle3Label->setCaption("Vehicle");
        mLeaderboardTableTitle3Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle3Label->setSpaceWidth(9.0f);
        mLeaderboardTableTitle3Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle3Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mLeaderboardTableTitle3Label->setFontName("SdkTrays/Caption");
        mLeaderboardTableTitle3Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mLeaderboardTableTitle3Label);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(560.0f, 57.0f, 0.0f, 0.0f);
        mLeaderboardTableTitle4Label = createTextArea("MainWindowTableLeaderboardTitle4", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mLeaderboardTableTitle4Label->setCaption("Points");
        mLeaderboardTableTitle4Label->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle4Label->setSpaceWidth(9.0f);
        mLeaderboardTableTitle4Label->setHeight(16.0f * viewportHeight / 1024.0f);
        mLeaderboardTableTitle4Label->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mLeaderboardTableTitle4Label->setFontName("SdkTrays/Caption");
        mLeaderboardTableTitle4Label->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mLeaderboardTableTitle4Label);
    }

    for (size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
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


        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(335.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mLeaderboardTable1Label[q] = createTextArea("MainWindowLeaderTable1_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
            mLeaderboardTable1Label[q]->setCaption(Conversions::DMToString(q + 1));
            mLeaderboardTable1Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable1Label[q]->setSpaceWidth(9.0f);
            mLeaderboardTable1Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable1Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mLeaderboardTable1Label[q]->setFontName("SdkTrays/Caption");
            mLeaderboardTable1Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mLeaderboardTable1Label[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(360.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mLeaderboardTable2Label[q] = createTextArea("MainWindowLeaderTable2_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
            mLeaderboardTable2Label[q]->setCaption("");
            mLeaderboardTable2Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable2Label[q]->setSpaceWidth(9.0f);
            mLeaderboardTable2Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable2Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mLeaderboardTable2Label[q]->setFontName("SdkTrays/Caption");
            mLeaderboardTable2Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mLeaderboardTable2Label[q]);
        }

        {
            const Ogre::Real left = 490.0f;
            const Ogre::Real top = 80.0f;
            Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(left, 22.0f * q + top, left + 52.0f, 22.0f * q + top + 18.0f);
            mLeaderboardTable3Icon[q] = createPanel("MainWindowLeaderTable3Icon_" + Conversions::DMToString(q), pos, "Test/car0_0s.bmp");
            mLeaderboardTable3Icon[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            getMainBackground()->addChild(mLeaderboardTable3Icon[q]);
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(560.0f, 22.0f * q + 80.0f, 0.0f, 0.0f);
            mLeaderboardTable4Label[q] = createTextArea("MainWindowLeaderTable4_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
            mLeaderboardTable4Label[q]->setCaption("");
            mLeaderboardTable4Label[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable4Label[q]->setSpaceWidth(9.0f);
            mLeaderboardTable4Label[q]->setHeight(26.0f * viewportHeight / 1024.0f);
            mLeaderboardTable4Label[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mLeaderboardTable4Label[q]->setFontName("SdkTrays/Caption");
            mLeaderboardTable4Label[q]->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mLeaderboardTable4Label[q]);
        }
    }

    //championship results
    for (size_t q = 0; q < mChampionshipResultsLabelsAmountMax; ++q)
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 40.0f + q * 20.0f, 0.0f, 0.0f);
        mChampionshipResultsLabel[q] = createTextArea("MainWindowChampionshipResults" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mChampionshipResultsLabel[q]->setCaption("");
        mChampionshipResultsLabel[q]->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mChampionshipResultsLabel[q]->setSpaceWidth(9.0f);
        mChampionshipResultsLabel[q]->setHeight(26.0f * viewportHeight / 1024.0f);
        mChampionshipResultsLabel[q]->setAlignment(Ogre::TextAreaOverlayElement::Center);
        mChampionshipResultsLabel[q]->setFontName("SdkTrays/Caption");
        mChampionshipResultsLabel[q]->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mChampionshipResultsLabel[q]);
    }


}