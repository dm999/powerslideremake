#include "pcheader.h"

#include "Graphics2D.h"

#include "OgreTools.h"

#include "CustomTrayManager.h"

#include "Conversions.h"
#include "GameState.h"

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

    mNeedleScaleCorrection.addPoint(-90.0f, 1.0f);
    mNeedleScaleCorrection.addPoint(0.0f, 0.0f);
    mNeedleScaleCorrection.addPoint(90.0f, 1.0f);
}

void Graphics2D::load(  CustomTrayManager* trayMgr, 
                        const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell,
                        const STRRacecrud& mSTRRacecrud)
{

    //startlight
    {
        TEXLoader().load(pfLoaderData, "data/misc/startlights", "ready_left_m_2.tex", "OriginalStartReadyL", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(pfLoaderData, "data/misc/startlights", "ready_centre_m_2.tex", "OriginalStartReadyC", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(pfLoaderData, "data/misc/startlights", "ready_right_m_2.tex", "OriginalStartReadyR", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

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
        TEXLoader().load(pfLoaderData, "data/misc/startlights", "set_left_m_2.tex", "OriginalStartSetL", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(pfLoaderData, "data/misc/startlights", "ready_centre_m_2.tex", "OriginalStartSetC", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(pfLoaderData, "data/misc/startlights", "set_right_m_2.tex", "OriginalStartSetR", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

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
        TEXLoader().load(pfLoaderData, "data/misc/startlights", "go_left_m_2.tex", "OriginalStartGoL", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(pfLoaderData, "data/misc/startlights", "go_centre_m_2.tex", "OriginalStartGoC", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TEXLoader().load(pfLoaderData, "data/misc/startlights", "go_right_m_2.tex", "OriginalStartGoR", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

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

    Ogre::Real startPanelWidth = viewportWidth / 9.0f;
    Ogre::Real startPanelHeight = viewportHeight / 7.0f;
    Ogre::Real startPanelLeft = viewportWidth / 2.0f - startPanelWidth * 3.0f / 2.0f;

    Ogre::Vector4 readyL = mSTRRacecrud.getArray4Value("on-grid parameters", "ready left texture region");
    readyL /= 255.0f;
    mBeforeStartPanelReadyL = createPanel("BeforeStartReadyL", startPanelWidth, startPanelHeight, startPanelLeft, 0.0f, "Test/BeforeStartReadyL");
    mBeforeStartPanelReadyL->setUV(readyL.x, readyL.y, readyL.z, readyL.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelReadyL);

    Ogre::Vector4 readyC = mSTRRacecrud.getArray4Value("on-grid parameters", "ready centre texture region");
    readyC /= 255.0f;
    mBeforeStartPanelReadyC = createPanel("BeforeStartReadyC", startPanelWidth, startPanelHeight, startPanelLeft + startPanelWidth, 0.0f, "Test/BeforeStartReadyC");
    mBeforeStartPanelReadyC->setUV(readyC.x, readyC.y, readyC.z, readyC.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelReadyC);

    Ogre::Vector4 readyR = mSTRRacecrud.getArray4Value("on-grid parameters", "ready right texture region");
    readyR /= 255.0f;
    mBeforeStartPanelReadyR = createPanel("BeforeStartReadyR", startPanelWidth, startPanelHeight, startPanelLeft + startPanelWidth * 2.0f, 0.0f, "Test/BeforeStartReadyR");
    mBeforeStartPanelReadyR->setUV(readyR.x, readyR.y, readyR.z, readyR.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelReadyR);

    Ogre::Vector4 setL = mSTRRacecrud.getArray4Value("on-grid parameters", "set left texture region");
    setL /= 255.0f;
    mBeforeStartPanelSetL = createPanel("BeforeStartSetL", startPanelWidth, startPanelHeight, startPanelLeft, 0.0f, "Test/BeforeStartSetL");
    mBeforeStartPanelSetL->setUV(setL.x, setL.y, setL.z, setL.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelSetL);

    Ogre::Vector4 setC = mSTRRacecrud.getArray4Value("on-grid parameters", "ready centre texture region");
    setC /= 255.0f;
    mBeforeStartPanelSetC = createPanel("BeforeStartSetC", startPanelWidth, startPanelHeight, startPanelLeft + startPanelWidth, 0.0f, "Test/BeforeStartSetC");
    mBeforeStartPanelSetC->setUV(setC.x, setC.y, setC.z, setC.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelSetC);

    Ogre::Vector4 setR = mSTRRacecrud.getArray4Value("on-grid parameters", "set right texture region");
    setR /= 255.0f;
    mBeforeStartPanelSetR = createPanel("BeforeStartSetR", startPanelWidth, startPanelHeight, startPanelLeft + startPanelWidth * 2.0f, 0.0f, "Test/BeforeStartSetR");
    mBeforeStartPanelSetR->setUV(setR.x, setR.y, setR.z, setR.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelSetR);

    Ogre::Vector4 goL = mSTRRacecrud.getArray4Value("on-grid parameters", "go left texture region");
    goL /= 255.0f;
    mBeforeStartPanelGoL = createPanel("BeforeStartGoL", startPanelWidth, startPanelHeight, startPanelLeft, 0.0f, "Test/BeforeStartGoL");
    mBeforeStartPanelGoL->setUV(goL.x, goL.y, goL.z, goL.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelGoL);

    Ogre::Vector4 goC = mSTRRacecrud.getArray4Value("on-grid parameters", "go centre texture region");
    goC /= 255.0f;
    mBeforeStartPanelGoC = createPanel("BeforeStartGoC", startPanelWidth, startPanelHeight, startPanelLeft + startPanelWidth, 0.0f, "Test/BeforeStartGoC");
    mBeforeStartPanelGoC->setUV(goC.x, goC.y, goC.z, goC.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelGoC);

    Ogre::Vector4 goR = mSTRRacecrud.getArray4Value("on-grid parameters", "go right texture region");
    goR /= 255.0f;
    mBeforeStartPanelGoR = createPanel("BeforeStartGoR", startPanelWidth, startPanelHeight, startPanelLeft + startPanelWidth * 2.0f, 0.0f, "Test/BeforeStartGoR");
    mBeforeStartPanelGoR->setUV(goR.x, goR.y, goR.z, goR.w);
    trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBeforeStartPanelGoR);

    hideAllStart();

    //dashboard and tacho

    Ogre::Real tachoWidth = viewportWidth / 5.7f;
    Ogre::Real tachoHeight = tachoWidth;

    //dashboard
    {
        //d.polubotko(TODO): refactor (remove absolute path)
        TextureLoader().load(pfLoaderData, "data/misc/dashes", "max_dash.tga", "OriginalDash", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        std::vector<Ogre::String> texName;
        texName.push_back("OriginalDash");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/DashboardWarthog", 
                            "Test/Diffuse", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }


    Ogre::Real dashWidth = viewportWidth;
    Ogre::Real dashHeight = tachoHeight / 1.7f;

    Ogre::Real dashTop = viewportHeight - dashHeight;

    {
        Ogre::PanelOverlayElement* dashboard = createPanel("Dashboard", dashWidth, dashHeight, 0.0f, dashTop, "Test/DashboardWarthog");
        dashboard->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(dashboard);
    }


    //dashboard cars
    {
        loadDashboardCars(pfLoaderGameshell);

        for ( int carsEnum = Warthog_0; carsEnum <= Pickup_2; carsEnum++ )
        {
            GameCars carEnum = static_cast<GameCars>(carsEnum);

            std::string fileName = GameState::getDashSkinByCarEnum(carEnum);

            std::vector<Ogre::String> texName;
            texName.push_back("chroma_" + fileName);
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/" + fileName, 
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
        //d.polubotko(TODO): refactor (remove absolute path)
        TEXLoader().load(pfLoaderData, "data/misc/tachos", "frantic_1024x768_m_1.tex", "OriginalTacho", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

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
        Ogre::PanelOverlayElement* tacho = createPanel("TachoFrantic", tachoWidth, tachoHeight, tachoLeft, tachoTop, "Test/TachoFrantic");
        tacho->setUV(0.0f, 0.008f, 0.703f, 0.649f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(tacho);
    }

    //tacho needle
    {
        const Ogre::Real needleWidth = tachoWidth * 0.047f;
        const Ogre::Real needleHeight = tachoHeight * 0.283f;

        mNeedleLayer = om.create("TachoNeedle");
        mNeedleLayer->setZOrder(201);
        Ogre::OverlayContainer* pContainer = (Ogre::OverlayContainer*)om.createOverlayElement("Panel", "TachoNeedleContainer");
        pContainer->setMetricsMode(Ogre::GMM_PIXELS);
        pContainer->setMaterialName("Test/TachoFrantic");
        pContainer->setLeft(viewportWidth / 2.0f - needleWidth / 4.0f);
        pContainer->setTop(viewportHeight / 2.0f - needleHeight * 0.79f);
        pContainer->setWidth(needleWidth);
        pContainer->setHeight(needleHeight);
        static_cast<Ogre::PanelOverlayElement*>(pContainer)->setUV(0.796f, 0.15f, 0.82f, 0.38f);
        mNeedleLayer->add2D(pContainer);
        mNeedleLayer->show();
        mNeedleLayer->setScroll((tachoLeft + needleWidth * 2.0f) / viewportWidth, -(tachoTop)/ viewportHeight);
    }

    //tacho lamps
    {
        Ogre::Real tachoLampWidth = tachoWidth * 0.364f;
        Ogre::Real tachoLampHeight = tachoHeight * 0.233f;
        Ogre::Real tachoLampLeft = viewportWidth - tachoLampWidth - tachoLampWidth * 0.157f;
        Ogre::Real tachoLampTop = viewportHeight - tachoHeight / 2.0f - tachoLampHeight / 2.0f + tachoLampHeight * 0.02f;
        mTachoLamps = createPanel("TachoLamps", tachoLampWidth, tachoLampHeight, tachoLampLeft, tachoLampTop, "Test/TachoFrantic");
        mTachoLamps->setUV(0.0f, 0.66f, 0.257f, 0.81f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoLamps);
        mTachoLamps->hide();
    }

    //tacho digits
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalTacho");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/TachoFranticDigits", 
                            "Test/DiffuseTransparent", 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureRotate(Ogre::Degree(90.0f));

        Ogre::Real tachoDigitWidth = tachoWidth * 0.08f;
        Ogre::Real tachoDigitHeight = tachoHeight * 0.1f;
        Ogre::Real tachoDigitLeft1 = viewportWidth - tachoWidth / 2.0f + tachoDigitWidth * 1.4f;
        Ogre::Real tachoDigitTop = viewportHeight - tachoHeight / 2.0f + tachoDigitHeight * 0.9f;
        mTachoSpeedDigit1 = createPanel("TachoDigit1", tachoDigitWidth, tachoDigitHeight, tachoDigitLeft1, tachoDigitTop, "Test/TachoFranticDigits");
        std::pair<Ogre::Real, Ogre::Real> texCoords = getTachoDigitOffset(0);
        mTachoSpeedDigit1->setUV(texCoords.first, 0.216f, texCoords.second, 0.298f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoSpeedDigit1);

        Ogre::Real tachoDigitLeft2 = viewportWidth - tachoWidth / 2.0f + tachoDigitWidth / 2.0f;
        mTachoSpeedDigit2 = createPanel("TachoDigit2", tachoDigitWidth, tachoDigitHeight, tachoDigitLeft2, tachoDigitTop, "Test/TachoFranticDigits");
        mTachoSpeedDigit2->setUV(texCoords.first, 0.216f, texCoords.second, 0.298f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoSpeedDigit2);

        Ogre::Real tachoDigitLeft3 = viewportWidth - tachoWidth / 2.0f - tachoDigitWidth * 0.4f;
        mTachoSpeedDigit3 = createPanel("TachoDigit3", tachoDigitWidth, tachoDigitHeight, tachoDigitLeft3, tachoDigitTop, "Test/TachoFranticDigits");
        mTachoSpeedDigit3->setUV(texCoords.first, 0.216f, texCoords.second, 0.298f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mTachoSpeedDigit3);


        //mTachoSpeedDigit1->hide();
        mTachoSpeedDigit2->hide();
        mTachoSpeedDigit3->hide();
    }


    //font
    {
        TEXLoader().load(pfLoaderData, "data/misc", "font_set_1_m_1.tex", "OriginalFont", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

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


    //paused
    {
        loadMisc(pfLoaderData);

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

void Graphics2D::reloadTextures(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell)
{
    loadDashboardCars(pfLoaderGameshell);
    loadMisc(pfLoaderData);
}

void Graphics2D::loadDashboardCars(const PFLoader& pfLoaderGameshell)
{
    for ( int carsEnum = Warthog_0; carsEnum <= Pickup_2; carsEnum++ )
    {
        GameCars carEnum = static_cast<GameCars>(carsEnum);

        std::string fileName = GameState::getDashSkinByCarEnum(carEnum);

        //too small - should be resized to work on mobile
        //d.polubotko(TODO): consider atlas usage
        Ogre::TexturePtr chromaTexture = TextureLoader().loadChroma( pfLoaderGameshell, 
                                    "data/gameshell", fileName, 
                                    "chroma_" + fileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
                                    Ogre::ColourValue::Black, 0.003f, true, 64);

        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("Test/" + fileName);
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
    Ogre::Real heightWidthRelation = viewportHeight / viewportWidth;

    Ogre::Real rotationDegree = mEngineRPMToRotation.getVal(rpm);
    mNeedleLayer->setRotate(Ogre::Degree(rotationDegree));
    mNeedleLayer->setScale(1.0f, 1.0f * (1.0f - mNeedleScaleCorrection.getVal(rotationDegree)) + heightWidthRelation * mNeedleScaleCorrection.getVal(rotationDegree));

    if(rpm < 3000.0f)
    {
        mTachoLamps->hide();
    }
    else
    {
        mTachoLamps->show();

        if(rpm < 7000.0f)       // green
        {
            mTachoLamps->setUV(0.259f, 0.66f, 0.514f, 0.81f);
        }
        else
        {
            if(rpm < 8500.0f)   // yellow
            {
                mTachoLamps->setUV(0.0f, 0.66f, 0.257f, 0.81f);
            }
            else                // red
            {
                mTachoLamps->setUV(0.518f, 0.66f, 0.771f, 0.81f);
            }
        }
    }
}

void Graphics2D::destroy()
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    om.destroy(mNeedleLayer);
}

std::pair<Ogre::Real, Ogre::Real> Graphics2D::getTachoDigitOffset(unsigned char digit)const
{
    if(digit > 9) digit = 9;

    std::pair<Ogre::Real, Ogre::Real> res = std::make_pair(0.0f, 0.0f);

    Ogre::Real charNumber = digit;
    const Ogre::Real charWidth = 14.0f;
    const Ogre::Real texWidth = 255.0f;
    const Ogre::Real charOffset = 1.0f / texWidth;

    res.first = charOffset + (charWidth * charNumber) / texWidth;
    res.second = charOffset + (charWidth * (charNumber + 1.0f)) / texWidth;

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

    const Ogre::Real charTop = 0.218f;
    const Ogre::Real charBottom = 0.296f;

    std::pair<Ogre::Real, Ogre::Real> texCoords = getTachoDigitOffset(digit1);
    mTachoSpeedDigit1->setUV(texCoords.first, charTop, texCoords.second, charBottom);

    if(speed >= 10.0f)
    {
        texCoords = getTachoDigitOffset(digit2);
        mTachoSpeedDigit2->setUV(texCoords.first, charTop, texCoords.second, charBottom);
        mTachoSpeedDigit2->show();
    }

    if(speed >= 100.0f)
    {
        texCoords = getTachoDigitOffset(digit3);
        mTachoSpeedDigit3->setUV(texCoords.first, charTop, texCoords.second, charBottom);
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

void Graphics2D::setPlayerDashBoardSkin(GameCars carEnum)
{
    std::string fileName = GameState::getDashSkinByCarEnum(carEnum);
    std::string matName = "Test/" + fileName;
    if(matName != mPlayerDashboardCar->getMaterialName())
        mPlayerDashboardCar->setMaterialName(matName);
}

void Graphics2D::setAIDashBoardSkin(size_t aiDashIndex, GameCars carEnum)
{
    std::string fileName = GameState::getDashSkinByCarEnum(carEnum);
    std::string matName = "Test/" + fileName;

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

void Graphics2D::createRearViewMirrorPanelTexture(Ogre::Root * root, int width, int height)
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