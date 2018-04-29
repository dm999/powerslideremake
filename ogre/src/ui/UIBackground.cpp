
#include "UIBackground.h"

#include "../GameState.h"

#include "../loaders/TextureLoader.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

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
#if defined(__ANDROID__)
    mTextureName = textureName;
#endif

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

#if defined(__ANDROID__)
void UIBackground::reloadTextures(const PFLoader& loader, const std::string& path, const std::string& fileName)
{
    TextureLoader().load(   loader, 
                            path, fileName, 
                            mTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}
#endif

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

#if defined(__ANDROID__)
    mTextureNameEnd = textureNameEnd;
    mTextureNameMiddle = textureNameMiddle;
#endif

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

#if defined(__ANDROID__)
void UIBackgroundLoaderProgress::reloadTextures(const PFLoader& loader, const std::string& path, const std::string& fileName)
{
    UIBackground::reloadTextures(loader, path, fileName);

    TextureLoader().load(   loader, 
                            "data/misc/loading", "end.tga", 
                            mTextureNameEnd, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    TextureLoader().load(   loader, 
                            "data/misc/loading", "middle.tga", 
                            mTextureNameMiddle, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}
#endif

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
#if defined(__ANDROID__)
        mTextureTrackNames.push_back(textureName);
        mTextureTrackTitlesNames.push_back(textureNameTitle);
#endif

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
#if defined(__ANDROID__)
            mTextureAIEasyName = textureName;
#endif

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
#if defined(__ANDROID__)
            mTextureAIMediumName = textureName;
#endif

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
#if defined(__ANDROID__)
            mTextureAIHardName = textureName;
#endif

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
#if defined(__ANDROID__)
            mTextureAIInsaneName = textureName;
#endif

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

#if defined(__ANDROID__)
void UIBackgroundLoaderProgressTracks::reloadTextures(const PFLoader& loader, const std::string& path, const std::string& fileName)
{
    UIBackgroundLoaderProgress::reloadTextures(loader, path, fileName);

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    for(size_t q = 0; q < availTracks.size(); ++q)
    {
        std::string loadingFilePath = "data/tracks/" + strPowerslide.getBaseDir(availTracks[q]) + "/misc/loading";

        TextureLoader().load(   loader, 
                                loadingFilePath, "loading.tga", 
                                mTextureTrackNames[q], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        TextureLoader().load(   loader, 
                                loadingFilePath, "text-english.tga", 
                                mTextureTrackTitlesNames[q], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }

    TextureLoader().load(   loader, 
                        "data/misc/loading", "easy-english.tga", 
                        mTextureAIEasyName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    TextureLoader().load(   loader, 
                        "data/misc/loading", "medium-english.tga", 
                        mTextureAIMediumName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    TextureLoader().load(   loader, 
                        "data/misc/loading", "hard-english.tga", 
                        mTextureAIHardName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    TextureLoader().load(   loader, 
                        "data/misc/loading", "insane-english.tga", 
                        mTextureAIInsaneName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}
#endif


UIBackgroundLoaderProgressTracksChampionship::UIBackgroundLoaderProgressTracksChampionship(const ModeContext& modeContext, 
                           const PFLoader& loader,
                           const std::string& path, const std::string& fileName,
                           float progressTop, float progressBottom, float progressLeft, float progressRight) :
    UIBackgroundLoaderProgress(modeContext, loader, path, fileName, progressTop, progressBottom, progressLeft, progressRight)
{

    loadThirds(loader);
    loadFifths(loader);
    loadEighths(loader);

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    for(size_t q = 0; q < availTracks.size(); ++q)
    {
        std::string loadingFilePath = "data/tracks/" + strPowerslide.getBaseDir(availTracks[q]) + "/misc/loading";

        Ogre::String textureNameTitle = nameGenTextures.generate();
#if defined(__ANDROID__)
        mTextureTrackTitlesNames.push_back(textureNameTitle);
#endif

        Ogre::String materialNameTitle = nameGenMaterials.generate();
        mMaterialTrackTitlesNames.insert(std::make_pair(availTracks[q], materialNameTitle));

        TextureLoader().load(   loader, 
                                loadingFilePath, "text-english.tga", 
                                textureNameTitle, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

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
#if defined(__ANDROID__)
            mTextureAIEasyName = textureName;
#endif

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
#if defined(__ANDROID__)
            mTextureAIMediumName = textureName;
#endif

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
#if defined(__ANDROID__)
            mTextureAIHardName = textureName;
#endif

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
#if defined(__ANDROID__)
            mTextureAIInsaneName = textureName;
#endif

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

void UIBackgroundLoaderProgressTracksChampionship::loadThirds(const PFLoader& loader)
{

    std::string loadingFilePathThirds = "data/misc/loading/thirds";

    loadTextureAndCreateMaterial(loader, loadingFilePathThirds, "desert-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesThirdsDark,
#endif
    mMaterialTrackNamesThirdsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathThirds, "speedway-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesThirdsDark,
#endif
    mMaterialTrackNamesThirdsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathThirds, "dam-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesThirdsDark,
#endif
    mMaterialTrackNamesThirdsDark);


    loadTextureAndCreateMaterial(loader, loadingFilePathThirds, "desert-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesThirdsLight,
#endif
    mMaterialTrackNamesThirdsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathThirds, "speedway-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesThirdsLight,
#endif
    mMaterialTrackNamesThirdsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathThirds, "dam-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesThirdsLight,
#endif
    mMaterialTrackNamesThirdsLight);
}

void UIBackgroundLoaderProgressTracksChampionship::loadFifths(const PFLoader& loader)
{
    std::string loadingFilePathFifths = "data/misc/loading/fifths";

    loadTextureAndCreateMaterial(loader, loadingFilePathFifths, "desert-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesFifthsDark,
#endif
    mMaterialTrackNamesFifthsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathFifths, "speedway-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesFifthsDark,
#endif
    mMaterialTrackNamesFifthsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathFifths, "dam-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesFifthsDark,
#endif
    mMaterialTrackNamesFifthsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathFifths, "mountain forest-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesFifthsDark,
#endif
    mMaterialTrackNamesFifthsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathFifths, "mineshaft-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesFifthsDark,
#endif
    mMaterialTrackNamesFifthsDark);


    loadTextureAndCreateMaterial(loader, loadingFilePathFifths, "desert-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesFifthsLight,
#endif
    mMaterialTrackNamesFifthsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathFifths, "speedway-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesFifthsLight,
#endif
    mMaterialTrackNamesFifthsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathFifths, "dam-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesFifthsLight,
#endif
    mMaterialTrackNamesFifthsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathFifths, "mountain forest-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesFifthsLight,
#endif
    mMaterialTrackNamesFifthsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathFifths, "mineshaft-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesFifthsLight,
#endif
    mMaterialTrackNamesFifthsLight);
}

void UIBackgroundLoaderProgressTracksChampionship::loadEighths(const PFLoader& loader)
{
    std::string loadingFilePathEighths = "data/misc/loading/eighths";


    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "desert-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsDark,
#endif
    mMaterialTrackNamesEighthsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "speedway-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsDark,
#endif
    mMaterialTrackNamesEighthsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "dam-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsDark,
#endif
    mMaterialTrackNamesEighthsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "mountain forest-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsDark,
#endif
    mMaterialTrackNamesEighthsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "mineshaft-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsDark,
#endif
    mMaterialTrackNamesEighthsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "alpine-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsDark,
#endif
    mMaterialTrackNamesEighthsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "city-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsDark,
#endif
    mMaterialTrackNamesEighthsDark);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "nutopia-dark.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsDark,
#endif
    mMaterialTrackNamesEighthsDark);


    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "desert-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsLight,
#endif
    mMaterialTrackNamesEighthsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "speedway-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsLight,
#endif
    mMaterialTrackNamesEighthsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "dam-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsLight,
#endif
    mMaterialTrackNamesEighthsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "mountain forest-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsLight,
#endif
    mMaterialTrackNamesEighthsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "mineshaft-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsLight,
#endif
    mMaterialTrackNamesEighthsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "alpine-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsLight,
#endif
    mMaterialTrackNamesEighthsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "city-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsLight,
#endif
    mMaterialTrackNamesEighthsLight);

    loadTextureAndCreateMaterial(loader, loadingFilePathEighths, "nutopia-light.tga",
#if defined(__ANDROID__)
        mTextureTrackNamesEighthsLight,
#endif
    mMaterialTrackNamesEighthsLight);
}

void UIBackgroundLoaderProgressTracksChampionship::loadTextureAndCreateMaterial(
    const PFLoader& loader,
    const std::string& loadingFilePath, const std::string& fileName,
#if defined(__ANDROID__)
    std::vector<Ogre::String>& textureList,
#endif
    std::vector<Ogre::String>& materialList)
{

    Ogre::String textureName = nameGenTextures.generate();
#if defined(__ANDROID__)
        textureList.push_back(textureName);
#endif

    Ogre::String materialName = nameGenMaterials.generate();
    materialList.push_back(materialName);

    TextureLoader().load(   loader, 
                            loadingFilePath, fileName, 
                            textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

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

}

void UIBackgroundLoaderProgressTracksChampionship::show(size_t trackIndex, const std::string& trackName, bool showAIStrength, AIStrength strength)
{
    UIBackgroundLoaderProgress::show();

    mStrength = strength;
    
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


    if(mStrength == Easy)
    {
        const size_t div = 3;
        Ogre::Vector4 trackPosAdj = trackPos;

        for(size_t q = 0; q < div; ++q)
        {
            trackPosAdj.x = q * trackPos.z / div;
            trackPosAdj.z = trackPosAdj.x + trackPos.z / div;

            if(q == trackIndex)
                mTrack[q] = createPanel("LoaderScreenProgressTrack" + Conversions::DMToString(q), trackPosAdj, mMaterialTrackNamesThirdsLight[q]);
            else
                mTrack[q] = createPanel("LoaderScreenProgressTrack" + Conversions::DMToString(q), trackPosAdj, mMaterialTrackNamesThirdsDark[q]);

            mTrack[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);

            mLoaderScreen->addChild(mTrack[q]);
            mTrack[q]->show();
        }
    }

    if(mStrength == Medium)
    {
        const size_t div = 5;
        Ogre::Vector4 trackPosAdj = trackPos;

        for(size_t q = 0; q < div; ++q)
        {
            trackPosAdj.x = q * trackPos.z / div;
            trackPosAdj.z = trackPosAdj.x + trackPos.z / div;

            if(q == trackIndex)
                mTrack[q] = createPanel("LoaderScreenProgressTrack" + Conversions::DMToString(q), trackPosAdj, mMaterialTrackNamesFifthsLight[q]);
            else
                mTrack[q] = createPanel("LoaderScreenProgressTrack" + Conversions::DMToString(q), trackPosAdj, mMaterialTrackNamesFifthsDark[q]);

            mTrack[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);

            mLoaderScreen->addChild(mTrack[q]);
            mTrack[q]->show();
        }
    }

    if(mStrength == Hard || mStrength == Insane)
    {
        const size_t div = 8;
        Ogre::Vector4 trackPosAdj = trackPos;

        for(size_t q = 0; q < div; ++q)
        {
            trackPosAdj.x = q * trackPos.z / div;
            trackPosAdj.z = trackPosAdj.x + trackPos.z / div;

            if(q == trackIndex)
                mTrack[q] = createPanel("LoaderScreenProgressTrack" + Conversions::DMToString(q), trackPosAdj, mMaterialTrackNamesEighthsLight[q]);
            else
                mTrack[q] = createPanel("LoaderScreenProgressTrack" + Conversions::DMToString(q), trackPosAdj, mMaterialTrackNamesEighthsDark[q]);

            mTrack[q]->setUV(0.0f, 0.0f, 1.0f, 1.0f);

            mLoaderScreen->addChild(mTrack[q]);
            mTrack[q]->show();
        }
    }


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


    mModeContext.getRenderWindow()->update(true);// update draw
}

void UIBackgroundLoaderProgressTracksChampionship::hide()
{
    if(mStrength == Easy)
    {
        for(size_t q = 0; q < 3; ++q)
        {
            mLoaderScreen->removeChild(mTrack[q]->getName());
        }
    }

    if(mStrength == Medium)
    {
        for(size_t q = 0; q < 5; ++q)
        {
            mLoaderScreen->removeChild(mTrack[q]->getName());
        }
    }

    if(mStrength == Hard || mStrength == Insane)
    {
        for(size_t q = 0; q < 8; ++q)
        {
            mLoaderScreen->removeChild(mTrack[q]->getName());
        }
    }

    mLoaderScreen->removeChild(mTrackTitle->getName());
    if(mUIStrength)
        mLoaderScreen->removeChild(mUIStrength->getName());

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    if(mStrength == Easy)
    {
        for(size_t q = 0; q < 3; ++q)
        {
            om.destroyOverlayElement(mTrack[q]);
        }
    }
    if(mStrength == Medium)
    {
        for(size_t q = 0; q < 5; ++q)
        {
            om.destroyOverlayElement(mTrack[q]);
        }
    }
    if(mStrength == Hard || mStrength == Insane)
    {
        for(size_t q = 0; q < 8; ++q)
        {
            om.destroyOverlayElement(mTrack[q]);
        }
    }
    om.destroyOverlayElement(mTrackTitle);
    if(mUIStrength)
        om.destroyOverlayElement(mUIStrength);

    UIBackgroundLoaderProgress::hide();
}

#if defined(__ANDROID__)
void UIBackgroundLoaderProgressTracksChampionship::reloadTextures(const PFLoader& loader, const std::string& path, const std::string& fileName)
{
    UIBackgroundLoaderProgress::reloadTextures(loader, path, fileName);

    {
        std::string loadingFilePath = "data/misc/loading/thirds";

        TextureLoader().load(loader, loadingFilePath, "desert-dark.tga", mTextureTrackNamesThirdsDark[0], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "speedway-dark.tga", mTextureTrackNamesThirdsDark[1], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "dam-dark.tga", mTextureTrackNamesThirdsDark[2], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        TextureLoader().load(loader, loadingFilePath, "desert-light.tga", mTextureTrackNamesThirdsLight[0], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "speedway-light.tga", mTextureTrackNamesThirdsLight[1], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "dam-light.tga", mTextureTrackNamesThirdsLight[2], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    }

    {
        std::string loadingFilePath = "data/misc/loading/fifths";

        TextureLoader().load(loader, loadingFilePath, "desert-dark.tga", mTextureTrackNamesFifthsDark[0], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "speedway-dark.tga", mTextureTrackNamesFifthsDark[1], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "dam-dark.tga", mTextureTrackNamesFifthsDark[2], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "mountain forest-dark.tga", mTextureTrackNamesFifthsDark[3], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "mineshaft-dark.tga", mTextureTrackNamesFifthsDark[4], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        TextureLoader().load(loader, loadingFilePath, "desert-light.tga", mTextureTrackNamesFifthsLight[0], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "speedway-light.tga", mTextureTrackNamesFifthsLight[1], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "dam-light.tga", mTextureTrackNamesFifthsLight[2], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "mountain forest-light.tga", mTextureTrackNamesFifthsLight[3], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "mineshaft-light.tga", mTextureTrackNamesFifthsLight[4], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }

    {
        std::string loadingFilePath = "data/misc/loading/eighths";

        TextureLoader().load(loader, loadingFilePath, "desert-dark.tga", mTextureTrackNamesEighthsDark[0], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "speedway-dark.tga", mTextureTrackNamesEighthsDark[1], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "dam-dark.tga", mTextureTrackNamesEighthsDark[2], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "mountain forest-dark.tga", mTextureTrackNamesEighthsDark[3], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "mineshaft-dark.tga", mTextureTrackNamesEighthsDark[4], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "alpine-dark.tga", mTextureTrackNamesEighthsDark[5], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "city-dark.tga", mTextureTrackNamesEighthsDark[6], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "nutopia-dark.tga", mTextureTrackNamesEighthsDark[7], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        TextureLoader().load(loader, loadingFilePath, "desert-light.tga", mTextureTrackNamesEighthsLight[0], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "speedway-light.tga", mTextureTrackNamesEighthsLight[1], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "dam-light.tga", mTextureTrackNamesEighthsLight[2], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "mountain forest-light.tga", mTextureTrackNamesEighthsLight[3], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "mineshaft-light.tga", mTextureTrackNamesEighthsLight[4], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "alpine-light.tga", mTextureTrackNamesEighthsLight[5], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "city-light.tga", mTextureTrackNamesEighthsLight[6], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureLoader().load(loader, loadingFilePath, "nutopia-light.tga", mTextureTrackNamesEighthsLight[7], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

    for(size_t q = 0; q < availTracks.size(); ++q)
    {
        std::string loadingFilePath = "data/tracks/" + strPowerslide.getBaseDir(availTracks[q]) + "/misc/loading";

        TextureLoader().load(   loader, 
                                loadingFilePath, "text-english.tga", 
                                mTextureTrackTitlesNames[q], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }

    TextureLoader().load(   loader, 
                        "data/misc/loading", "easy-english.tga", 
                        mTextureAIEasyName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    TextureLoader().load(   loader, 
                        "data/misc/loading", "medium-english.tga", 
                        mTextureAIMediumName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    TextureLoader().load(   loader, 
                        "data/misc/loading", "hard-english.tga", 
                        mTextureAIHardName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    TextureLoader().load(   loader, 
                        "data/misc/loading", "insane-english.tga", 
                        mTextureAIInsaneName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}
#endif