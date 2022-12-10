void UIMainMenuLabels::createLabelsOptionsInput(const Ogre::Matrix4& screenAdaptionRelative)
{
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f;

    //Options Input Type
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 62.0f, 0.0f, 0.0f);;
        mOptionInputLabel_Type = mUILabelsManager.add("mInputType");
        mOptionInputLabel_Type->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mOptionInputLabel_Type->setFixed(true);
#if !defined(__ANDROID__)
        if (mModeContext.getGameState().getInputType() == itKeyboard)
            mOptionInputLabel_Type->getTextArea()->setCaption("Keyboard");
        if (mModeContext.getGameState().getInputType() == itMouse)
            mOptionInputLabel_Type->getTextArea()->setCaption("Mouse");
#else
        mOptionInputLabel_Type->getTextArea()->setCaption("Touchscreen");
#endif
        mOptionInputLabel_Type->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionInputLabel_Type->getTextArea()->setSpaceWidth(9.0f);
        mOptionInputLabel_Type->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionInputLabel_Type->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(mOptionInputLabel_Type->getTextArea());
    }
    {
        mInputTypeValLeft = mUIButtonsManager.add("mInputType");
        mInputTypeValLeft->setBackgroundMaterial(mGammaValLeft->getMaterialName());
        mInputTypeValLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mInputTypeValLeft->setButtonOnAction(this);

        mInputTypeValRight = mUIButtonsManager.add("mInputType");
        mInputTypeValRight->setBackgroundMaterial(mGammaValRight->getMaterialName());
        mInputTypeValRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(280.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mInputTypeValRight->setButtonOnAction(this);
    }

#if !defined(__ANDROID__)

    const char labels[InputKeyMapping::kmSticky][100] = {
        "Turn Left", "Turn Right", "Accelerate", "Brake", 
        "Gear Up", "Gear Down",
        "Change View",
        "Change Ghost Visibility*",
        "Hand Brake",
        "Weapon Burn**", "Weapon Bomb**", "Nitro**", "Drop Camera***"
    };

    //input values
    for (size_t q = 0; q < InputKeyMapping::kmSticky; ++q)
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 82.0f + q * 20.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption(labels[q]);
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 352.0f, 0.0f, 0.0f);
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("* Time Trial Only");
        label->getTextArea()->setCharHeight(16.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(16.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 362.0f, 0.0f, 0.0f);
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("** Not Available in Championship and Time Trial");
        label->getTextArea()->setCharHeight(16.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(16.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 372.0f, 0.0f, 0.0f);
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("*** Use Tab in Multiplayer to Display Chat Input");
        label->getTextArea()->setCharHeight(16.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(16.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }


    mOptionInputLabels_Keys.clear();
    for(size_t q = 0; q < InputKeyMapping::kmSticky; ++q)
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(200.0f, 82.0f + q * 20.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        label->getTextArea()->setColour(UILabel::mInactiveLabel);
        label->setLabelOnAction(this);
        getMainBackground()->addChild(label->getTextArea());
        mOptionInputLabels_Keys.push_back(label);
    }
    setKeyText();
    
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(120.0f, 342.0f, 0.0f, 0.0f);;
        mOptionInputLabel_ResetDefaults = mUILabelsManager.add("mInputType");
        mOptionInputLabel_ResetDefaults->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mOptionInputLabel_ResetDefaults->getTextArea()->setCaption("Reset Defaults");
        mOptionInputLabel_ResetDefaults->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionInputLabel_ResetDefaults->getTextArea()->setSpaceWidth(9.0f);
        mOptionInputLabel_ResetDefaults->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionInputLabel_ResetDefaults->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionInputLabel_ResetDefaults->getTextArea()->setColour(UILabel::mInactiveLabel);
        mOptionInputLabel_ResetDefaults->setLabelOnAction(this);
        getMainBackground()->addChild(mOptionInputLabel_ResetDefaults->getTextArea());
        mOptionInputLabels_Keys.push_back(mOptionInputLabel_ResetDefaults);
    }
#endif
}