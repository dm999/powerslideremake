void UIMainMenuLabels::createLabelsOptionsMain(const Ogre::Matrix4& screenAdaptionRelative)
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
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(430.0f, 10.0f, 0.0f, 0.0f);
        mWindowTitleTrophies = createTextArea("MainWindowTitleTrophies", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mWindowTitleTrophies->setCaption("Trophies");
        mWindowTitleTrophies->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mWindowTitleTrophies->setSpaceWidth(9.0f);
        mWindowTitleTrophies->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mWindowTitleTrophies->setFontName("SdkTrays/Caption");
        mWindowTitleTrophies->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mWindowTitleTrophies);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 100.0f, 0.0f, 0.0f);
        mModeSingle = mUILabelsManager.add();
        mModeSingle->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeSingle->getTextArea()->setCaption("Single Player");
        mModeSingle->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mModeSingle->getTextArea()->setSpaceWidth(9.0f);
        mModeSingle->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mModeSingle->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingle->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingle->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingle->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 135.0f, 0.0f, 0.0f);
        mModeMulti = mUILabelsManager.add();
        mModeMulti->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeMulti->getTextArea()->setCaption("Multi Player");
        mModeMulti->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mModeMulti->getTextArea()->setSpaceWidth(9.0f);
        mModeMulti->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mModeMulti->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMulti->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeMulti->setLabelOnAction(this);
        getMainBackground()->addChild(mModeMulti->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(24.0f, 200.0f, 0.0f, 0.0f);
        mModeMultiIP = createTextArea("MainWindowMultiIP", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeMultiIP->setCaption("Server IP:");
        mModeMultiIP->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiIP->setSpaceWidth(9.0f);
        mModeMultiIP->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiIP->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiIP->setFontName("SdkTrays/Caption");
        mModeMultiIP->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mModeMultiIP);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 350.0f, 0.0f, 0.0f);
        mModeMultiListOfRooms = mUILabelsManager.add();
        mModeMultiListOfRooms->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeMultiListOfRooms->getTextArea()->setCaption("Get List of Rooms");
        mModeMultiListOfRooms->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiListOfRooms->getTextArea()->setSpaceWidth(9.0f);
        mModeMultiListOfRooms->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiListOfRooms->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiListOfRooms->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeMultiListOfRooms->setLabelOnAction(this);
        getMainBackground()->addChild(mModeMultiListOfRooms->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(24.0f, 260.0f, 0.0f, 0.0f);
        mModeMultiUserName = createTextArea("MainWindowMultiUserName", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeMultiUserName->setCaption("User Name:");
        mModeMultiUserName->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiUserName->setSpaceWidth(9.0f);
        mModeMultiUserName->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiUserName->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiUserName->setFontName("SdkTrays/Caption");
        mModeMultiUserName->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mModeMultiUserName);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(24.0f, 310.0f, 0.0f, 0.0f);
        mModeMultiRoomName = createTextArea("MainWindowMultiRoomName", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeMultiRoomName->setCaption("New Room Name:");
        mModeMultiRoomName->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiRoomName->setSpaceWidth(9.0f);
        mModeMultiRoomName->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiRoomName->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiRoomName->setFontName("SdkTrays/Caption");
        mModeMultiRoomName->setColour(UILabel::mInactiveLabel);
        getMainBackground()->addChild(mModeMultiRoomName);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(24.0f, 350.0f, 0.0f, 0.0f);
        mModeMultiCreateRoom = mUILabelsManager.add();
        mModeMultiCreateRoom->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeMultiCreateRoom->getTextArea()->setCaption("Create Room");
        mModeMultiCreateRoom->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiCreateRoom->getTextArea()->setSpaceWidth(9.0f);
        mModeMultiCreateRoom->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiCreateRoom->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiCreateRoom->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeMultiCreateRoom->setLabelOnAction(this);
        getMainBackground()->addChild(mModeMultiCreateRoom->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(460.0f, 350.0f, 0.0f, 0.0f);
        mModeMultiJoinRoom = mUILabelsManager.add();
        mModeMultiJoinRoom->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeMultiJoinRoom->getTextArea()->setCaption("Join Room");
        mModeMultiJoinRoom->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiJoinRoom->getTextArea()->setSpaceWidth(9.0f);
        mModeMultiJoinRoom->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiJoinRoom->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiJoinRoom->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeMultiJoinRoom->setLabelOnAction(this);
        getMainBackground()->addChild(mModeMultiJoinRoom->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 175.0f, 0.0f, 0.0f);
        mModeSingleTypeRace = mUILabelsManager.add("mModeSingle");
        mModeSingleTypeRace->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeSingleTypeRace->getTextArea()->setCaption("Single Race");
        mModeSingleTypeRace->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeRace->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleTypeRace->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeRace->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleTypeRace->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleTypeRace->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleTypeRace->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 200.0f, 0.0f, 0.0f);
        mModeSingleTypeChampionship = mUILabelsManager.add("mModeSingle");
        mModeSingleTypeChampionship->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeSingleTypeChampionship->getTextArea()->setCaption("Championship");
        mModeSingleTypeChampionship->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeChampionship->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleTypeChampionship->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeChampionship->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleTypeChampionship->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleTypeChampionship->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleTypeChampionship->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 225.0f, 0.0f, 0.0f);
        mModeSingleTypeTimetrial = mUILabelsManager.add("mModeSingle");
        mModeSingleTypeTimetrial->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeSingleTypeTimetrial->getTextArea()->setCaption("Time Trial");
        mModeSingleTypeTimetrial->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeTimetrial->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleTypeTimetrial->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleTypeTimetrial->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleTypeTimetrial->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleTypeTimetrial->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleTypeTimetrial->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 193.0f, 0.0f, 0.0f);
        mModeSingleDifficultyNovice = mUILabelsManager.add();
        mModeSingleDifficultyNovice->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeSingleDifficultyNovice->getTextArea()->setCaption("Novice");
        mModeSingleDifficultyNovice->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyNovice->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleDifficultyNovice->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyNovice->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyNovice->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleDifficultyNovice->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleDifficultyNovice->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 213.0f, 0.0f, 0.0f);
        mModeSingleDifficultyAdvanced = mUILabelsManager.add();
        mModeSingleDifficultyAdvanced->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeSingleDifficultyAdvanced->getTextArea()->setCaption("Advanced");
        mModeSingleDifficultyAdvanced->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyAdvanced->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleDifficultyAdvanced->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyAdvanced->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyAdvanced->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleDifficultyAdvanced->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleDifficultyAdvanced->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 233.0f, 0.0f, 0.0f);
        mModeSingleDifficultyExpert = mUILabelsManager.add();
        mModeSingleDifficultyExpert->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeSingleDifficultyExpert->getTextArea()->setCaption("Expert");
        mModeSingleDifficultyExpert->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyExpert->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleDifficultyExpert->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyExpert->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyExpert->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleDifficultyExpert->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleDifficultyExpert->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 253.0f, 0.0f, 0.0f);
        mModeSingleDifficultyInsane = mUILabelsManager.add();
        mModeSingleDifficultyInsane->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeSingleDifficultyInsane->getTextArea()->setCaption("Insane");
        mModeSingleDifficultyInsane->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyInsane->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleDifficultyInsane->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyInsane->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyInsane->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleDifficultyInsane->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleDifficultyInsane->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(324.0f, 273.0f, 0.0f, 0.0f);
        mModeSingleDifficultyUltraInsane = mUILabelsManager.add();
        mModeSingleDifficultyUltraInsane->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mModeSingleDifficultyUltraInsane->getTextArea()->setCaption("9x Insane");
        mModeSingleDifficultyUltraInsane->getTextArea()->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyUltraInsane->getTextArea()->setSpaceWidth(9.0f);
        mModeSingleDifficultyUltraInsane->getTextArea()->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyUltraInsane->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyUltraInsane->getTextArea()->setColour(UILabel::mInactiveLabel);
        mModeSingleDifficultyUltraInsane->setLabelOnAction(this);
        getMainBackground()->addChild(mModeSingleDifficultyUltraInsane->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(25.0f, 20.0f, 0.0f, 0.0f);
        mSingleTrackDescription = mUILabelsManager.add();
        mSingleTrackDescription->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mSingleTrackDescription->setFixed(true);
        mSingleTrackDescription->getTextArea()->setCaption("");
        mSingleTrackDescription->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackDescription->getTextArea()->setSpaceWidth(9.0f);
        mSingleTrackDescription->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackDescription->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(mSingleTrackDescription->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 340.0f, 0.0f, 0.0f);
        mSingleTrackBestTime = mUILabelsManager.add();
        mSingleTrackBestTime->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mSingleTrackBestTime->setFixed(true);
        mSingleTrackBestTime->getTextArea()->setCaption("");
        mSingleTrackBestTime->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackBestTime->getTextArea()->setSpaceWidth(9.0f);
        mSingleTrackBestTime->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackBestTime->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(mSingleTrackBestTime->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 360.0f, 0.0f, 0.0f);
        mSingleTrackViewBy = mUILabelsManager.add("mSingleTrackView");
        mSingleTrackViewBy->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mSingleTrackViewBy->setFixed(true);
        mSingleTrackViewBy->getTextArea()->setCaption("View By");
        mSingleTrackViewBy->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackViewBy->getTextArea()->setSpaceWidth(9.0f);
        mSingleTrackViewBy->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackViewBy->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(mSingleTrackViewBy->getTextArea());
    }

    {
        Ogre::Real viewBySize = getTextWidth(mSingleTrackViewBy->getTextArea()->getCaption(), mSingleTrackViewBy->getTextArea());
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(0.0f, 360.0f, 0.0f, 0.0f);
        mSingleTrackViewBySelection = mUILabelsManager.add("mSingleTrackView");
        mSingleTrackViewBySelection->init(0.0f, 0.0f, mSingleTrackViewBy->getTextArea()->getLeft() + viewBySize + 9.0f, textBoxPos.y);
        mSingleTrackViewBySelection->getTextArea()->setCaption("Image");
        mSingleTrackViewBySelection->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackViewBySelection->getTextArea()->setSpaceWidth(9.0f);
        mSingleTrackViewBySelection->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleTrackViewBySelection->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mSingleTrackViewBySelection->getTextArea()->setFontName("SdkTrays/Caption");
        mSingleTrackViewBySelection->getTextArea()->setColour(UILabel::mInactiveLabel);
        mSingleTrackViewBySelection->setLabelOnAction(this);
        getMainBackground()->addChild(mSingleTrackViewBySelection->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(25.0f, 20.0f, 0.0f, 0.0f);
        mSingleBioDescription = mUILabelsManager.add();
        mSingleBioDescription->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mSingleBioDescription->setFixed(true);
        mSingleBioDescription->getTextArea()->setCaption("");
        mSingleBioDescription->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioDescription->getTextArea()->setSpaceWidth(9.0f);
        mSingleBioDescription->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioDescription->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(mSingleBioDescription->getTextArea());
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(20.0f, 360.0f, 0.0f, 0.0f);
        mSingleBioViewBy = mUILabelsManager.add("mSingleBio");
        mSingleBioViewBy->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mSingleBioViewBy->setFixed(true);
        mSingleBioViewBy->getTextArea()->setCaption("View By");
        mSingleBioViewBy->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioViewBy->getTextArea()->setSpaceWidth(9.0f);
        mSingleBioViewBy->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioViewBy->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        getMainBackground()->addChild(mSingleBioViewBy->getTextArea());
    }

    {
        Ogre::Real viewBySize = getTextWidth(mSingleBioViewBy->getTextArea()->getCaption(), mSingleBioViewBy->getTextArea());
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(0.0f, 360.0f, 0.0f, 0.0f);
        mSingleBioViewBySelection = mUILabelsManager.add("mSingleBio");
        mSingleBioViewBySelection->init(0.0f, 0.0f, mSingleBioViewBy->getTextArea()->getLeft() + viewBySize + 9.0f, textBoxPos.y);
        mSingleBioViewBySelection->getTextArea()->setCaption("Bio");
        mSingleBioViewBySelection->getTextArea()->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioViewBySelection->getTextArea()->setSpaceWidth(9.0f);
        mSingleBioViewBySelection->getTextArea()->setHeight(26.0f * viewportHeight / 1024.0f);
        mSingleBioViewBySelection->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mSingleBioViewBySelection->getTextArea()->setFontName("SdkTrays/Caption");
        mSingleBioViewBySelection->getTextArea()->setColour(UILabel::mInactiveLabel);
        mSingleBioViewBySelection->setLabelOnAction(this);
        getMainBackground()->addChild(mSingleBioViewBySelection->getTextArea());
    }

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
    std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");

    mTracksLabels.clear();

    for (size_t q = 0; q < availTracks.size(); ++q)
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(333.0f, 56.0f + q * 30.0f, 0.0f, 0.0f);
        mTracksLabels.push_back(mUILabelsManager.add());
        mTracksLabels[q]->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mTracksLabels[q]->getTextArea()->setCaption(strPowerslide.getTrackTitle(availTracks[q]));
        mTracksLabels[q]->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mTracksLabels[q]->getTextArea()->setSpaceWidth(9.0f);
        mTracksLabels[q]->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mTracksLabels[q]->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mTracksLabels[q]->getTextArea()->setColour(UILabel::mInactiveLabel);
        mTracksLabels[q]->setLabelOnAction(this);
        getMainBackground()->addChild(mTracksLabels[q]->getTextArea());
    }

    mCarsLabels.clear();

    for (size_t q = 0; q < availCars.size(); ++q)
    {
        Ogre::Vector4 pos(333.0f, 64.0f + q * 25.0f, 0.0f, 0.0f);
        if (q > 2)
            pos.y += 15;
        if (q > 5)
            pos.y += 15;
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * pos;
        mCarsLabels.push_back(mUILabelsManager.add());
        mCarsLabels[q]->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mCarsLabels[q]->getTextArea()->setCaption(STRPowerslide::getCarTitle(availCars[q]));
        mCarsLabels[q]->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mCarsLabels[q]->getTextArea()->setSpaceWidth(9.0f);
        mCarsLabels[q]->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mCarsLabels[q]->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mCarsLabels[q]->getTextArea()->setColour(UILabel::mInactiveLabel);
        mCarsLabels[q]->setLabelOnAction(this);
        getMainBackground()->addChild(mCarsLabels[q]->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 223.0f, 0.0f, 0.0f);
        UILabel* label = mUILabelsManager.add("mCarParam");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Mass:");
        label->getTextArea()->setCharHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 243.0f, 0.0f, 0.0f);
        UILabel* label = mUILabelsManager.add("mCarParam");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Power:");
        label->getTextArea()->setCharHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 263.0f, 0.0f, 0.0f);
        UILabel* label = mUILabelsManager.add("mCarParam");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Acceleration:");
        label->getTextArea()->setCharHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 281.0f, 0.0f, 0.0f);
        UILabel* label = mUILabelsManager.add("mCarParam");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Traction:");
        label->getTextArea()->setCharHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 300.0f, 0.0f, 0.0f);
        UILabel* label = mUILabelsManager.add("mCarParam");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Top Speed:");
        label->getTextArea()->setCharHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 319.0f, 0.0f, 0.0f);
        UILabel* label = mUILabelsManager.add("mCarParam");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("AeroDynamics:");
        label->getTextArea()->setCharHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(169.0f, 338.0f, 0.0f, 0.0f);
        UILabel* label = mUILabelsManager.add("mCarParam");
        label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        label->setFixed(true);
        label->getTextArea()->setCaption("Stability:");
        label->getTextArea()->setCharHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setSpaceWidth(9.0f);
        label->getTextArea()->setHeight(20.0f * viewportHeight / 1024.0f);
        label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Right);
        getMainBackground()->addChild(label->getTextArea());
    }

    mCharactersLabels.clear();

    for (size_t q = 0; q < 7; ++q)//maximum available is 7
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(333.0f, 64.0f + q * 25.0f, 0.0f, 0.0f);;
        mCharactersLabels.push_back(mUILabelsManager.add());
        mCharactersLabels[q]->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mCharactersLabels[q]->getTextArea()->setCaption("");
        mCharactersLabels[q]->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mCharactersLabels[q]->getTextArea()->setSpaceWidth(9.0f);
        mCharactersLabels[q]->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mCharactersLabels[q]->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mCharactersLabels[q]->getTextArea()->setColour(UILabel::mInactiveLabel);
        mCharactersLabels[q]->setLabelOnAction(this);
        getMainBackground()->addChild(mCharactersLabels[q]->getTextArea());
    }

    mOptionLabels.clear();

    for (size_t q = 0; q < 9; ++q)
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(333.0f, 64.0f + q * 25.0f, 0.0f, 0.0f);;
        mOptionLabels.push_back(mUILabelsManager.add("mOptionLabels"));
        mOptionLabels[q]->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
        mOptionLabels[q]->getTextArea()->setCaption("");
        mOptionLabels[q]->getTextArea()->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mOptionLabels[q]->getTextArea()->setSpaceWidth(9.0f);
        mOptionLabels[q]->getTextArea()->setHeight(46.0f * viewportHeight / 1024.0f);
        mOptionLabels[q]->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionLabels[q]->getTextArea()->setColour(UILabel::mInactiveLabel);
        mOptionLabels[q]->setLabelOnAction(this);
        getMainBackground()->addChild(mOptionLabels[q]->getTextArea());
    }
    mOptionLabels[0]->getTextArea()->setCaption("Graphics");
    mOptionLabels[1]->getTextArea()->setCaption("Input Devices");
    mOptionLabels[2]->getTextArea()->setCaption("Sound");
    mOptionLabels[3]->getTextArea()->setCaption("Race Options");
    mOptionLabels[4]->getTextArea()->setCaption("High Scores");
    mOptionLabels[5]->getTextArea()->setCaption("Change Name");
    mOptionLabels[6]->getTextArea()->setCaption("Trophies");
    mOptionLabels[7]->getTextArea()->setCaption("Credits");
    mOptionLabels[8]->getTextArea()->setCaption("About");

}