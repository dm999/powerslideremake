void UIMainMenuLabels::createLabelsOptionsHiscores(const Ogre::Matrix4& screenAdaptionRelative)
{
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f;
    
    //Options Highscores
    {
        mHighScoresTrackLeft = mUIButtonsManager.add("mHighScores");
        mHighScoresTrackLeft->setBackgroundMaterial(mInputTypeValLeft->getMaterialName());
        mHighScoresTrackLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(100.0f + buttonLeftAdj, 32.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mHighScoresTrackLeft->setButtonOnAction(this);

        mHighScoresTrackRight = mUIButtonsManager.add("mHighScores");
        mHighScoresTrackRight->setBackgroundMaterial(mInputTypeValRight->getMaterialName());
        mHighScoresTrackRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(180.0f + buttonLeftAdj, 32.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mHighScoresTrackRight->setButtonOnAction(this);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(128.0f, 32.0f, 0.0f, 0.0f);
        UILabel* label = mUILabelsManager.add("mHighScores");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("TRACK");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(211.0f, 32.0f, 0.0f, 0.0f);
        mOptionHighScoresTrackLabel = mUILabelsManager.add("mHighScores");
        mOptionHighScoresTrackLabel->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mOptionHighScoresTrackLabel->setFixed(true);
        mOptionHighScoresTrackLabel->getTextArea()->setCaption("SandBlaster");
        mOptionHighScoresTrackLabel->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionHighScoresTrackLabel->getTextArea()->setSpaceWidth(9.0f);
        mOptionHighScoresTrackLabel->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionHighScoresTrackLabel->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(mOptionHighScoresTrackLabel->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(15.0f, 62.0f, 0.0f, 0.0f);
        mOptionHighScoresReplayLabel = mUILabelsManager.add();
        mOptionHighScoresReplayLabel->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mOptionHighScoresReplayLabel->setFixed(true);
        mOptionHighScoresReplayLabel->getTextArea()->setCaption("");
        mOptionHighScoresReplayLabel->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionHighScoresReplayLabel->getTextArea()->setSpaceWidth(9.0f);
        mOptionHighScoresReplayLabel->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionHighScoresReplayLabel->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(mOptionHighScoresReplayLabel->getTextArea());
    }
    {
        const Ogre::Real left = 250.0f;
        const Ogre::Real top = 55.0f;
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(left, top, left + 52.0f, top + 20.0f);
        mOptionHighScoresReplayIcon = createPanel("MainWindowHiscoreReplayIcon", pos, "Test/car0_0s.bmp");
        mOptionHighScoresReplayIcon->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        getMainBackground()->addChild(mOptionHighScoresReplayIcon);
    }
    for (size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(15.0f, 20.0f * q + 90.0f, 0.0f, 0.0f);
            UILabel* label = mUILabelsManager.add("mHighScores");
            label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
            label->setFixed(true);
            label->getTextArea()->setCaption(Conversions::DMToString(q + 1));
            label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
            label->getTextArea()->setSpaceWidth(9.0f);
            label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
            label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
            getMainBackground()->addChild(label->getTextArea());
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(40.0f, 20.0f * q + 90.0f, 0.0f, 0.0f);
            mOptionHighScoresTable2Label[q] = mUILabelsManager.add("mHighScores");
            mOptionHighScoresTable2Label[q]->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
            mOptionHighScoresTable2Label[q]->setFixed(true);
            mOptionHighScoresTable2Label[q]->getTextArea()->setCaption("");
            mOptionHighScoresTable2Label[q]->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mOptionHighScoresTable2Label[q]->getTextArea()->setSpaceWidth(9.0f);
            mOptionHighScoresTable2Label[q]->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
            mOptionHighScoresTable2Label[q]->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
            getMainBackground()->addChild(mOptionHighScoresTable2Label[q]->getTextArea());
        }

        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(190.0f, 20.0f * q + 90.0f, 0.0f, 0.0f);
            mOptionHighScoresTable3Label[q] = mUILabelsManager.add("mHighScores");
            mOptionHighScoresTable3Label[q]->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
            mOptionHighScoresTable3Label[q]->setFixed(true);
            mOptionHighScoresTable3Label[q]->getTextArea()->setCaption("");
            mOptionHighScoresTable3Label[q]->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
            mOptionHighScoresTable3Label[q]->getTextArea()->setSpaceWidth(9.0f);
            mOptionHighScoresTable3Label[q]->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
            mOptionHighScoresTable3Label[q]->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mOptionHighScoresTable3Label[q]->getTextArea()->setFontName("SdkTrays/Caption");
            mOptionHighScoresTable3Label[q]->getTextArea()->setColour(Ogre::ColourValue::White);
            getMainBackground()->addChild(mOptionHighScoresTable3Label[q]->getTextArea());
        }

        {
            const Ogre::Real left = 250.0f;
            const Ogre::Real top = 85.0f;
            Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(left, 20.0f * q + top, left + 52.0f, 20.0f * q + top + 20.0f);
            mOptionHighScoresTable4Icon[q] = createPanel("MainWindowHiscoreTable4Icon_" + Conversions::DMToString(q), pos, "Test/car0_0s.bmp");
            mOptionHighScoresTable4Icon[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            getMainBackground()->addChild(mOptionHighScoresTable4Icon[q]);
        }
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(100.0f, 330.0f, 0.0f, 0.0f);
        mOptionHighScoresEmergentGunLabel = mUILabelsManager.add("mHighScores");
        mOptionHighScoresEmergentGunLabel->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mOptionHighScoresEmergentGunLabel->setFixed(true);
        mOptionHighScoresEmergentGunLabel->getTextArea()->setCaption("");
        mOptionHighScoresEmergentGunLabel->getTextArea()->setCharHeight(16.0f * viewportHeight / 1024.0f);
        mOptionHighScoresEmergentGunLabel->getTextArea()->setSpaceWidth(9.0f);
        mOptionHighScoresEmergentGunLabel->getTextArea()->setHeight(16.0f * viewportHeight / 1024.0f);
        mOptionHighScoresEmergentGunLabel->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(mOptionHighScoresEmergentGunLabel->getTextArea());
    }
}