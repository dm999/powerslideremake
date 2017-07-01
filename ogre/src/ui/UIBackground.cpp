#include "../pcheader.h"

#include "UIBackground.h"

#include "../GameState.h"

#include "../loaders/TextureLoader.h"

#include "../tools/OgreTools.h"

#include "../customs/CustomTrayManager.h"
#include "../customs/CustomOverlaySystem.h"


namespace
{
#if defined(__ANDROID__)
    const char vert[] = "               \n\
    #version 100                        \n\
    uniform mat4 modelviewproj;         \n\
    uniform mat4 texturematrix;         \n\
    attribute vec4 position;            \n\
    attribute vec2 uv0;                 \n\
    varying vec2 T;                     \n\
    void main()                         \n\
    {                                                   \n\
        T = (texturematrix * vec4(uv0, 0.0, 1.0)).xy;   \n\
        gl_Position = modelviewproj * position;         \n\
    }                                                   \n\
    ";

    const char  frag[] = "                              \n\
    #version 100                                        \n\
    precision highp float;                              \n\
    uniform sampler2D diffuseMap;                       \n\
    varying vec2 T;                                     \n\
    void main()                                         \n\
    {                                                   \n\
        gl_FragColor = vec4(texture2D(diffuseMap, T));  \n\
    }                                                   \n\
    ";

    const char lang[] = "glsles";
#else
    const char vert[] = "               \n\
    #version 120                        \n\
    uniform mat4 modelviewproj;         \n\
    uniform mat4 texturematrix;         \n\
    attribute vec4 position;            \n\
    attribute vec2 uv0;                 \n\
    varying vec2 T;                     \n\
    void main()                         \n\
    {                                                   \n\
        T = (texturematrix * vec4(uv0, 0.0, 1.0)).xy;   \n\
        gl_Position = modelviewproj * position;         \n\
    }                                                   \n\
    ";

    const char  frag[] = "                              \n\
    #version 120                                        \n\
    uniform sampler2D diffuseMap;                       \n\
    varying vec2 T;                                     \n\
    void main()                                         \n\
    {                                                   \n\
        gl_FragColor = vec4(texture2D(diffuseMap, T));  \n\
    }                                                   \n\
    ";

    const char lang[] = "glsl";
#endif
}

Ogre::NameGenerator UIBackground::nameGenMaterials("Loaders/Material");
Ogre::NameGenerator UIBackground::nameGenTextures("Loaders/Texture");


UIBackground::UIBackground(const ModeContext& modeContext, 
                           const PFLoader& loader,
                           const std::string& path, const std::string& fileName) :
    mModeContext(modeContext)
{
    mMaterialName = nameGenMaterials.generate();

    Ogre::String textureName = nameGenTextures.generate();

    TextureLoader().load(   loader, 
                            path, fileName, 
                            textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    mMaterial = Ogre::MaterialManager::getSingleton().create(mMaterialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    Ogre::HighLevelGpuProgramPtr vertex = Ogre::HighLevelGpuProgramManager::getSingletonPtr()->createProgram( "loader_basicVS", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, lang, Ogre::GPT_VERTEX_PROGRAM);
    vertex->setSource(vert);

    Ogre::HighLevelGpuProgramPtr fragment = Ogre::HighLevelGpuProgramManager::getSingletonPtr()->createProgram( "loader_basicFS", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, lang, Ogre::GPT_FRAGMENT_PROGRAM);
    fragment->setSource(frag);

    Ogre::Pass *pass = mMaterial->getTechnique(0)->getPass(0);

    pass->setVertexProgram( vertex->getName() );
    pass->setFragmentProgram( fragment->getName() );

    Ogre::GpuProgramParametersSharedPtr params_v = pass->getVertexProgramParameters();
    params_v->setNamedAutoConstant("modelviewproj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    params_v->setNamedAutoConstant("texturematrix", Ogre::GpuProgramParameters::ACT_TEXTURE_MATRIX);

    Ogre::GpuProgramParametersSharedPtr params_f = pass->getFragmentProgramParameters();
    params_f->setNamedConstant("diffuseMap", 0);

    mMaterial->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
    mMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);

    mMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(textureName);

    Ogre::TextureUnitState *state = mMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);

}

Ogre::PanelOverlayElement* UIBackground::createPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top, const Ogre::String& material)
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

    return res;
}

Ogre::PanelOverlayElement* UIBackground::createPanel(const Ogre::String& name, const Ogre::Vector4& pos, const Ogre::String& material)
{
    return createPanel(name, pos.z - pos.x, pos.w - pos.y, pos.x, pos.y, material);
}

Ogre::TextAreaOverlayElement* UIBackground::createTextArea(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top)
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


void UIBackground::show()
{
    createCamera();

    mModeContext.mWindow->update(true);// update viewport sizes for Ogre::OverlayManager

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    mLoaderScreen = createPanel("LoaderScreen", viewportWidth, viewportHeight, 0.0f, 0.0f, mMaterialName);
    mLoaderScreen->setUV(0.0f, 0.0f, 1.0f, 1.0f);

    mModeContext.getTrayManager()->getTrayContainer(OgreBites::TL_NONE)->addChild(mLoaderScreen);
    mLoaderScreen->show();
    mModeContext.mWindow->update(true);// update draw
}

void UIBackground::hide()
{
    mLoaderScreen->hide();
    mModeContext.getTrayManager()->getTrayContainer(OgreBites::TL_NONE)->removeChild(mLoaderScreen->getName());

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    om.destroyOverlayElement(mLoaderScreen);

    destroyCamera();
}

void UIBackground::createCamera()
{
    mSceneMgr = mModeContext.mRoot->createSceneManager(Ogre::ST_GENERIC);
    mSceneMgr->addRenderQueueListener(mModeContext.mOverlaySystem);
    mCamera = mSceneMgr->createCamera("LoadScreenCam");
    Ogre::Viewport * mViewPort = mModeContext.mWindow->addViewport(mCamera);
    mCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    mCamera->setOrthoWindow(static_cast<float>(mViewPort->getActualWidth()), static_cast<float>(mViewPort->getActualHeight()));
    mCamera->setNearClipDistance(0.5f);
    mCamera->setFarClipDistance(10000.0f);
    mCamera->setFOVy(Ogre::Degree(45.0f));
    mCamera->setPosition(0.0f, 0.0f, 100.0f);
    mCamera->lookAt(Ogre::Vector3::ZERO);

}

void UIBackground::destroyCamera()
{
    mSceneMgr->clearScene();
    mModeContext.mRoot->destroySceneManager(mSceneMgr);
    mModeContext.mWindow->removeAllViewports();
}


UIBackgroundLoaderProgress::UIBackgroundLoaderProgress(const ModeContext& modeContext, 
                           const PFLoader& loader,
                           const std::string& path, const std::string& fileName,
                           float progressTop, float progressBottom, float progressLeft, float progressRight) :
    UIBackground(modeContext, loader, path, fileName),
    mProgressTop(progressTop), mProgressBottom(progressBottom), mProgressLeft(progressLeft), mProgressRight(progressRight),
    mPercent(0.0f)
{
    Ogre::String textureNameEnd = nameGenTextures.generate();
    Ogre::String textureNameMiddle = nameGenTextures.generate();

    mMaterialNameEnd = nameGenMaterials.generate();
    mMaterialNameMiddle = nameGenMaterials.generate();

    TextureLoader().load(   loader, 
                            "data/misc/loading", "end.tga", 
                            textureNameEnd, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    TextureLoader().load(   loader, 
                            "data/misc/loading", "middle.tga", 
                            textureNameMiddle, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    {
        std::vector<Ogre::String> texName;
        texName.push_back(textureNameEnd);

        Ogre::MaterialPtr newMat = CloneMaterial(  mMaterialNameEnd, 
                            mMaterialName, 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
    }

    {
        std::vector<Ogre::String> texName;
        texName.push_back(textureNameMiddle);

        Ogre::MaterialPtr newMat = CloneMaterial(  mMaterialNameMiddle, 
                            mMaterialName, 
                            texName, 
                            1.0f,
                            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
    }

}

void UIBackgroundLoaderProgress::show()
{
    UIBackground::show();

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    Ogre::Matrix4 screenAdaptionRelative(
        viewportWidth / 640.0f, 0.0f,                       0.0f,                   0.0f,
        0.0f,                   viewportHeight / 480.0f,    0.0f,                   0.0f,
        0.0f,                   0.0f,                       viewportWidth / 640.0f, 0.0f,
        0.0f,                   0.0f,                       0.0f,                   viewportHeight / 480.0f);

    Ogre::Vector4 beginPos = screenAdaptionRelative * Ogre::Vector4(mProgressLeft - 16.0f, mProgressTop, mProgressLeft, mProgressBottom);
    Ogre::Vector4 middlePos = screenAdaptionRelative * Ogre::Vector4(mProgressLeft, mProgressTop, mProgressLeft + 1.0f, mProgressBottom);
    Ogre::Vector4 endPos = screenAdaptionRelative * Ogre::Vector4(mProgressLeft + 1.0f, mProgressTop, mProgressLeft + 16.0f, mProgressBottom);
    Ogre::Vector4 infoPos = screenAdaptionRelative * Ogre::Vector4(320.0f, mProgressTop + 20.0f, 320.0f, mProgressTop + 20.0f);


    mBegin = createPanel("LoaderScreenProgressBegin", beginPos, mMaterialNameEnd);
    mBegin->setUV(1.0f, 0.0f, 0.0f, 1.0f);

    mLoaderScreen->addChild(mBegin);
    mBegin->show();


    mMiddle = createPanel("LoaderScreenProgressMiddle", middlePos, mMaterialNameMiddle);
    mMiddle->setUV(0.0f, 0.0f, 1.0f, 1.0f);

    mLoaderScreen->addChild(mMiddle);
    mMiddle->show();


    mEnd = createPanel("LoaderScreenProgressEnd", endPos, mMaterialNameEnd);
    mEnd->setUV(0.0f, 0.0f, 1.0f, 1.0f);

    mLoaderScreen->addChild(mEnd);
    mEnd->show();

    mInfo = createTextArea("LoaderScreenProgressInfo", infoPos.z - infoPos.x, infoPos.w - infoPos.y, infoPos.x, infoPos.y); 
    mInfo->setCaption("");
    mInfo->setCharHeight(16.0f * viewportHeight / 1024.0f);
    mInfo->setSpaceWidth(9.0f);
    mInfo->setAlignment(Ogre::TextAreaOverlayElement::Center);
    mInfo->setFontName("SdkTrays/Caption");
    mInfo->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
    mLoaderScreen->addChild(mInfo);
    mInfo->show();



    mModeContext.mWindow->update(true);// update draw
}

void UIBackgroundLoaderProgress::hide()
{
    mLoaderScreen->removeChild(mInfo->getName());
    mLoaderScreen->removeChild(mBegin->getName());
    mLoaderScreen->removeChild(mMiddle->getName());
    mLoaderScreen->removeChild(mEnd->getName());

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    om.destroyOverlayElement(mBegin);
    om.destroyOverlayElement(mMiddle);
    om.destroyOverlayElement(mEnd);
    om.destroyOverlayElement(mInfo);

    UIBackground::hide();
}

void UIBackgroundLoaderProgress::setPercent(float percent, const std::string& info)
{
    mPercent = percent;

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    Ogre::Matrix4 screenAdaptionRelative(
        viewportWidth / 640.0f, 0.0f,                       0.0f,                   0.0f,
        0.0f,                   viewportHeight / 480.0f,    0.0f,                   0.0f,
        0.0f,                   0.0f,                       viewportWidth / 640.0f, 0.0f,
        0.0f,                   0.0f,                       0.0f,                   viewportHeight / 480.0f);

    float screenWidth = (mProgressRight - mProgressLeft) * mPercent;

    Ogre::Vector4 resWidth = screenAdaptionRelative * Ogre::Vector4(screenWidth, 0.0f, mProgressLeft, 0.0f);

    mMiddle->setWidth(resWidth.x);
    mEnd->setLeft(resWidth.z + resWidth.x);

    mInfo->setCaption(info);

    mModeContext.mWindow->update(true);// update draw
}

UIBackgroundLoaderProgressTracks::UIBackgroundLoaderProgressTracks(const ModeContext& modeContext, 
                           const PFLoader& loader,
                           const std::string& path, const std::string& fileName,
                           float progressTop, float progressBottom, float progressLeft, float progressRight) :
    UIBackgroundLoaderProgress(modeContext, loader, path, fileName, progressTop, progressBottom, progressLeft, progressRight)
{
    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    for(size_t q = 0; q < availTracks.size(); ++q)
    {
        std::string loadingFilePath = "data/tracks/" + strPowerslide.getBaseDir(availTracks[q]) + "/misc/loading";

        Ogre::String textureName = nameGenTextures.generate();
        Ogre::String textureNameTitle = nameGenTextures.generate();

        Ogre::String materialName = nameGenMaterials.generate();
        mMaterialTrackNames.insert(std::make_pair(availTracks[q], materialName));

        Ogre::String materialNameTitle = nameGenMaterials.generate();
        mMaterialTrackTitlesNames.insert(std::make_pair(availTracks[q], materialNameTitle));

        TextureLoader().load(   loader, 
                                loadingFilePath, "loading.tga", 
                                textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        TextureLoader().load(   loader, 
                                loadingFilePath, "text-english.tga", 
                                textureNameTitle, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        {
            std::vector<Ogre::String> texName;
            texName.push_back(textureName);

            Ogre::MaterialPtr newMat = CloneMaterial(  materialName, 
                                mMaterialName, 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
            newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
            Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
            state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
        }

        {
            std::vector<Ogre::String> texName;
            texName.push_back(textureNameTitle);

            Ogre::MaterialPtr newMat = CloneMaterial(  materialNameTitle, 
                                mMaterialName, 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
            newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
            Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
            state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
        }
    }

    //AI strength
    {
        {
            Ogre::String textureName = nameGenTextures.generate();
            mMaterialAIEasyName = nameGenMaterials.generate();


            TextureLoader().load(   loader, 
                                "data/misc/loading", "easy-english.tga", 
                                textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

            std::vector<Ogre::String> texName;
            texName.push_back(textureName);

            Ogre::MaterialPtr newMat = CloneMaterial(  mMaterialAIEasyName, 
                                mMaterialName, 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
            newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
            Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
            state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
        }

        {
            Ogre::String textureName = nameGenTextures.generate();
            mMaterialAIMediumName = nameGenMaterials.generate();


            TextureLoader().load(   loader, 
                                "data/misc/loading", "medium-english.tga", 
                                textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

            std::vector<Ogre::String> texName;
            texName.push_back(textureName);

            Ogre::MaterialPtr newMat = CloneMaterial(  mMaterialAIMediumName, 
                                mMaterialName, 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
            newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
            Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
            state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
        }

        {
            Ogre::String textureName = nameGenTextures.generate();
            mMaterialAIHardName = nameGenMaterials.generate();


            TextureLoader().load(   loader, 
                                "data/misc/loading", "hard-english.tga", 
                                textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

            std::vector<Ogre::String> texName;
            texName.push_back(textureName);

            Ogre::MaterialPtr newMat = CloneMaterial(  mMaterialAIHardName, 
                                mMaterialName, 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
            newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
            Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
            state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
        }

        {
            Ogre::String textureName = nameGenTextures.generate();
            mMaterialAIInsaneName = nameGenMaterials.generate();


            TextureLoader().load(   loader, 
                                "data/misc/loading", "insane-english.tga", 
                                textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

            std::vector<Ogre::String> texName;
            texName.push_back(textureName);

            Ogre::MaterialPtr newMat = CloneMaterial(  mMaterialAIInsaneName, 
                                mMaterialName, 
                                texName, 
                                1.0f,
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
            newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
            Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
            state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
        }
    }//AI strength
}

void UIBackgroundLoaderProgressTracks::show(const std::string& trackName, bool showAIStrength, AIStrength strength)
{
    UIBackgroundLoaderProgress::show();
    
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    Ogre::Matrix4 screenAdaptionRelative(
        viewportWidth / 640.0f, 0.0f,                       0.0f,                   0.0f,
        0.0f,                   viewportHeight / 480.0f,    0.0f,                   0.0f,
        0.0f,                   0.0f,                       viewportWidth / 640.0f, 0.0f,
        0.0f,                   0.0f,                       0.0f,                   viewportHeight / 480.0f);

    Ogre::Vector4 trackPos = screenAdaptionRelative * Ogre::Vector4(0.0f, 140.0f, 640.0f, 328.0f);
    Ogre::Vector4 trackTitlePos = screenAdaptionRelative * Ogre::Vector4(133.0f, 341.0f, 133.0f + 363.0f, 341.0f + 40.0f);


    mTrack = createPanel("LoaderScreenProgressTrack", trackPos, mMaterialTrackNames[trackName]);
    mTrack->setUV(0.0f, 0.0f, 1.0f, 1.0f);

    mLoaderScreen->addChild(mTrack);
    mTrack->show();


    mTrackTitle = createPanel("LoaderScreenProgressTrackTitle", trackTitlePos, mMaterialTrackTitlesNames[trackName]);
    mTrackTitle->setUV(0.0f, 0.0f, 1.0f, 1.0f);

    mLoaderScreen->addChild(mTrackTitle);
    mTrackTitle->show();


    mUIStrength = NULL;
    if(showAIStrength)
    {
        Ogre::Vector4 uiStrengthPos = screenAdaptionRelative * Ogre::Vector4(0.0f, 0.0f, 280.0f, 119.0f);

        Ogre::String uiStrengthName = mMaterialAIEasyName;
        switch(strength)
        {
            case Medium:
                uiStrengthName = mMaterialAIMediumName;
                break;
            case Hard:
                uiStrengthName = mMaterialAIHardName;
                break;
            case Insane:
                uiStrengthName = mMaterialAIInsaneName;
                break;
        }
        mUIStrength = createPanel("LoaderScreenProgressUIStrength", uiStrengthPos, uiStrengthName);
        mUIStrength->setUV(0.0f, 0.0f, 1.0f, 1.0f);

        mLoaderScreen->addChild(mUIStrength);
        mUIStrength->show();
    }


    mModeContext.mWindow->update(true);// update draw
}

void UIBackgroundLoaderProgressTracks::hide()
{
    mLoaderScreen->removeChild(mTrack->getName());
    mLoaderScreen->removeChild(mTrackTitle->getName());
    if(mUIStrength)
        mLoaderScreen->removeChild(mUIStrength->getName());

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    om.destroyOverlayElement(mTrack);
    om.destroyOverlayElement(mTrackTitle);
    if(mUIStrength)
        om.destroyOverlayElement(mUIStrength);

    UIBackgroundLoaderProgress::hide();
}
