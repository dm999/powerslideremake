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
        mInputTypeValLeft->loadBackground("OriginalButtonDown");
        mInputTypeValLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mInputTypeValLeft->setButtonOnAction(this);

        mInputTypeValRight = mUIButtonsManager.add("mInputType");
        mInputTypeValRight->loadBackground("OriginalButtonUp");
        mInputTypeValRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(280.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mInputTypeValRight->setButtonOnAction(this);
    }

#if !defined(__ANDROID__)
    //input values
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 82.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Turn Left");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 102.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Turn Right");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 122.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Accelerate");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 142.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Brake");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 162.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Gear Up");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 182.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Gear Down");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 202.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Change View");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 222.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Hand Brake");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 242.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Weapon Burn");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 262.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Weapon Bomb");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 282.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Nitro");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 302.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Drop Camera");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }


    mOptionInputLabels_Keys.clear();
    for(size_t q = 0; q < 12; ++q)
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(200.0f, 82.0f + q * 20.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mInputType");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->getTextArea()->setCaption("Left");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        label->getTextArea()->setColour(UILabel::mInactiveLabel);
        label->setLabelOnAction(this);
        getMainBackground()->addChild(label->getTextArea());
        mOptionInputLabels_Keys.push_back(label);
    }
    
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(120.0f, 322.0f, 0.0f, 0.0f);;
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