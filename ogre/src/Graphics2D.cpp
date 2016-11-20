#include "pcheader.h"

#include "Graphics2D.h"

#include "tools/OgreTools.h"

#include "customs/CustomTrayManager.h"

#include "tools/Conversions.h"

#include "BaseApp.h"

#include "loaders/TextureLoader.h"
#include "loaders/TEXLoader.h"

#if defined(__ANDROID__)
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_write(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_write(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 
#endif

Graphics2D::Graphics2D() :
    mLoaded(false), mRearViewMirrorPanel(NULL)
{
    mEngineRPMToRotation.addPoint(1000.0f, 155.0f);
    mEngineRPMToRotation.addPoint(10000.0f, -70.0f);
}

void Graphics2D::load(  CustomTrayManager* trayMgr, const GameState& gameState)
{

    //startlight
    {
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc/startlights", "ready_left_m_2.tex", "OriginalStartReadyL", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc/startlights", "ready_centre_m_2.tex", "OriginalStartReadyC", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc/startlights", "ready_right_m_2.tex", "OriginalStartReadyR", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalStartReadyL");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BeforeStartReadyL", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }

        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalStartReadyC");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BeforeStartReadyC", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }

        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalStartReadyR");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BeforeStartReadyR", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }
    }

    {
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc/startlights", "set_left_m_2.tex", "OriginalStartSetL", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc/startlights", "ready_centre_m_2.tex", "OriginalStartSetC", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc/startlights", "set_right_m_2.tex", "OriginalStartSetR", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalStartSetL");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BeforeStartSetL", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }

        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalStartSetC");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BeforeStartSetC", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }

        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalStartSetR");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BeforeStartSetR", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }
    }

    {
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc/startlights", "go_left_m_2.tex", "OriginalStartGoL", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc/startlights", "go_centre_m_2.tex", "OriginalStartGoC", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc/startlights", "go_right_m_2.tex", "OriginalStartGoR", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalStartGoL");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BeforeStartGoL", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }

        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalStartGoC");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BeforeStartGoC", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }

        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalStartGoR");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BeforeStartGoR", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }
    }

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    Ogre::Matrix4 screenAdaptionRelative(
        viewportWidth / 640.0f, 0.0f,                       0.0f,                   0.0f,
        0.0f,                   viewportHeight / 480.0f,    0.0f,                   0.0f,
        0.0f,                   0.0f,                       viewportWidth / 640.0f, 0.0f,
        0.0f,                   0.0f,                       0.0f,                   viewportHeight / 480.0f);

    Ogre::Matrix4 screenAdaption(
        viewportWidth / 640.0f / 2.0f,  0.0f,                           0.0f,                           0.0f,
        0.0f,                           viewportHeight / 480.0f / 2.0f, 0.0f,                           0.0f,
        0.0f,                           0.0f,                           viewportWidth / 640.0f / 2.0f,  0.0f,
        0.0f,                           0.0f,                           0.0f,                           viewportHeight / 480.0f / 2.0f);
    Ogre::Vector4 screenShift(viewportWidth / 4.0f, 0.0f, viewportWidth / 4.0f, 0.0f);

    Ogre::Vector4 readyL = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "ready left texture region");
    readyL /= 255.0f;
    Ogre::Vector4 readySL = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "ready left screen region");
    readySL = screenAdaption * readySL + screenShift;
    mBeforeStartPanelReadyL = createPanel("BeforeStartReadyL", readySL, "Test/BeforeStartReadyL");
    mBeforeStartPanelReadyL->setUV(readyL.x, readyL.y, readyL.z, readyL.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelReadyL);

    Ogre::Vector4 readyC = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "ready centre texture region");
    readyC /= 255.0f;
    Ogre::Vector4 readySC = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "ready centre screen region");
    readySC = screenAdaption * readySC + screenShift;
    mBeforeStartPanelReadyC = createPanel("BeforeStartReadyC", readySC, "Test/BeforeStartReadyC");
    mBeforeStartPanelReadyC->setUV(readyC.x, readyC.y, readyC.z, readyC.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelReadyC);

    Ogre::Vector4 readyR = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "ready right texture region");
    readyR /= 255.0f;
    Ogre::Vector4 readySR = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "ready right screen region");
    readySR = screenAdaption * readySR + screenShift;
    mBeforeStartPanelReadyR = createPanel("BeforeStartReadyR", readySR, "Test/BeforeStartReadyR");
    mBeforeStartPanelReadyR->setUV(readyR.x, readyR.y, readyR.z, readyR.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelReadyR);

    Ogre::Vector4 setL = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "set left texture region");
    setL /= 255.0f;
    Ogre::Vector4 setSL = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "set left screen region");
    setSL = screenAdaption * setSL + screenShift;
    mBeforeStartPanelSetL = createPanel("BeforeStartSetL", setSL, "Test/BeforeStartSetL");
    mBeforeStartPanelSetL->setUV(setL.x, setL.y, setL.z, setL.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelSetL);

    Ogre::Vector4 setC = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "ready centre texture region");
    setC /= 255.0f;
    Ogre::Vector4 setSC = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "ready centre screen region");
    setSC = screenAdaption * setSC + screenShift;
    mBeforeStartPanelSetC = createPanel("BeforeStartSetC", setSC, "Test/BeforeStartSetC");
    mBeforeStartPanelSetC->setUV(setC.x, setC.y, setC.z, setC.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelSetC);

    Ogre::Vector4 setR = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "set right texture region");
    setR /= 255.0f;
    Ogre::Vector4 setSR = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "set right screen region");
    setSR = screenAdaption * setSR + screenShift;
    mBeforeStartPanelSetR = createPanel("BeforeStartSetR", setSR, "Test/BeforeStartSetR");
    mBeforeStartPanelSetR->setUV(setR.x, setR.y, setR.z, setR.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelSetR);

    Ogre::Vector4 goL = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "go left texture region");
    goL /= 255.0f;
    Ogre::Vector4 goSL = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "go left screen region");
    goSL = screenAdaption * goSL + screenShift;
    mBeforeStartPanelGoL = createPanel("BeforeStartGoL", goSL, "Test/BeforeStartGoL");
    mBeforeStartPanelGoL->setUV(goL.x, goL.y, goL.z, goL.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelGoL);

    Ogre::Vector4 goC = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "go centre texture region");
    goC /= 255.0f;
    Ogre::Vector4 goSC = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "go centre screen region");
    goSC = screenAdaption * goSC + screenShift;
    mBeforeStartPanelGoC = createPanel("BeforeStartGoC", goSC, "Test/BeforeStartGoC");
    mBeforeStartPanelGoC->setUV(goC.x, goC.y, goC.z, goC.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelGoC);

    Ogre::Vector4 goR = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "go right texture region");
    goR /= 255.0f;
    Ogre::Vector4 goSR = gameState.getSTRRacecrud().getArray4Value("on-grid parameters", "go right screen region");
    goSR = screenAdaption * goSR + screenShift;
    mBeforeStartPanelGoR = createPanel("BeforeStartGoR", goSR, "Test/BeforeStartGoR");
    mBeforeStartPanelGoR->setUV(goR.x, goR.y, goR.z, goR.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelGoR);

    hideAllStart();

    //dashboard and tacho

    Ogre::Real tachoWidth = viewportWidth / 5.73f;
    Ogre::Real tachoHeight = viewportHeight / 4.73f;

    //dashboard
    {
        std::string dashTexture = gameState.getSTRRacecrud().getValue(gameState.getPlayerCharacterName() + " dash parameters", "dash file", "max_dash.tga");
        std::transform(dashTexture.begin(), dashTexture.end(), dashTexture.begin(), ::tolower);
        TextureLoader().load(gameState.getPFLoaderData(), "data/misc/dashes", dashTexture, "OriginalDash", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        std::vector<Ogre::String> texName;
        texName.push_back("OriginalDash");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/DashboardWarthog", 
                            //"Test/Diffuse", 
                            "Test/DiffuseTransparent", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }

    //dashboard position
    {
        std::string dashPositionTexture = gameState.getSTRPowerslide().getValue(gameState.getPlayerCharacterName() + " parameters", "dash texture", "max_m_3.tex");
        std::transform(dashPositionTexture .begin(), dashPositionTexture .end(), dashPositionTexture .begin(), ::tolower);
        dashPositionTexture = dashPositionTexture.substr(0, dashPositionTexture.length() - 4) + "_m_3.tex";
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc/dashes", dashPositionTexture, "OriginalDashPosition", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        std::vector<Ogre::String> texName;
        texName.push_back("OriginalDashPosition");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/DashboardWarthogPosition", 
                            "Test/Diffuse", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
    }


    Ogre::Real dashWidth = viewportWidth;
    Ogre::Real dashHeight = tachoHeight / 1.7f;

    Ogre::Real dashTop = viewportHeight - dashHeight;

    {
        Ogre::Real dashPositionHeight = dashHeight / 1.8f;

        Ogre::PanelOverlayElement* dashboardPosition = createPanel("DashboardPosition", dashWidth, dashPositionHeight, 0.0f, dashTop, "Test/DashboardWarthogPosition");
        dashboardPosition->setUV(0.0f, 0.0f, 10.0f, 1.0f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(dashboardPosition);
    }

    {
        Ogre::PanelOverlayElement* dashboard = createPanel("Dashboard", dashWidth, dashHeight, 0.0f, dashTop, "Test/DashboardWarthog");
        dashboard->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(dashboard);
    }


    //dashboard cars
    {
        loadDashboardCars(gameState);

        std::vector<std::string> availableCharacters = gameState.getSTRPowerslide().getArrayValue("", "available characters");
        for (size_t q = 0; q < availableCharacters.size(); ++q)
        {
            std::string iconName = gameState.getSTRPowerslide().getValue(availableCharacters[q] + " parameters", "icon", "car0_0s.bmp");

            std::vector<Ogre::String> texName;
            texName.push_back("chroma_" + iconName);
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/" + iconName, 
                                "Test/DiffuseTransparent", 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            newMat->getTechnique(0)->getPass(0)->setSceneBlending (Ogre::SBT_TRANSPARENT_ALPHA);
            newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
            newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);

            Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
            state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
            state->setTextureScale(-1.0f, 1.0f);
        }

        Ogre::Real dashCarWidth = dashWidth / 18.0f;
        Ogre::Real dashCarHeight = dashHeight / 4.0f;

        Ogre::Real dashCarLeft = dashWidth / 3.0f + dashCarWidth;
        Ogre::Real dashCarTop = dashTop + (dashHeight / 2.91f) / 2.0f - dashCarHeight / 2.0f;
        

        mPlayerDashboardCar = createPanel("DashboardCar0", dashCarWidth, dashCarHeight, dashCarLeft, dashCarTop, "Test/car0_0s.bmp");
        mPlayerDashboardCar->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mPlayerDashboardCar);

        for(size_t q = 0; q < mDashboardCarsCount; ++q)
        {
            std::string dashCarName = "DashboardCar" + Conversions::DMToString(q + 1);
            mDashboardCars[q] = createPanel(dashCarName, dashCarWidth, dashCarHeight, dashCarLeft, dashCarTop, "Test/car0_0s.bmp");
            mDashboardCars[q]->hide();
            mDashboardCars[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);
            trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashboardCars[q]);
        }
    }

    //tacho
    {
        std::string tachoTexture = gameState.getSTRRacecrud().getValue(gameState.getPlayerCharacterName() + " dash parameters", "tacho file 1", "frantic_1024x768.tex");
        std::transform(tachoTexture.begin(), tachoTexture.end(), tachoTexture.begin(), ::tolower);
        tachoTexture = tachoTexture.substr(0, tachoTexture.length() - 4) + "_m_1.tex";
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc/tachos", tachoTexture, "OriginalTacho", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        std::vector<Ogre::String> texName;
        texName.push_back("OriginalTacho");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/TachoFrantic", 
                            "Test/DiffuseTransparent", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }

    Ogre::Real tachoLeft = viewportWidth - tachoWidth;
    Ogre::Real tachoTop = viewportHeight - tachoHeight;

    {
        Ogre::Vector4 tachoRegion = gameState.getSTRRacecrud().getArray4Value(gameState.getPlayerCharacterName() + " dash parameters", "tacho texture region 1");
        tachoRegion /= 255.0f;

        Ogre::PanelOverlayElement* tacho = createPanel("TachoFrantic", tachoWidth, tachoHeight, tachoLeft, tachoTop, "Test/TachoFrantic");
        tacho->setUV(tachoRegion.x, tachoRegion.y, tachoRegion.z, tachoRegion.w);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(tacho);
    }

    //tacho lamps
    {
        mIsPickup = false;
        if(gameState.getSTRRacecrud().getIntValue(gameState.getPlayerCharacterName() + " dash parameters", "pickup hack"))
            mIsPickup = true;

        size_t lampsCount = gameState.getSTRRacecrud().getIntValue(gameState.getPlayerCharacterName() + " dash parameters", "num tacho lights");
        mTachoRange.resize(lampsCount);

        if(mIsPickup) ++lampsCount;
        mTachoLamps.resize(lampsCount);

        for(size_t q = 0; q < lampsCount; ++q)
        {
            Ogre::Vector4 tachoLightsScreen = gameState.getSTRRacecrud().getArray4Value(gameState.getPlayerCharacterName() + " dash parameters", "tacho 1 lights screen " + Conversions::DMToString(q));
            tachoLightsScreen = screenAdaptionRelative * tachoLightsScreen;

            Ogre::Vector4 tachoLightsTexture = gameState.getSTRRacecrud().getArray4Value(gameState.getPlayerCharacterName() + " dash parameters", "tacho 1 lights " + Conversions::DMToString(q));
            tachoLightsTexture /= 255.0f;

            if(!mIsPickup || q < 1)
                mTachoRange[q] = gameState.getSTRRacecrud().getIntValue(gameState.getPlayerCharacterName() + " dash parameters", "tacho rev range " + Conversions::DMToString(q));

            mTachoLamps[q] = createPanel("TachoLamps" + Conversions::DMToString(q), tachoLightsScreen, "Test/TachoFrantic");
            mTachoLamps[q]->setUV(tachoLightsTexture.x, tachoLightsTexture.y, tachoLightsTexture.z, tachoLightsTexture.w);
            trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoLamps[q]);
            mTachoLamps[q]->hide();
        }
    }

    //tacho digits
    {
        Ogre::Vector4 tachoDigitsScreen100 = gameState.getSTRRacecrud().getArray4Value(gameState.getPlayerCharacterName() + " dash parameters", "speed digit hundreds 1");
        Ogre::Vector4 tachoDigitsScreen10 = gameState.getSTRRacecrud().getArray4Value(gameState.getPlayerCharacterName() + " dash parameters", "speed digit tens 1");
        Ogre::Vector4 tachoDigitsScreen1 = gameState.getSTRRacecrud().getArray4Value(gameState.getPlayerCharacterName() + " dash parameters", "speed digit units 1");

        tachoDigitsScreen1 = screenAdaptionRelative * tachoDigitsScreen1;
        tachoDigitsScreen10 = screenAdaptionRelative * tachoDigitsScreen10;
        tachoDigitsScreen100 = screenAdaptionRelative * tachoDigitsScreen100;

        mTachoDigitsTexture = gameState.getSTRRacecrud().getArray4Value(gameState.getPlayerCharacterName() + " dash parameters", "speed digit texture 1");
        mTachoDigitsTexture /= 255.0f;

        std::vector<Ogre::String> texName;
        texName.push_back("OriginalTacho");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/TachoFranticDigits", 
                            "Test/DiffuseTransparent", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureRotate(Ogre::Degree(90.0f));

        mTachoSpeedDigit1 = createPanel("TachoDigit1", tachoDigitsScreen1, "Test/TachoFranticDigits");
        std::pair<Ogre::Real, Ogre::Real> texCoords = getTachoDigitOffset(0);
        mTachoSpeedDigit1->setUV(texCoords.first, 1.0f - mTachoDigitsTexture.x, texCoords.second, 1.0f - mTachoDigitsTexture.z);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoSpeedDigit1);

        mTachoSpeedDigit2 = createPanel("TachoDigit2", tachoDigitsScreen10, "Test/TachoFranticDigits");
        mTachoSpeedDigit2->setUV(texCoords.first, 1.0f - mTachoDigitsTexture.x, texCoords.second, 1.0f - mTachoDigitsTexture.z);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoSpeedDigit2);

        mTachoSpeedDigit3 = createPanel("TachoDigit3", tachoDigitsScreen100, "Test/TachoFranticDigits");
        mTachoSpeedDigit3->setUV(texCoords.first, 1.0f - mTachoDigitsTexture.x, texCoords.second, 1.0f - mTachoDigitsTexture.z);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoSpeedDigit3);


        //mTachoSpeedDigit1->hide();
        mTachoSpeedDigit2->hide();
        mTachoSpeedDigit3->hide();
    }


    //font
    {
        TEXLoader().load(gameState.getPFLoaderData(), "data/misc", "font_set_1_m_1.tex", "OriginalFont", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        std::vector<Ogre::String> texName;
        texName.push_back("OriginalFont");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/DashFont", 
                            "Test/Diffuse", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        newMat->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_COLOUR);
    }

    {
        std::pair<Ogre::Real, Ogre::Real> texCoordsDivX = getDashDigitOffsetX('/');
        std::pair<Ogre::Real, Ogre::Real> texCoordsDivY = getDashDigitOffsetY('/');

        Ogre::Real dashDigitWidth = tachoWidth * 0.14f;
        Ogre::Real dashDigitHeight = tachoHeight * 0.25f;
        Ogre::Real dashDigitWidthSmall = dashDigitWidth * 0.8f;
        Ogre::Real dashDigitHeightSmall = dashDigitHeight * 0.8f;
        Ogre::Real dashDigitLeftGear = viewportWidth - tachoWidth - dashDigitWidth;
        Ogre::Real dashDigitTop = dashTop + dashDigitHeight * 1.1f;
        Ogre::Real dashDigitTopSmall = dashTop + dashDigitHeightSmall * 1.45f;
        mDashGear = createPanel("DashDigitGear", dashDigitWidth, dashDigitHeight, dashDigitLeftGear, dashDigitTop, "Test/DashFont");
        std::pair<Ogre::Real, Ogre::Real> texCoordsX = getDashDigitOffsetX(0);
        std::pair<Ogre::Real, Ogre::Real> texCoordsY = getDashDigitOffsetY(0);
        mDashGear->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashGear);

        Ogre::Real dashDigitLeftLap1 = viewportWidth - tachoWidth - dashDigitWidth * 20.5f;
        mDashLap1 = createPanel("DashDigitLap1", dashDigitWidth, dashDigitHeight, dashDigitLeftLap1, dashDigitTop, "Test/DashFont");
        mDashLap1->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashLap1);

        Ogre::Real dashDigitLeftLap2 = viewportWidth - tachoWidth - dashDigitWidth * 21.5f;
        mDashLap2 = createPanel("DashDigitLap2", dashDigitWidth, dashDigitHeight, dashDigitLeftLap2, dashDigitTop, "Test/DashFont");
        mDashLap2->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashLap2);

        {
            Ogre::Real dashDigitLeftLapDiv = viewportWidth - tachoWidth - dashDigitWidth * 19.5f;
            Ogre::PanelOverlayElement* tachoLapDigitDiv = createPanel("DashDigitLapDiv", dashDigitWidthSmall, dashDigitHeightSmall, dashDigitLeftLapDiv, dashDigitTopSmall, "Test/DashFont");
            tachoLapDigitDiv->setUV(texCoordsDivX.first, texCoordsDivY.first, texCoordsDivX.second, texCoordsDivY.second);
            trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(tachoLapDigitDiv);
        }

        mDashDigitLeftTotalLap1 = viewportWidth - tachoWidth - dashDigitWidth * 17.5f;
        mDashTotalLap1 = createPanel("DashDigitTotalLap1", dashDigitWidthSmall, dashDigitHeightSmall, mDashDigitLeftTotalLap1, dashDigitTopSmall, "Test/DashFont");
        mDashTotalLap1->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashTotalLap1);

        Ogre::Real dashDigitLeftTotalLap2 = viewportWidth - tachoWidth - dashDigitWidth * 18.5f;
        mDashTotalLap2 = createPanel("DashDigitTotalLap2", dashDigitWidthSmall, dashDigitHeightSmall, dashDigitLeftTotalLap2, dashDigitTopSmall, "Test/DashFont");
        mDashTotalLap2->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashTotalLap2);


        mDashLap2->hide();
        mDashTotalLap2->hide();



        mDashDigitLeftTotalCars1 = viewportWidth - tachoWidth - dashDigitWidth * 26.5f;
        mTachoTotalCarsDigit1 = createPanel("DashDigitTotalCars1", dashDigitWidthSmall, dashDigitHeightSmall, mDashDigitLeftTotalCars1, dashDigitTopSmall, "Test/DashFont");
        mTachoTotalCarsDigit1->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoTotalCarsDigit1);

        Ogre::Real dashDigitLeftTotalCars2 = viewportWidth - tachoWidth - dashDigitWidth * 27.5f;
        mTachoTotalCarsDigit2 = createPanel("DashDigitTotalCars2", dashDigitWidthSmall, dashDigitHeightSmall, dashDigitLeftTotalCars2, dashDigitTopSmall, "Test/DashFont");
        mTachoTotalCarsDigit2->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoTotalCarsDigit2);

        mTachoTotalCarsDigit2->hide();


        {
            Ogre::Real dashDigitLeftTotalCarsDiv = viewportWidth - tachoWidth - dashDigitWidth * 28.5f;
            Ogre::PanelOverlayElement* tachoTotalCarsDigitDiv = createPanel("DashDigitTotalCarsDiv", dashDigitWidthSmall, dashDigitHeightSmall, dashDigitLeftTotalCarsDiv, dashDigitTopSmall, "Test/DashFont");
            tachoTotalCarsDigitDiv->setUV(texCoordsDivX.first, texCoordsDivY.first, texCoordsDivX.second, texCoordsDivY.second);
            trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(tachoTotalCarsDigitDiv);
        }


        Ogre::Real dashDigitLeftPos1 = viewportWidth - tachoWidth - dashDigitWidth * 29.5f;
        mTachoPosDigit1 = createPanel("DashDigitPos1", dashDigitWidth, dashDigitHeight, dashDigitLeftPos1, dashDigitTop, "Test/DashFont");
        mTachoPosDigit1->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoPosDigit1);

        Ogre::Real dashDigitLeftPos2 = viewportWidth - tachoWidth - dashDigitWidth * 30.5f;
        mTachoPosDigit2 = createPanel("DashDigitPos2", dashDigitWidth, dashDigitHeight, dashDigitLeftPos2, dashDigitTop, "Test/DashFont");
        mTachoPosDigit2->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoPosDigit2);

        mTachoPosDigit2->hide();



        Ogre::Real dashDigitLeftTime1 = viewportWidth - tachoWidth - dashDigitWidth * 6.8f;
        mDashLapTime1 = createPanel("DashDigitTime1", dashDigitWidth, dashDigitHeight, dashDigitLeftTime1, dashDigitTop, "Test/DashFont");
        mDashLapTime1->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashLapTime1);

        Ogre::Real dashDigitLeftTime2 = viewportWidth - tachoWidth - dashDigitWidth * 7.8f;
        mDashLapTime2 = createPanel("DashDigitTime2", dashDigitWidth, dashDigitHeight, dashDigitLeftTime2, dashDigitTop, "Test/DashFont");
        mDashLapTime2->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashLapTime2);

        Ogre::Real dashDigitLeftTime3 = viewportWidth - tachoWidth - dashDigitWidth * 9.3f;
        mDashLapTime3 = createPanel("DashDigitTime3", dashDigitWidth, dashDigitHeight, dashDigitLeftTime3, dashDigitTop, "Test/DashFont");
        mDashLapTime3->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashLapTime3);

        Ogre::Real dashDigitLeftTime4 = viewportWidth - tachoWidth - dashDigitWidth * 10.3f;
        mDashLapTime4 = createPanel("DashDigitTime4", dashDigitWidth, dashDigitHeight, dashDigitLeftTime4, dashDigitTop, "Test/DashFont");
        mDashLapTime4->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashLapTime4);

        Ogre::Real dashDigitLeftTime5 = viewportWidth - tachoWidth - dashDigitWidth * 11.8f;
        mDashLapTime5 = createPanel("DashDigitTime5", dashDigitWidth, dashDigitHeight, dashDigitLeftTime5, dashDigitTop, "Test/DashFont");
        mDashLapTime5->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashLapTime5);

        Ogre::Real dashDigitLeftTime6 = viewportWidth - tachoWidth - dashDigitWidth * 12.8f;
        mDashLapTime6 = createPanel("DashDigitTime6", dashDigitWidth, dashDigitHeight, dashDigitLeftTime6, dashDigitTop, "Test/DashFont");
        mDashLapTime6->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mDashLapTime6);
    }

    loadMisc(gameState.getPFLoaderData());

    //paused
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalPaused");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/Paused", 
                            "Test/DiffuseTransparent", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);

        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);

        Ogre::Real pausedPanelWidth = viewportWidth / 10.0f;
        Ogre::Real pausedPanelHeight = pausedPanelWidth;
        Ogre::Real pausedPanelLeft = viewportWidth / 2.0f - pausedPanelWidth / 2.0f;
        Ogre::Real pausedPanelTop = viewportHeight / 2.0f - pausedPanelHeight / 2.0f;;

        mPausedPanel = createPanel("PausedPanel", pausedPanelWidth, pausedPanelHeight, pausedPanelLeft, pausedPanelTop, "Test/Paused");
        mPausedPanel->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mPausedPanel);
        mPausedPanel->hide();

        Ogre::Real pausedTextWidth = pausedPanelWidth / 1.5f;
        Ogre::Real pausedTextHeight = pausedPanelHeight / 10.0f;
        Ogre::Real pausedTextLeft = pausedPanelWidth / 2.0f;
        Ogre::Real pausedTextTop = pausedPanelHeight / 2.0f - pausedTextHeight / 2.0f;

        {
            Ogre::TextAreaOverlayElement * pauseText = createTextArea("PausedTextBox", pausedTextWidth, pausedTextHeight, pausedTextLeft, pausedTextTop); 
            pauseText->setCaption("PAUSED");
            pauseText->setCharHeight(16.0f);
            pauseText->setSpaceWidth(9.0f);
            pauseText->setAlignment(Ogre::TextAreaOverlayElement::Center);
            pauseText->setFontName("SdkTrays/Caption");
            pauseText->setColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));
            mPausedPanel->addChild(pauseText);
        }

    }

    //finish sign
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalFinished1");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/FinishSign1", 
                            "Test/DiffuseTransparentAnimated", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);



        texName.clear();
        texName.push_back("OriginalFinished2");
        newMat = CloneMaterial(  "Test/FinishSign2", 
                            "Test/DiffuseTransparentAnimated", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);


        texName.clear();
        texName.push_back("OriginalFinished3");
        newMat = CloneMaterial(  "Test/FinishSign3", 
                            "Test/DiffuseTransparentAnimated", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);

        texName.clear();
        texName.push_back("OriginalFinished4");
        newMat = CloneMaterial(  "Test/FinishSign4", 
                            "Test/DiffuseTransparentAnimated", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);



        Ogre::Real pausedPanelWidth = viewportWidth / 10.0f;
        Ogre::Real pausedPanelHeight = pausedPanelWidth;
        Ogre::Real pausedPanelLeft = viewportWidth / 3.0f;
        Ogre::Real pausedPanelTop = viewportHeight / 2.0f - pausedPanelHeight / 2.0f;;

        mFinishSignPanel = createPanel("FinishSignPanel", pausedPanelWidth, pausedPanelHeight, pausedPanelLeft, pausedPanelTop, "Test/FinishSign1");
        mFinishSignPanel->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mFinishSignPanel);
        mFinishSignPanel->hide();
    }

    //misc text
    {

        Ogre::Real miscPanelWidth = viewportWidth / 5.0f;
        Ogre::Real miscPanelHeight = viewportHeight / 40.0f;

        Ogre::Real miscTextWidth = miscPanelWidth / 1.5f;
        Ogre::Real miscTextHeight = miscPanelHeight / 10.0f;
        Ogre::Real miscTextLeft = 0.0f;
        Ogre::Real miscTextTop = miscTextHeight;

        mMiscText = createTextArea("MiscTextBox", miscTextWidth, miscTextHeight, miscTextLeft, miscTextTop); 
        mMiscText->setCaption("MISC");
        mMiscText->setCharHeight(16.0f);
        mMiscText->setSpaceWidth(9.0f);
        mMiscText->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mMiscText->setFontName("SdkTrays/Caption");
        mMiscText->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
        mMiscText->hide();

        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mMiscText);

        for(int q = 0; q < mMiscTextArraySize; ++q)
        {
            mMiscTextArray[q] = createTextArea("MiscTextBox" + Conversions::DMToString(q), miscTextWidth, miscTextHeight, miscTextLeft, miscTextTop + 16.0f * (q + 1)); 
            mMiscTextArray[q]->setCaption("MISC");
            mMiscTextArray[q]->setCharHeight(16.0f);
            mMiscTextArray[q]->setSpaceWidth(9.0f);
            mMiscTextArray[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mMiscTextArray[q]->setFontName("SdkTrays/Caption");
            mMiscTextArray[q]->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
            mMiscTextArray[q]->hide();

            trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mMiscTextArray[q]);
        }
    }

    mLoaded = true;

}

void Graphics2D::setVisibleTachoNeedle(bool isVisible)
{
    mNeedle->setVisible(isVisible);
}

void Graphics2D::initTachoNeedle(Ogre::SceneManager * sceneManager, const GameState& gameState)
{

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    float needleCenterX = gameState.getSTRRacecrud().getFloatValue(gameState.getPlayerCharacterName() + " dash parameters", "needle centre 1 x") / 640.0f * viewportWidth;
    float needleCenterY = gameState.getSTRRacecrud().getFloatValue(gameState.getPlayerCharacterName() + " dash parameters", "needle centre 1 y") / 480.0f * viewportHeight;

    float needleOffset = gameState.getSTRRacecrud().getFloatValue(gameState.getPlayerCharacterName() + " dash parameters", "needle offset 1") / 480.0f * viewportHeight;

    float needleWidthTop = gameState.getSTRRacecrud().getFloatValue(gameState.getPlayerCharacterName() + " dash parameters", "needle width top 1") / 640.0f * viewportWidth;
    float needleWidthBottom = gameState.getSTRRacecrud().getFloatValue(gameState.getPlayerCharacterName() + " dash parameters", "needle width bottom 1") / 640.0f * viewportWidth;
    float needleHeight = gameState.getSTRRacecrud().getFloatValue(gameState.getPlayerCharacterName() + " dash parameters", "needle size 1") / 480.0f * viewportHeight;

    std::vector<std::string> tachoNeedleStr = gameState.getSTRRacecrud().getArrayValue(gameState.getPlayerCharacterName() + " dash parameters", "needle texture region 1");
    Ogre::Vector4 tachoNeedle;
    Conversions::DMFromString(tachoNeedleStr[0], tachoNeedle.x);
    Conversions::DMFromString(tachoNeedleStr[1], tachoNeedle.y);
    Conversions::DMFromString(tachoNeedleStr[6], tachoNeedle.z);
    Conversions::DMFromString(tachoNeedleStr[7], tachoNeedle.w);
    tachoNeedle /= 255.0f;

    //http://www.ogre3d.org/tikiwiki/tiki-index.php?page=ManualObject+2D

    mNeedle = sceneManager->createManualObject("ManualTachoNeedle");

    mNeedle->begin("Test/TachoFrantic", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

    mNeedle->position(-needleWidthBottom / 2.0f, needleOffset, 0.0f);
    mNeedle->textureCoord(tachoNeedle.x, tachoNeedle.w);
    mNeedle->position(needleWidthBottom / 2.0f, needleOffset, 0.0f);
    mNeedle->textureCoord(tachoNeedle.z, tachoNeedle.w);
    mNeedle->position(needleWidthTop / 2.0f, needleHeight + needleOffset, 0.0f);
    mNeedle->textureCoord(tachoNeedle.z, tachoNeedle.y);
    mNeedle->position(-needleWidthTop / 2.0f, needleHeight + needleOffset, 0.0f);
    mNeedle->textureCoord(tachoNeedle.x, tachoNeedle.y);
     
    mNeedle->index(0);
    mNeedle->index(1);
    mNeedle->index(2);
    mNeedle->index(3);
    mNeedle->index(0);

    mNeedle->end();
     
    // Use infinite AAB to always stay visible
    Ogre::AxisAlignedBox aabInf;
    aabInf.setInfinite();
    mNeedle->setBoundingBox(aabInf);
     
    // Render just before overlays
    mNeedle->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY + 1);

     float angleMin = gameState.getSTRRacecrud().getFloatValue(gameState.getPlayerCharacterName() + " dash parameters", "revs zero angle");
     float angle10K = gameState.getSTRRacecrud().getFloatValue(gameState.getPlayerCharacterName() + " dash parameters", "revs 10k delta angle");
     //float angleMax = gameState.getSTRRacecrud().getFloatValue(gameState.getPlayerCharacterName() + " dash parameters", "revs max delta angle");
     angleMin = angleMin - 90.0f;
     angle10K = angleMin + angle10K;
     //angleMax = angleMin + angleMax;

     mEngineRPMToRotation.clear();
     mEngineRPMToRotation.addPoint(1000.0f, angleMin);
     mEngineRPMToRotation.addPoint(10000.0f, angle10K);
     //mEngineRPMToRotation.addPoint(11000.0f, angleMax);

    // Attach to scene
    Ogre::SceneNode * baseNeedle = sceneManager->getRootSceneNode()->createChildSceneNode();
    baseNeedle->setPosition(needleCenterX - viewportWidth / 2.0f, -needleCenterY + viewportHeight / 2.0f, 0.0f);
    mChildNeedle = baseNeedle->createChildSceneNode();
    mChildNeedle->attachObject(mNeedle);
}

void Graphics2D::setVisibleFinishSign(bool isVisible, size_t finishPos)
{
    if(isVisible)
    {
        switch(finishPos)
        {
        case 1:
            mFinishSignPanel->setMaterialName("Test/FinishSign1");
            break;
        case 2:
            mFinishSignPanel->setMaterialName("Test/FinishSign2");
            break;
        case 3:
            mFinishSignPanel->setMaterialName("Test/FinishSign3");
            break;
        default:
            mFinishSignPanel->setMaterialName("Test/FinishSign4");
            break;
        }
        mFinishSignPanel->show();
    }
    else
        mFinishSignPanel->hide();
}

Ogre::PanelOverlayElement* Graphics2D::createPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top, const Ogre::String& material)
{
    Ogre::PanelOverlayElement* res = NULL;

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 

    res = (Ogre::PanelOverlayElement*)om.createOverlayElement("Panel", name);
    res->setMetricsMode(Ogre::GMM_PIXELS);
    res->setWidth(width);
    res->setHeight(height);
    res->setMaterialName(material);
    res->setTop(top);
    res->setLeft(left);
    res->setHorizontalAlignment(Ogre::GHA_LEFT);
    res->setVerticalAlignment(Ogre::GVA_TOP);
    res->show();

    return res;
}

Ogre::PanelOverlayElement* Graphics2D::createPanel(const Ogre::String& name, const Ogre::Vector4& pos, const Ogre::String& material)
{
    return createPanel(name, pos.z - pos.x, pos.w - pos.y, pos.x, pos.y, material);
}

Ogre::TextAreaOverlayElement* Graphics2D::createTextArea(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top)
{
    Ogre::TextAreaOverlayElement* res = NULL;

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 

    res = (Ogre::TextAreaOverlayElement*)om.createOverlayElement("TextArea", name);
    res->setMetricsMode(Ogre::GMM_PIXELS);
    res->setWidth(width);
    res->setHeight(height);
    res->setTop(top);
    res->setLeft(left);
    res->setHorizontalAlignment(Ogre::GHA_LEFT);
    res->setVerticalAlignment(Ogre::GVA_TOP);
    res->show();

    return res;
}

void Graphics2D::reloadTextures(const GameState& gameState)
{
    loadDashboardCars(gameState);
    loadMisc(gameState.getPFLoaderData());
}

void Graphics2D::loadDashboardCars(const GameState& gameState)
{
    std::vector<std::string> availableCharacters = gameState.getSTRPowerslide().getArrayValue("", "available characters");
    for (size_t q = 0; q < availableCharacters.size(); ++q)
    {
        std::string iconName = gameState.getSTRPowerslide().getValue(availableCharacters[q] + " parameters", "icon", "car0_0s.bmp");

        //too small - should be resized to work on mobile
        //d.polubotko(TODO): consider atlas usage
        Ogre::TexturePtr chromaTexture = TextureLoader().loadChroma( gameState.getPFLoaderGameshell(), 
                                    "data/gameshell", iconName, 
                                    "chroma_" + iconName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
                                    Ogre::ColourValue::Black, 0.003f, true, 64);

        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("Test/" + iconName);
        if(!mat.isNull())
        {
            Ogre::TextureUnitState *state = mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
            state->setTexture(chromaTexture);
        }
    }
}

void Graphics2D::loadMisc(const PFLoader& pfLoaderData)
{
#if defined(__ANDROID__)
        LOGI("Graphics2D[loadMisc]: Begin"); 
#endif

    Ogre::TexturePtr chromaTexture = TextureLoader().loadChroma( pfLoaderData, 
                                "data/misc", "paused_bg.tga", 
                                "OriginalPaused", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
                                Ogre::ColourValue::Black, 0.1f);

    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("Test/Paused");
    if(!mat.isNull())
    {
        Ogre::TextureUnitState *state = mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTexture(chromaTexture);
    }

    chromaTexture = TEXLoader().load( pfLoaderData, 
                                "data/misc", "1_m_1.tex", 
                                "OriginalFinished1", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    chromaTexture = TEXLoader().load( pfLoaderData, 
                                "data/misc", "2_m_1.tex", 
                                "OriginalFinished2", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    chromaTexture = TEXLoader().load( pfLoaderData, 
                                "data/misc", "3_m_1.tex", 
                                "OriginalFinished3", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    chromaTexture = TEXLoader().load( pfLoaderData, 
                                "data/misc", "4_m_1.tex", 
                                "OriginalFinished4", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

#if defined(__ANDROID__)
        LOGI("Graphics2D[loadMisc]: End"); 
#endif
}

void Graphics2D::showBeforeStart1()
{
    mBeforeStartPanelReadyL->show();
    mBeforeStartPanelReadyC->show();
    mBeforeStartPanelReadyR->show();
}

void Graphics2D::showBeforeStart2()
{
    mBeforeStartPanelSetL->show();
    mBeforeStartPanelSetC->show();
    mBeforeStartPanelSetR->show();
}

void Graphics2D::showBeforeStart3()
{
    mBeforeStartPanelGoL->show();
    mBeforeStartPanelGoC->show();
    mBeforeStartPanelGoR->show();
}

void Graphics2D::hideAllStart()
{
    mBeforeStartPanelReadyL->hide();
    mBeforeStartPanelReadyC->hide();
    mBeforeStartPanelReadyR->hide();

    mBeforeStartPanelSetL->hide();
    mBeforeStartPanelSetC->hide();
    mBeforeStartPanelSetR->hide();

    mBeforeStartPanelGoL->hide();
    mBeforeStartPanelGoC->hide();
    mBeforeStartPanelGoR->hide();
}

void Graphics2D::setEngineRPM(Ogre::Real rpm)
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 
    Ogre::Real aspect = viewportWidth / viewportHeight;
    Ogre::Real reciprocalAspect = 1.0f / aspect;

    Ogre::Real rotationDegree = mEngineRPMToRotation.getVal(rpm);
    Ogre::Quaternion needleRot(Ogre::Degree(rotationDegree), Ogre::Vector3::UNIT_Z);
    mChildNeedle->setOrientation(needleRot);
    //mChildNeedle->setScale(1.0f, 1.0f - reciprocalAspect * Ogre::Math::Abs(Ogre::Math::Sin(Ogre::Degree(rotationDegree))), 1.0f);

    for(size_t q = 0; q < mTachoLamps.size(); ++q)
        mTachoLamps[q]->hide();

    size_t indexToShow = 0;
    bool isVisible = false;

    for(size_t q = 0; q < mTachoRange.size(); ++q)
    {
        if(rpm > mTachoRange[q])
        {
            indexToShow = q;
            isVisible = true;
        }
    }

    if(isVisible)
    {
        mTachoLamps[indexToShow]->show();
        if(mIsPickup)
            mTachoLamps[indexToShow + 1]->show();
    }
}

void Graphics2D::destroy()
{
    //Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    //om.destroy(mNeedleLayer);
}

std::pair<Ogre::Real, Ogre::Real> Graphics2D::getTachoDigitOffset(unsigned char digit)const
{
    if(digit > 9) digit = 9;

    std::pair<Ogre::Real, Ogre::Real> res = std::make_pair(0.0f, 0.0f);

    res.first = mTachoDigitsTexture.y + mTachoDigitsTexture.w * static_cast<float>(digit);
    res.second = mTachoDigitsTexture.y + mTachoDigitsTexture.w * (static_cast<float>(digit) + 1.0f);

    return res;
}

void Graphics2D::setCarSpeed(Ogre::Real speed)
{
    speed = Ogre::Math::Abs(speed);

    int speedInt = static_cast<int>(speed);
    unsigned char digit1 = speedInt % 10;
    unsigned char digit2 = (speedInt / 10) % 10;
    unsigned char digit3 = (speedInt / 100) % 10;

    mTachoSpeedDigit2->hide();
    mTachoSpeedDigit3->hide();

    std::pair<Ogre::Real, Ogre::Real> texCoords = getTachoDigitOffset(digit1);
    mTachoSpeedDigit1->setUV(texCoords.first, 1.0f - mTachoDigitsTexture.x, texCoords.second, 1.0f - mTachoDigitsTexture.z);

    if(speed >= 10.0f)
    {
        texCoords = getTachoDigitOffset(digit2);
        mTachoSpeedDigit2->setUV(texCoords.first, 1.0f - mTachoDigitsTexture.x, texCoords.second, 1.0f - mTachoDigitsTexture.z);
        mTachoSpeedDigit2->show();
    }

    if(speed >= 100.0f)
    {
        texCoords = getTachoDigitOffset(digit3);
        mTachoSpeedDigit3->setUV(texCoords.first, 1.0f - mTachoDigitsTexture.x, texCoords.second, 1.0f - mTachoDigitsTexture.z);
        mTachoSpeedDigit3->show();
    }
}

std::pair<Ogre::Real, Ogre::Real> Graphics2D::getDashDigitOffsetX(unsigned char digit)const
{
    std::pair<Ogre::Real, Ogre::Real> res = std::make_pair(0.0f, 0.0f);

    const Ogre::Real texWidth = 255.0f;
    const Ogre::Real charWidth = 37.0f / texWidth;

    if(digit >= 0 && digit <= 5)
    {
        Ogre::Real offset = static_cast<Ogre::Real>(digit);
        res.first = offset * charWidth;
        res.second = (offset + 1.0f) * charWidth;
    }

    if(digit >= 6 && digit <= 9)
    {
        Ogre::Real offset = static_cast<Ogre::Real>(digit - 6);
        res.first = offset * charWidth;
        res.second = (offset + 1.0f) * charWidth;
    }

    if(digit == '/')
    {
        Ogre::Real offset = 4.0f;
        res.first = offset * charWidth;
        res.second = (offset + 1.0f) * charWidth;
    }

    if(digit == 'r')
    {
        Ogre::Real offset = 2.0f;
        res.first = offset * charWidth;
        res.second = (offset + 1.0f) * charWidth;
    }

    if(digit == 'n')
    {
        Ogre::Real offset = 3.0f;
        res.first = offset * charWidth;
        res.second = (offset + 1.0f) * charWidth;
    }

    if(digit == '-')
    {
        Ogre::Real offset = 0.0f;
        res.first = offset * charWidth;
        res.second = (offset + 1.0f) * charWidth;
    }

    if(digit == ':')
    {
        Ogre::Real offset = 1.0f;
        res.first = offset * charWidth;
        res.second = (offset + 1.0f) * charWidth;
    }


    return res;
}

std::pair<Ogre::Real, Ogre::Real> Graphics2D::getDashDigitOffsetY(unsigned char digit)const
{
    std::pair<Ogre::Real, Ogre::Real> res = std::make_pair(0.0f, 0.0f);

    if(digit >= 0 && digit <= 5)
    {
        res.first = 0.0f;
        res.second = 0.238f;
    }

    if(digit >= 6 && digit <= 9)
    {
        res.first = 0.239f;
        res.second = 0.476f;
    }

    if(digit == '/')
    {
        res.first = 0.239f;
        res.second = 0.476f;
    }

    if(digit == 'r')
    {
        res.first = 0.477f;
        res.second = 0.714f;
    }

    if(digit == 'n')
    {
        res.first = 0.477f;
        res.second = 0.714f;
    }

    if(digit == '-')
    {
        res.first = 0.477f;
        res.second = 0.714f;
    }

    if(digit == ':')
    {
        res.first = 0.477f;
        res.second = 0.714f;
    }


    return res;
}

void Graphics2D::setCurrentLap(unsigned short lap, unsigned short totallap)
{
    if(lap < 1) lap = 1;
    if(lap > totallap) lap = totallap;

    unsigned char digit1 = lap % 10;
    unsigned char digit2 = lap / 10 % 10;

    unsigned char digittotal1 = totallap % 10;
    unsigned char digittotal2 = totallap / 10 % 10;

    mDashLap2->hide();
    mDashTotalLap2->hide();

    std::pair<Ogre::Real, Ogre::Real> texCoordsX = getDashDigitOffsetX(digit1);
    std::pair<Ogre::Real, Ogre::Real> texCoordsY = getDashDigitOffsetY(digit1);

    mDashLap1->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

    texCoordsX = getDashDigitOffsetX(digittotal1);
    texCoordsY = getDashDigitOffsetY(digittotal1);

    mDashTotalLap1->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
    mDashTotalLap1->setLeft(mDashTotalLap2->getLeft());

    if(lap > 9)
    {
        texCoordsX = getDashDigitOffsetX(digit2);
        texCoordsY = getDashDigitOffsetY(digit2);

        mDashLap2->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

        mDashLap2->show();
    }

    if(totallap > 9)
    {
        texCoordsX = getDashDigitOffsetX(digittotal2);
        texCoordsY = getDashDigitOffsetY(digittotal2);

        mDashTotalLap2->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

        mDashTotalLap2->show();

        mDashTotalLap1->setLeft(mDashDigitLeftTotalLap1);
    }

}

void Graphics2D::setCarGear(unsigned char gear)
{
    if(gear == 255) gear = 'r';

    std::pair<Ogre::Real, Ogre::Real> texCoordsX = getDashDigitOffsetX(gear);
    std::pair<Ogre::Real, Ogre::Real> texCoordsY = getDashDigitOffsetY(gear);
    mDashGear->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
}

void Graphics2D::setCarPos(unsigned char pos, unsigned char totalcars)
{
    mTachoTotalCarsDigit2->hide();
    mTachoPosDigit2->hide();

    unsigned char digitpos1 = pos % 10;
    unsigned char digitpos2 = pos / 10 % 10;

    unsigned char digittotal1 = totalcars % 10;
    unsigned char digittotal2 = totalcars / 10 % 10;

    std::pair<Ogre::Real, Ogre::Real> texCoordsX = getDashDigitOffsetX(digitpos1);
    std::pair<Ogre::Real, Ogre::Real> texCoordsY = getDashDigitOffsetY(digitpos1);

    mTachoPosDigit1->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

    if(pos >= 10)
    {
        texCoordsX = getDashDigitOffsetX(digitpos2);
        texCoordsY = getDashDigitOffsetY(digitpos2);

        mTachoPosDigit2->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

        mTachoPosDigit2->show();
    }

    texCoordsX = getDashDigitOffsetX(digittotal1);
    texCoordsY = getDashDigitOffsetY(digittotal1);

    mTachoTotalCarsDigit1->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);
    mTachoTotalCarsDigit1->setLeft(mTachoTotalCarsDigit2->getLeft());

    if(totalcars >= 10)
    {
        texCoordsX = getDashDigitOffsetX(digittotal2);
        texCoordsY = getDashDigitOffsetY(digittotal2);

        mTachoTotalCarsDigit2->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

        mTachoTotalCarsDigit2->show();

        mTachoTotalCarsDigit1->setLeft(mDashDigitLeftTotalCars1);
    }
}

void Graphics2D::hideAIDashboardCars()
{
    for(size_t q = 0; q < mDashboardCarsCount; ++q)
    {
        mDashboardCars[q]->hide();
    }
}

void Graphics2D::setPlayerDashBoardSkin(const GameState& gameState)
{
    std::string iconName = gameState.getSTRPowerslide().getValue(gameState.getPlayerCharacterName() + " parameters", "icon", "car0_0s.bmp");
    std::string matName = "Test/" + iconName;
    if(matName != mPlayerDashboardCar->getMaterialName())
        mPlayerDashboardCar->setMaterialName(matName);
}

void Graphics2D::setAIDashBoardSkin(const GameState& gameState, size_t aiDashIndex, const std::string& characterName)
{
    std::string iconName = gameState.getSTRPowerslide().getValue(characterName + " parameters", "icon", "car0_0s.bmp");
    std::string matName = "Test/" + iconName;

    if(aiDashIndex < mDashboardCarsCount)
    {
        if(matName != mDashboardCars[aiDashIndex]->getMaterialName())
            mDashboardCars[aiDashIndex]->setMaterialName(matName);
    }
}

void Graphics2D::setDashCarPos(size_t aiDashIndex, size_t playerLap, Ogre::Real playerLapPos, size_t aiLap, Ogre::Real aiLapPos)
{
    if(aiDashIndex < mDashboardCarsCount)
    {
        //if(playerLap == 0) playerLap = 1;
        //if(aiLap == 0) aiLap = 1;

        Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
        const Ogre::Real viewportWidth = om.getViewportWidth(); 
        const Ogre::Real dashWidth = viewportWidth;
        const Ogre::Real dashCarWidth = dashWidth / 18.0f;

        Ogre::Real fracToFull = 1.0f - playerLapPos;
        aiLapPos += fracToFull;
        if(aiLapPos >= 1.0f)
        {
            ++aiLap;
            aiLapPos -= 1.0f;
        }

        const Ogre::Real dashCarLeftCenter = dashWidth / 3.0f + dashCarWidth;
        Ogre::Real dashCarLeft = dashCarLeftCenter;

        if(aiLap == playerLap)
        {
            dashCarLeft *= aiLapPos;
        }
        if(aiLap > playerLap)
        {
            dashCarLeft += dashCarLeft * aiLapPos;
        }

        if(aiLap == playerLap || aiLap == (playerLap + 1))
        {
            mDashboardCars[aiDashIndex]->setLeft(dashCarLeft);
            mDashboardCars[aiDashIndex]->show();
        }
    }
}

void Graphics2D::setRaceTime(const std::string& time)
{
    mDashLapTime6->hide();

    const unsigned char digit1 = time[7] - '0';
    const unsigned char digit2 = time[6] - '0';
    const unsigned char digit3 = time[4] - '0';
    const unsigned char digit4 = time[3] - '0';
    const unsigned char digit5 = time[1] - '0';
    const unsigned char digit6 = time[0] - '0';

    std::pair<Ogre::Real, Ogre::Real> texCoordsX = getDashDigitOffsetX(digit1);
    std::pair<Ogre::Real, Ogre::Real> texCoordsY = getDashDigitOffsetY(digit1);

    mDashLapTime1->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

    texCoordsX = getDashDigitOffsetX(digit2);
    texCoordsY = getDashDigitOffsetY(digit2);

    mDashLapTime2->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

    texCoordsX = getDashDigitOffsetX(digit3);
    texCoordsY = getDashDigitOffsetY(digit3);

    mDashLapTime3->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

    texCoordsX = getDashDigitOffsetX(digit4);
    texCoordsY = getDashDigitOffsetY(digit4);

    mDashLapTime4->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

    texCoordsX = getDashDigitOffsetX(digit5);
    texCoordsY = getDashDigitOffsetY(digit5);

    mDashLapTime5->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

    if(digit6 > 0)
    {
        texCoordsX = getDashDigitOffsetX(digit6);
        texCoordsY = getDashDigitOffsetY(digit6);

        mDashLapTime6->setUV(texCoordsX.first, texCoordsY.first, texCoordsX.second, texCoordsY.second);

        mDashLapTime6->show();
    }
}

void Graphics2D::createRearViewMirrorPanelTexture(BaseApp* baseApp, Ogre::Root * root, int width, int height)
{
    Ogre::TexturePtr tex = root->getTextureManager()->createManual(
        "RearViewMirrorTex",
        TEMP_RESOURCE_GROUP_NAME,
        Ogre::TEX_TYPE_2D,
        width,
        height,
        0,
        Ogre::PF_R8G8B8,
        Ogre::TU_RENDERTARGET);
    mRearCamTexture = tex->getBuffer()->getRenderTarget();
    mRearCamTexture->addListener(baseApp);
    
    std::vector<Ogre::String> texName;
    texName.push_back("RearViewMirrorTex");
    Ogre::MaterialPtr newMat = CloneMaterial(  "Test/RearViewMirror", 
                        "Test/Diffuse", 
                        texName, 
                        1.0f,
                        TEMP_RESOURCE_GROUP_NAME);

    Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    state->setTextureScale(-1.0f, 1.0f);
}

void Graphics2D::rearViewMirrorPanelTextureRemoveAllViewports()
{
    mRearCamTexture->removeAllViewports();
}

Ogre::Viewport * Graphics2D::rearViewMirrorPanelTextureAddViewport(Ogre::Camera* camera)
{
    return mRearCamTexture->addViewport(camera);
}

void Graphics2D::createRearViewMirrorPanel(CustomTrayManager* trayMgr, bool isPanelEnabled)
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth();
    Ogre::Real viewportHeight = om.getViewportHeight();

    Ogre::Real panelWidth = viewportWidth / 3.0f;
    Ogre::Real panelHeight = viewportHeight / 8.0f;

    Ogre::Real panelLeft = viewportWidth / 2.0f - panelWidth / 2.0f;

    mRearViewMirrorPanel = createPanel("MirrorPanel", panelWidth, panelHeight, panelLeft, 0.0f, "BaseWhite");

    mRearViewMirrorPanel->setUV(0.0f, 0.0f, 1.0f, 1.0f);

    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mRearViewMirrorPanel);

    if(isPanelEnabled)
    {
        mRearViewMirrorPanel->setMaterialName("Test/RearViewMirror");
    }
    else
    {
        mRearViewMirrorPanel->hide();
    }
}

void Graphics2D::setRearViewMirrorPanelMaterial(const Ogre::String& material)
{
    if(mRearViewMirrorPanel)
    {
        mRearViewMirrorPanel->setMaterialName(material);
        mRearViewMirrorPanel->hide();
    }
}

void Graphics2D::setRearViewMirrorPanelShow(bool isShow)
{
    if(mRearViewMirrorPanel && isShow)
    {
        mRearViewMirrorPanel->show();
    }

    if(mRearViewMirrorPanel && !isShow)
    {
        mRearViewMirrorPanel->hide();
    }
}

void Graphics2D::setShowPausedPanel(bool isShow)
{
    if(isShow)
    {
        mPausedPanel->show();
    }
    else
    {
        mPausedPanel->hide();
    }
}

void Graphics2D::setShowMiscText(bool isShow)
{
    if(isShow)
    {
        mMiscText->show();
    }
    else
    {
        mMiscText->hide();
    }
}

void Graphics2D::setMiscText(const std::string& text)
{
    mMiscText->setCaption(text);
}

void Graphics2D::addMiscPanelText(const std::string& text, const Ogre::ColourValue& col)
{
    if(mLoaded)
    {
        for(int q = mMiscTextArraySize - 1; q >0; --q)
        {
            if(mMiscTextArray[q - 1]->isVisible())
            {
                mMiscTextArray[q]->show();
            }
            mMiscTextArray[q]->setColour(mMiscTextArray[q - 1]->getColour());
            mMiscTextArray[q]->setCaption(mMiscTextArray[q - 1]->getCaption());
        }

        mMiscTextArray[0]->setColour(col);
        mMiscTextArray[0]->setCaption(text);
        mMiscTextArray[0]->show();
    }
}

void Graphics2D::clearMiscPanelText()
{
    for(int q = 0; q < mMiscTextArraySize; ++q)
    {
        mMiscTextArray[q]->hide();
    }
}