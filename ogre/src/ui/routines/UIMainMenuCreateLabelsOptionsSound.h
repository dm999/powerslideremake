
void UIMainMenuLabels::createLabelsOptionsSound(const Ogre::Matrix4& screenAdaptionRelative)
{
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f;

    //Options Sound
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 62.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mSoundVolume");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("SoundFX");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 62.0f, 0.0f, 0.0f);;
        mOptionSoundLabel_VolumeFX_Val = mUILabelsManager.add("mSoundVolume");
        mOptionSoundLabel_VolumeFX_Val->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mOptionSoundLabel_VolumeFX_Val->setFixed(true);
        mOptionSoundLabel_VolumeFX_Val->getTextArea()->setCaption("");
        mOptionSoundLabel_VolumeFX_Val->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeFX_Val->getTextArea()->setSpaceWidth(9.0f);
        mOptionSoundLabel_VolumeFX_Val->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeFX_Val->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(mOptionSoundLabel_VolumeFX_Val->getTextArea());
    }
    {
        mSoundVolumeFXValLeft = mUIButtonsManager.add("mSoundVolume");
        mSoundVolumeFXValLeft->setBackgroundMaterial(mInputTypeValLeft->getMaterialName());
        mSoundVolumeFXValLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mSoundVolumeFXValLeft->setButtonOnAction(this);

        mSoundVolumeFXValRight = mUIButtonsManager.add("mSoundVolume");
        mSoundVolumeFXValRight->setBackgroundMaterial(mInputTypeValRight->getMaterialName());
        mSoundVolumeFXValRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(280.0f + buttonLeftAdj, 62.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mSoundVolumeFXValRight->setButtonOnAction(this);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 82.0f, 0.0f, 0.0f);;
        UILabel* label = mUILabelsManager.add("mSoundVolume");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Music");
        label->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 82.0f, 0.0f, 0.0f);;
        mOptionSoundLabel_VolumeMusic_Val = mUILabelsManager.add("mSoundVolume");
        mOptionSoundLabel_VolumeMusic_Val->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mOptionSoundLabel_VolumeMusic_Val->setFixed(true);
        mOptionSoundLabel_VolumeMusic_Val->getTextArea()->setCaption("");
        mOptionSoundLabel_VolumeMusic_Val->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeMusic_Val->getTextArea()->setSpaceWidth(9.0f);
        mOptionSoundLabel_VolumeMusic_Val->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionSoundLabel_VolumeMusic_Val->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(mOptionSoundLabel_VolumeMusic_Val->getTextArea());
    }
    {
        mSoundVolumeMusicValLeft = mUIButtonsManager.add("mSoundVolume");
        mSoundVolumeMusicValLeft->setBackgroundMaterial(mInputTypeValLeft->getMaterialName());
        mSoundVolumeMusicValLeft->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(250.0f + buttonLeftAdj, 82.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mSoundVolumeMusicValLeft->setButtonOnAction(this);

        mSoundVolumeMusicValRight = mUIButtonsManager.add("mSoundVolume");
        mSoundVolumeMusicValRight->setBackgroundMaterial(mInputTypeValRight->getMaterialName());
        mSoundVolumeMusicValRight->init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(280.0f + buttonLeftAdj, 82.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mSoundVolumeMusicValRight->setButtonOnAction(this);
    }
}