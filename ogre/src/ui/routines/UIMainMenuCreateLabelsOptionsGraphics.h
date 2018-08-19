void UIMainMenuLabels::createLabelsOptionsGraphics(const Ogre::Matrix4& screenAdaptionRelative)
{
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f;
    const size_t maxValStr = 20;

    //Options Graphics Vendor
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 62.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Vendor");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 62.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        Ogre::String vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        if (vendor.length() > maxValStr)
        {
            vendor = vendor.substr(0, maxValStr);
            vendor += "...";
        }
        label->getTextArea()->setCaption(vendor);
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }

    //Options Graphics Renderer
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 82.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Renderer");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 82.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        Ogre::String renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        if (renderer.length() > maxValStr)
        {
            renderer = renderer.substr(0, maxValStr);
            renderer += "...";
        }
        label->getTextArea()->setCaption(renderer);
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(label->getTextArea());
    }

    //Options Graphics Shadow
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 102.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Shadows");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        mShadowVal = mUIButtonTicksManager.add("mGraphics");
        mShadowVal->loadBackground("OriginalButtonTick");
        mShadowVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 102.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getGameState().isCastShadows(), true);
        mShadowVal->setButtonOnAction(this);
    }

    //Options Graphics VSync
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 122.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("VSync");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        bool isActive = true;
#if defined(__ANDROID__)
        isActive = false;
#endif
        mVSyncVal = mUIButtonTicksManager.add("mGraphics");
        mVSyncVal->loadBackground("OriginalButtonTick");
        mVSyncVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 122.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getRenderWindow()->isVSyncEnabled(), isActive);
        mVSyncVal->setButtonOnAction(this);
    }

    //Options Graphics Fullscreen
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 142.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Fullscreen");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        bool isActive = true;
#if defined(__ANDROID__)
        isActive = false;
#endif
        mFulscreenVal = mUIButtonTicksManager.add("mGraphics");
        mFulscreenVal->loadBackground("OriginalButtonTick");
        mFulscreenVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 142.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getRenderWindow()->isFullScreen(), isActive);
        mFulscreenVal->setButtonOnAction(this);
    }

    //Options Graphics Resolution
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 162.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Resolution");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 162.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Resolution_Val = mUILabelsManager.add("mOptionGraphicsLabel");
        mOptionGraphicsLabel_Resolution_Val->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        Ogre::RenderSystem * rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
        Ogre::ConfigOptionMap configOpts = rs->getConfigOptions();
        Ogre::ConfigOption videoMode = configOpts["Video Mode"];
        mOptionGraphicsLabel_Resolution_Val->getTextArea()->setCaption(videoMode.currentValue);
        mOptionGraphicsLabel_Resolution_Val->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Resolution_Val->getTextArea()->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Resolution_Val->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Resolution_Val->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionGraphicsLabel_Resolution_Val->getTextArea()->setColour(UILabel::mInactiveLabel);
        mOptionGraphicsLabel_Resolution_Val->setLabelOnAction(this);
        getMainBackground()->addChild(mOptionGraphicsLabel_Resolution_Val->getTextArea());
    }
    {
        Ogre::Real viewBySize = getTextWidth(mOptionGraphicsLabel_Resolution_Val->getTextArea()->getCaption(), mOptionGraphicsLabel_Resolution_Val->getTextArea());
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(0.0f, 162.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Resolution_Apply = mUILabelsManager.add("mOptionGraphicsLabel");
        mOptionGraphicsLabel_Resolution_Apply->init(0.0f, 0.0f, mOptionGraphicsLabel_Resolution_Val->getTextArea()->getLeft() + viewBySize + 9.0f, textBoxPos.y);
        mOptionGraphicsLabel_Resolution_Apply->getTextArea()->setCaption("apply");
        mOptionGraphicsLabel_Resolution_Apply->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Resolution_Apply->getTextArea()->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Resolution_Apply->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Resolution_Apply->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionGraphicsLabel_Resolution_Apply->getTextArea()->setColour(UILabel::mInactiveLabel);
        mOptionGraphicsLabel_Resolution_Apply->setLabelOnAction(this);
        getMainBackground()->addChild(mOptionGraphicsLabel_Resolution_Apply->getTextArea());
    }


    //Options Adv Lighting Player
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 182.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Specular Player");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        mAdvLightingPlayerVal = mUIButtonTicksManager.add("mGraphics");
        mAdvLightingPlayerVal->loadBackground("OriginalButtonTick");
        mAdvLightingPlayerVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 182.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getGameState().getAdvancedLightingPlayer(), true);
        mAdvLightingPlayerVal->setButtonOnAction(this);
    }

    //Options Attenuation Player
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 202.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Attenuation Player");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        mAttenuationPlayerVal = mUIButtonTicksManager.add("mGraphics");
        mAttenuationPlayerVal->loadBackground("OriginalButtonTick");
        mAttenuationPlayerVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 202.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getGameState().getAttenuationPlayer(), mModeContext.getGameState().getAdvancedLightingPlayer());
        mAttenuationPlayerVal->setButtonOnAction(this);
    }

    //Options Adv Lighting AI
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 222.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Specular AI");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        mAdvLightingAIVal = mUIButtonTicksManager.add("mGraphics");
        mAdvLightingAIVal->loadBackground("OriginalButtonTick");
        mAdvLightingAIVal->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(194.0f + buttonLeftAdj, 222.0f + buttonTopAdj, buttonSize, buttonSize), mModeContext.getGameState().getAdvancedLightingAI(), true);
        mAdvLightingAIVal->setButtonOnAction(this);
    }

    //Options Gamma
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 242.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mOptionGraphicsLabel");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Gamma");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 242.0f, 0.0f, 0.0f);;
        mOptionGraphicsLabel_Gamma_Val = mUILabelsManager.add("mOptionGraphicsLabel");
        mOptionGraphicsLabel_Gamma_Val->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mOptionGraphicsLabel_Gamma_Val->setFixed(true);
        mOptionGraphicsLabel_Gamma_Val->getTextArea()->setCaption(Conversions::DMToString(mModeContext.getGameState().getGamma(), 3, 1));
        mOptionGraphicsLabel_Gamma_Val->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Gamma_Val->getTextArea()->setSpaceWidth(9.0f);
        mOptionGraphicsLabel_Gamma_Val->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionGraphicsLabel_Gamma_Val->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(mOptionGraphicsLabel_Gamma_Val->getTextArea());
    }
    {
        mGammaValLeft = mUIButtonsManager.add("mGraphics");
        mGammaValLeft->loadBackground("OriginalButtonDown");
        mGammaValLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f + buttonLeftAdj, 242.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mGammaValLeft->setButtonOnAction(this);

        mGammaValRight = mUIButtonsManager.add("mGraphics");
        mGammaValRight->loadBackground("OriginalButtonUp");
        mGammaValRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(280.0f + buttonLeftAdj, 242.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mGammaValRight->setButtonOnAction(this);
    }
}