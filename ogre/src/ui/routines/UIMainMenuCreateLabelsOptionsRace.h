void UIMainMenuLabels::createLabelsOptionsRace(const Ogre::Matrix4& screenAdaptionRelative)
{
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f;

    //Options Race Opponents
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 62.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionRace");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Number of Opponents");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 62.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Opponents_Val = mUILabelsManager.add("mOptionRace");
        mOptionRaceLabel_Opponents_Val->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mOptionRaceLabel_Opponents_Val->setFixed(true);
        mOptionRaceLabel_Opponents_Val->getTextArea()->setCaption(Conversions::DMToString(mModeContext.getGameState().getAICount()));
        mOptionRaceLabel_Opponents_Val->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Opponents_Val->getTextArea()->setSpaceWidth(9.0f);
        mOptionRaceLabel_Opponents_Val->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Opponents_Val->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(mOptionRaceLabel_Opponents_Val->getTextArea());
    }
    {
        mOpponentsValLeft = mUIButtonsManager.add("mOptionRace");
        //mOpponentsValLeft->loadBackground("OriginalButtonDown");
        mOpponentsValLeft->setBackgroundMaterial(mInputTypeValLeft->getMaterialName());
        mOpponentsValLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mOpponentsValLeft->setButtonOnAction(this);

        mOpponentsValRight = mUIButtonsManager.add("mOptionRace");
        //mOpponentsValRight->loadBackground("OriginalButtonUp");
        mOpponentsValRight->setBackgroundMaterial(mInputTypeValRight->getMaterialName());
        mOpponentsValRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(280.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mOpponentsValRight->setButtonOnAction(this);
    }

    //Options Race Transmission
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 82.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionRace");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Transmission");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 82.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Transmission_Val = mUILabelsManager.add("mOptionRace");
        mOptionRaceLabel_Transmission_Val->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        if (mModeContext.getGameState().getTransmissionType() == trAuto)
            mOptionRaceLabel_Transmission_Val->getTextArea()->setCaption("Auto");
        else if(mModeContext.getGameState().getTransmissionType() == trManual)
            mOptionRaceLabel_Transmission_Val->getTextArea()->setCaption("Manual");
        else if(mModeContext.getGameState().getTransmissionType() == trAutoAdvanced)
            mOptionRaceLabel_Transmission_Val->getTextArea()->setCaption("Auto 9");
        mOptionRaceLabel_Transmission_Val->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Transmission_Val->getTextArea()->setSpaceWidth(9.0f);
        mOptionRaceLabel_Transmission_Val->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Transmission_Val->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionRaceLabel_Transmission_Val->getTextArea()->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Transmission_Val->getTextArea()->setColour(UILabel::mInactiveLabel);
        mOptionRaceLabel_Transmission_Val->setLabelOnAction(this);
        getMainBackground()->addChild(mOptionRaceLabel_Transmission_Val->getTextArea());
    }
    {
        //mTransmissionValLeft.setBackgroundMaterial(mOpponentsValLeft.getMaterialName());
        //mTransmissionValLeft.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f, 82.0f, 12.0f, 12.0f));
        //mTransmissionValLeft.setButtonOnAction(this);
    }

    //Options Race KMPH
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 102.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionRace");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Speed Display");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 102.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_KMPH_Val = mUILabelsManager.add("mOptionRace");
        mOptionRaceLabel_KMPH_Val->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        //mOptionRaceLabel_KMPH_Val->setFixed(true);
        if (mModeContext.getGameState().isKMPh())
            mOptionRaceLabel_KMPH_Val->getTextArea()->setCaption("Kph");
        else
            mOptionRaceLabel_KMPH_Val->getTextArea()->setCaption("Mph");
        mOptionRaceLabel_KMPH_Val->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_KMPH_Val->getTextArea()->setSpaceWidth(9.0f);
        mOptionRaceLabel_KMPH_Val->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_KMPH_Val->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionRaceLabel_KMPH_Val->getTextArea()->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_KMPH_Val->getTextArea()->setColour(UILabel::mInactiveLabel);
        mOptionRaceLabel_KMPH_Val->setLabelOnAction(this);
        getMainBackground()->addChild(mOptionRaceLabel_KMPH_Val->getTextArea());
    }

    //Options Race Mirror
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 122.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionRace");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Rearview Mirror");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        mMirrorVal = mUIButtonTicksManager.add("mOptionRace");
        mMirrorVal->loadBackground("OriginalButtonTick");
        mMirrorVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 122.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getGameState().getMirrorEnabled(), true);
        mMirrorVal->setButtonOnAction(this);
    }

    //Options Race Ghost
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 142.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionRace");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Timetrial Ghost");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        mGhostVal = mUIButtonTicksManager.add("mOptionRace");
        mGhostVal->loadBackground("OriginalButtonTick");
        mGhostVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 142.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getGameState().getGhostEnabled(), true);
        mGhostVal->setButtonOnAction(this);
    }

}