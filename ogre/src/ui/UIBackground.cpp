#include "../pcheader.h"

#include "UIBackground.h"

#include "../GameState.h"

#include "../loaders/TextureLoader.h"

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
    mTextureName = nameGenTextures.generate();

    TextureLoader().load(   loader, 
                            path, fileName, 
                            mTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

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

    mMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(mTextureName);

    Ogre::TextureUnitState *state = mMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    state->setTextureFiltering(Ogre::FO_NONE, Ogre::FO_NONE, Ogre::FO_NONE);

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