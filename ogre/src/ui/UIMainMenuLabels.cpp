
#include "UIMainMenuLabels.h"

#include "../tools/Conversions.h"

#include "../BaseApp.h"

const Ogre::ColourValue UIMainMenuLabels::mInactiveLabel(0.51f, 0.51f, 0.51f);

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
        mModeSingle->setColour(mInactiveLabel);
        getMainBackground()->addChild(mModeSingle);
    }

#ifndef NO_MULTIPLAYER
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 137.0f, 0.0f, 0.0f);
        mModeMulti = createTextArea("MainWindowMulti", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMulti->setCaption("Multi Player");
        mModeMulti->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mModeMulti->setSpaceWidth(9.0f);
        mModeMulti->setHeight(46.0f * viewportHeight / 1024.0f);
        mModeMulti->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMulti->setFontName("SdkTrays/Caption");
        mModeMulti->setColour(mInactiveLabel);
        getMainBackground()->addChild(mModeMulti);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 160.0f, 0.0f, 0.0f);
        mModeMultiIP = createTextArea("MainWindowMultiIP", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiIP->setCaption("Server IP:");
        mModeMultiIP->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiIP->setSpaceWidth(9.0f);
        mModeMultiIP->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiIP->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiIP->setFontName("SdkTrays/Caption");
        mModeMultiIP->setColour(mInactiveLabel);
        getMainBackground()->addChild(mModeMultiIP);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 200.0f, 0.0f, 0.0f);
        mModeMultiConnect = createTextArea("MainWindowMultiConnect", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiConnect->setCaption("Get List of Rooms");
        mModeMultiConnect->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiConnect->setSpaceWidth(9.0f);
        mModeMultiConnect->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiConnect->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiConnect->setFontName("SdkTrays/Caption");
        mModeMultiConnect->setColour(mInactiveLabel);
        getMainBackground()->addChild(mModeMultiConnect);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 240.0f, 0.0f, 0.0f);
        mModeMultiUserName = createTextArea("MainWindowMultiUserName", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiUserName->setCaption("User Name:");
        mModeMultiUserName->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiUserName->setSpaceWidth(9.0f);
        mModeMultiUserName->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiUserName->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiUserName->setFontName("SdkTrays/Caption");
        mModeMultiUserName->setColour(mInactiveLabel);
        getMainBackground()->addChild(mModeMultiUserName);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 290.0f, 0.0f, 0.0f);
        mModeMultiRoomName = createTextArea("MainWindowMultiRoomName", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiRoomName->setCaption("Room Name:");
        mModeMultiRoomName->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiRoomName->setSpaceWidth(9.0f);
        mModeMultiRoomName->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiRoomName->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiRoomName->setFontName("SdkTrays/Caption");
        mModeMultiRoomName->setColour(mInactiveLabel);
        getMainBackground()->addChild(mModeMultiRoomName);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 350.0f, 0.0f, 0.0f);
        mModeMultiCreateRoom = createTextArea("MainWindowMultiCreateRoom", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiCreateRoom->setCaption("Create Room");
        mModeMultiCreateRoom->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiCreateRoom->setSpaceWidth(9.0f);
        mModeMultiCreateRoom->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiCreateRoom->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiCreateRoom->setFontName("SdkTrays/Caption");
        mModeMultiCreateRoom->setColour(mInactiveLabel);
        getMainBackground()->addChild(mModeMultiCreateRoom);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(420.0f, 350.0f, 0.0f, 0.0f);
        mModeMultiJoinRoom = createTextArea("MainWindowMultiJoinRoom", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeMultiJoinRoom->setCaption("Join Room");
        mModeMultiJoinRoom->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiJoinRoom->setSpaceWidth(9.0f);
        mModeMultiJoinRoom->setHeight(26.0f * viewportHeight / 1024.0f);
        mModeMultiJoinRoom->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeMultiJoinRoom->setFontName("SdkTrays/Caption");
        mModeMultiJoinRoom->setColour(mInactiveLabel);
        getMainBackground()->addChild(mModeMultiJoinRoom);
    }
#endif

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(320.0f, 193.0f, 0.0f, 0.0f);
        mModeSingleDifficultyNovice = createTextArea("MainWindowSingleDiffNovice", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mModeSingleDifficultyNovice->setCaption("Novice");
        mModeSingleDifficultyNovice->setCharHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyNovice->setSpaceWidth(9.0f);
        mModeSingleDifficultyNovice->setHeight(36.0f * viewportHeight / 1024.0f);
        mModeSingleDifficultyNovice->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mModeSingleDifficultyNovice->setFontName("SdkTrays/Caption");
        mModeSingleDifficultyNovice->setColour(mInactiveLabel);
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
        mModeSingleDifficultyAdvanced->setColour(mInactiveLabel);
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
        mModeSingleDifficultyExpert->setColour(mInactiveLabel);
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
        mModeSingleDifficultyInsane->setColour(mInactiveLabel);
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
        mTracksLabels[q]->setColour(mInactiveLabel);
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
        mCarsLabels[q]->setColour(mInactiveLabel);
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
        mCharactersLabels[q]->setColour(mInactiveLabel);
        getMainBackground()->addChild(mCharactersLabels[q]);
    }

    mOptionLabels.clear();

    for(size_t q = 0; q < 7; ++q)
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(333.0f, 64.0f + q * 25.0f, 0.0f, 0.0f);;
        mOptionLabels.push_back(NULL);
        mOptionLabels[q] = createTextArea("MainWindowOptionLabel_" + Conversions::DMToString(q), 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionLabels[q]->setCaption("");
        mOptionLabels[q]->setCharHeight(46.0f * viewportHeight / 1024.0f);
        mOptionLabels[q]->setSpaceWidth(9.0f);
        mOptionLabels[q]->setHeight(46.0f * viewportHeight / 1024.0f);
        mOptionLabels[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionLabels[q]->setFontName("SdkTrays/Caption");
        mOptionLabels[q]->setColour(mInactiveLabel);
        getMainBackground()->addChild(mOptionLabels[q]);
    }
    mOptionLabels[0]->setCaption("Graphics");
    mOptionLabels[1]->setCaption("Input Devices");
    mOptionLabels[2]->setCaption("Sound");
    mOptionLabels[3]->setCaption("Race Options");
    mOptionLabels[4]->setCaption("High Scores");
    mOptionLabels[5]->setCaption("Change Name");
    mOptionLabels[6]->setCaption("Trophies");

    //Options Race Opponents
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 62.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Opponents = createTextArea("MainWindowOptionRaceOpponentsLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionRaceLabel_Opponents->setCaption("Number of Opponents");
        mOptionRaceLabel_Opponents->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Opponents->setSpaceWidth(9.0f);
        mOptionRaceLabel_Opponents->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Opponents->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionRaceLabel_Opponents->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Opponents->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionRaceLabel_Opponents);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 62.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Opponents_Val = createTextArea("MainWindowOptionRaceOpponentsValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionRaceLabel_Opponents_Val->setCaption(Conversions::DMToString(mModeContext.getGameState().getAICount()));
        mOptionRaceLabel_Opponents_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Opponents_Val->setSpaceWidth(9.0f);
        mOptionRaceLabel_Opponents_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Opponents_Val->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionRaceLabel_Opponents_Val->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Opponents_Val->setColour(mInactiveLabel);
        getMainBackground()->addChild(mOptionRaceLabel_Opponents_Val);
    }

    //Options Race Transmission
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 82.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Transmission = createTextArea("MainWindowOptionRaceTransmissionLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionRaceLabel_Transmission->setCaption("Transmission");
        mOptionRaceLabel_Transmission->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Transmission->setSpaceWidth(9.0f);
        mOptionRaceLabel_Transmission->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Transmission->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionRaceLabel_Transmission->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Transmission->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionRaceLabel_Transmission);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 82.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Transmission_Val = createTextArea("MainWindowOptionRaceTransmissionValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        if(mModeContext.getGameState().getTransmissionType() == trAuto)
            mOptionRaceLabel_Transmission_Val->setCaption("Auto");
        else
            mOptionRaceLabel_Transmission_Val->setCaption("Manual");
        mOptionRaceLabel_Transmission_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Transmission_Val->setSpaceWidth(9.0f);
        mOptionRaceLabel_Transmission_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Transmission_Val->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionRaceLabel_Transmission_Val->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Transmission_Val->setColour(mInactiveLabel);
        getMainBackground()->addChild(mOptionRaceLabel_Transmission_Val);
    }

    //Options Race KMPH
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 102.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_KMPH = createTextArea("MainWindowOptionRaceKMPHLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionRaceLabel_KMPH->setCaption("Speed Display");
        mOptionRaceLabel_KMPH->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_KMPH->setSpaceWidth(9.0f);
        mOptionRaceLabel_KMPH->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_KMPH->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionRaceLabel_KMPH->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_KMPH->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionRaceLabel_KMPH);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 102.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_KMPH_Val = createTextArea("MainWindowOptionRaceKMPHValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        if(mModeContext.getGameState().isKMPh())
            mOptionRaceLabel_KMPH_Val->setCaption("Kph");
        else
            mOptionRaceLabel_KMPH_Val->setCaption("Mph");
        mOptionRaceLabel_KMPH_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_KMPH_Val->setSpaceWidth(9.0f);
        mOptionRaceLabel_KMPH_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_KMPH_Val->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionRaceLabel_KMPH_Val->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_KMPH_Val->setColour(mInactiveLabel);
        getMainBackground()->addChild(mOptionRaceLabel_KMPH_Val);
    }

    //Options Race Mirror
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(181.0f, 122.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Mirror = createTextArea("MainWindowOptionRaceMirrorLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mOptionRaceLabel_Mirror->setCaption("Rearview Mirror");
        mOptionRaceLabel_Mirror->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Mirror->setSpaceWidth(9.0f);
        mOptionRaceLabel_Mirror->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Mirror->setAlignment(Ogre::TextAreaOverlayElement::Right);
        mOptionRaceLabel_Mirror->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Mirror->setColour(Ogre::ColourValue::White);
        getMainBackground()->addChild(mOptionRaceLabel_Mirror);
    }
    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(194.0f, 122.0f, 0.0f, 0.0f);;
        mOptionRaceLabel_Mirror_Val = createTextArea("MainWindowOptionRaceMirrorValLabel", 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        if(mModeContext.getGameState().getMirrorEnabled())
            mOptionRaceLabel_Mirror_Val->setCaption("Yes");
        else
            mOptionRaceLabel_Mirror_Val->setCaption("No");
        mOptionRaceLabel_Mirror_Val->setCharHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Mirror_Val->setSpaceWidth(9.0f);
        mOptionRaceLabel_Mirror_Val->setHeight(26.0f * viewportHeight / 1024.0f);
        mOptionRaceLabel_Mirror_Val->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mOptionRaceLabel_Mirror_Val->setFontName("SdkTrays/Caption");
        mOptionRaceLabel_Mirror_Val->setColour(mInactiveLabel);
        getMainBackground()->addChild(mOptionRaceLabel_Mirror_Val);
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
        mGameExitLabel->setColour(mInactiveLabel);
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
        mGameExitYesLabel->setColour(mInactiveLabel);
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
        mGameExitNoLabel->setColour(mInactiveLabel);
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

#ifndef NO_MULTIPLAYER
    if(mModeMulti->isVisible() && OgreBites::Widget::isCursorOver(mModeMulti, pos, 0))
    {
        switchState(State_Multi);
        return;
    }

    if(mModeMultiConnect->isVisible() && OgreBites::Widget::isCursorOver(mModeMultiConnect, pos, 0))
    {
        switchState(State_MultiConnect);
        return;
    }

    if(mModeMultiCreateRoom->isVisible() && OgreBites::Widget::isCursorOver(mModeMultiCreateRoom, pos, 0))
    {
        switchState(State_MultiCreateRoom);
        return;
    }

    if(mModeMultiJoinRoom->isVisible() && OgreBites::Widget::isCursorOver(mModeMultiJoinRoom, pos, 0))
    {
        switchState(State_MultiJoinRoom);
        return;
    }
#endif

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

    for(size_t q = 0; q < mOptionLabels.size(); ++q)
    {
        if(mOptionLabels[q]->isVisible() && OgreBites::Widget::isCursorOver(mOptionLabels[q], pos, 0))
        {
            setWindowTitle("Options: " + mOptionLabels[q]->getCaption());
            switchState(static_cast<SinglePlayerMenuStates>(State_Options + q));
            return;
        }
    }

    if(mOptionRaceLabel_Opponents_Val->isVisible() && OgreBites::Widget::isCursorOver(mOptionRaceLabel_Opponents_Val, pos, 0))
    {
        size_t aiCount = mModeContext.getGameState().getAICount();
        ++aiCount;
        if(aiCount > GameState::mAIMax) aiCount = 3;

        mModeContext.getGameState().setAICount(aiCount);
        mOptionRaceLabel_Opponents_Val->setCaption(Conversions::DMToString(mModeContext.getGameState().getAICount()));
    }

    if(mOptionRaceLabel_Transmission_Val->isVisible() && OgreBites::Widget::isCursorOver(mOptionRaceLabel_Transmission_Val, pos, 0))
    {
        if(mModeContext.getGameState().getTransmissionType() == trAuto)
        {
            mModeContext.getGameState().setTransmissionType(trManual);
            mOptionRaceLabel_Transmission_Val->setCaption("Manual");
        }
        else
        {
            mModeContext.getGameState().setTransmissionType(trAuto);
            mOptionRaceLabel_Transmission_Val->setCaption("Auto");
        }
    }

    if(mOptionRaceLabel_KMPH_Val->isVisible() && OgreBites::Widget::isCursorOver(mOptionRaceLabel_KMPH_Val, pos, 0))
    {
        if(mModeContext.getGameState().isKMPh())
        {
            mModeContext.getGameState().setIsKMPh(false);
            mOptionRaceLabel_KMPH_Val->setCaption("Mph");
        }
        else
        {
            mModeContext.getGameState().setIsKMPh(true);
            mOptionRaceLabel_KMPH_Val->setCaption("Kph");
        }
    }

    if(mOptionRaceLabel_Mirror_Val->isVisible() && OgreBites::Widget::isCursorOver(mOptionRaceLabel_Mirror_Val, pos, 0))
    {
        if(mModeContext.getGameState().getMirrorEnabled())
        {
            mModeContext.getGameState().setMirrorEnabled(false);
            mOptionRaceLabel_Mirror_Val->setCaption("No");
        }
        else
        {
            mModeContext.getGameState().setMirrorEnabled(true);
            mOptionRaceLabel_Mirror_Val->setCaption("Yes");
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
#ifndef NO_MULTIPLAYER
    checkCursorOverLabel(pos, mModeMulti);
    checkCursorOverLabel(pos, mModeMultiConnect);
    checkCursorOverLabel(pos, mModeMultiCreateRoom);
    checkCursorOverLabel(pos, mModeMultiJoinRoom);
#endif
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

    for(size_t q = 0; q < mOptionLabels.size(); ++q)
    {
        bool isOver = checkCursorOverLabel(pos, mOptionLabels[q]);
    }
    checkCursorOverLabel(pos, mOptionRaceLabel_Opponents_Val);
    checkCursorOverLabel(pos, mOptionRaceLabel_Transmission_Val);
    checkCursorOverLabel(pos, mOptionRaceLabel_KMPH_Val);
    checkCursorOverLabel(pos, mOptionRaceLabel_Mirror_Val);

    checkCursorOverLabel(pos, mGameExitYesLabel);
    checkCursorOverLabel(pos, mGameExitNoLabel);
}

void UIMainMenuLabels::showModeSingleMulti()
{
    mModeSingle->show();
#ifndef NO_MULTIPLAYER
    mModeMulti->show();
#endif
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

void UIMainMenuLabels::showOptionLabels()
{
    for(size_t q = 0; q < mOptionLabels.size(); ++q)
        mOptionLabels[q]->show();
}

void UIMainMenuLabels::showOptionRaceLabels()
{
    mOptionRaceLabel_Opponents->show();
    mOptionRaceLabel_Opponents_Val->show();
    mOptionRaceLabel_Transmission->show();
    mOptionRaceLabel_Transmission_Val->show();
    mOptionRaceLabel_KMPH->show();
    mOptionRaceLabel_KMPH_Val->show();
    mOptionRaceLabel_Mirror->show();
    mOptionRaceLabel_Mirror_Val->show();
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

void UIMainMenuLabels::showMultiIPLabels()
{
    mModeMultiIP->show();
    mModeMultiConnect->show();
    mModeMultiUserName->show();
    mModeMultiRoomName->show();
    mModeMultiCreateRoom->show();
    mModeMultiJoinRoom->show();
}

void UIMainMenuLabels::hideAllLabels()
{
    mModeSingle->hide();
#ifndef NO_MULTIPLAYER
    mModeMulti->hide();
    mModeMultiIP->hide();
    mModeMultiConnect->hide();
    mModeMultiUserName->hide();
    mModeMultiRoomName->hide();
    mModeMultiCreateRoom->hide();
    mModeMultiJoinRoom->hide();
#endif
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

    for(size_t q = 0; q < mOptionLabels.size(); ++q)
        mOptionLabels[q]->hide();

    mOptionRaceLabel_Opponents->hide();
    mOptionRaceLabel_Opponents_Val->hide();
    mOptionRaceLabel_Transmission->hide();
    mOptionRaceLabel_Transmission_Val->hide();
    mOptionRaceLabel_KMPH->hide();
    mOptionRaceLabel_KMPH_Val->hide();
    mOptionRaceLabel_Mirror->hide();
    mOptionRaceLabel_Mirror_Val->hide();

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
        label->setColour(mInactiveLabel);
    }

    return ret;
}
