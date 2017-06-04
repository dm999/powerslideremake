#ifndef UIBACKGROUND_H
#define UIBACKGROUND_H

#include "../includes/OgreInclude.h"

#include "SdkTrays.h"

#include "../gamemodes/ModeContext.h"

class UIBackground
{
public:
    UIBackground(const ModeContext& modeContext, 
        const PFLoader& loader,
        const std::string& path, const std::string& fileName);

    void show();
    void hide();

protected:

    Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top, const Ogre::String& material);
    Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, const Ogre::Vector4& pos, const Ogre::String& material);


    ModeContext mModeContext;

    Ogre::PanelOverlayElement* mLoaderScreen;

    Ogre::String mMaterialName;

    static Ogre::NameGenerator nameGenMaterials;
    static Ogre::NameGenerator nameGenTextures;

private:

    
    void createCamera();
    void destroyCamera();

    Ogre::MaterialPtr mMaterial;

    Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;

    Ogre::String mTextureName;
};

class UIBackgroundLoader : public UIBackground
{
public:

    UIBackgroundLoader(const ModeContext& modeContext, 
        const PFLoader& loader,
        const std::string& path, const std::string& fileName,
        float progressTop, float progressBottom, float progressLeft, float progressRight);

    void show();
    void hide();

    void setPercent(float percent);

private:

    float mProgressTop;
    float mProgressBottom;
    float mProgressLeft;
    float mProgressRight;

    Ogre::PanelOverlayElement* mBegin;
    Ogre::PanelOverlayElement* mMiddle;
    Ogre::PanelOverlayElement* mEnd;

    Ogre::String mTextureNameEnd;
    Ogre::String mTextureNameMiddle;

    Ogre::String mMaterialNameEnd;
    Ogre::String mMaterialNameMiddle;

    float mPercent;
};

#endif