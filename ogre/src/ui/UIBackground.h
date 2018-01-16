#ifndef UIBACKGROUND_H
#define UIBACKGROUND_H

#include "../includes/OgreInclude.h"

#include "OgreTrays.h"

#include "../gamemodes/ModeContext.h"

class UIBackground
{
public:
    UIBackground(const ModeContext& modeContext, 
        const PFLoader& loader,
        const std::string& path, const std::string& fileName);

    void show();
    void hide();

#if defined(__ANDROID__)
    virtual void reloadTextures(const PFLoader& loader, const std::string& path, const std::string& fileName);
#endif

protected:

    Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top, const Ogre::String& material);
    Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, const Ogre::Vector4& pos, const Ogre::String& material);
    Ogre::TextAreaOverlayElement* createTextArea(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top);


    ModeContext mModeContext;

    Ogre::PanelOverlayElement* mLoaderScreen;

    Ogre::String mMaterialName;

    static Ogre::NameGenerator nameGenMaterials;
    static Ogre::NameGenerator nameGenTextures;

private:

    
    void createCamera();
    void destroyCamera();

#if defined(__ANDROID__)
    std::string mTextureName;
#endif

    Ogre::MaterialPtr mMaterial;

    Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;
};

class UIBackgroundLoaderProgress : public UIBackground
{
public:

    UIBackgroundLoaderProgress(const ModeContext& modeContext, 
        const PFLoader& loader,
        const std::string& path, const std::string& fileName,
        float progressTop, float progressBottom, float progressLeft, float progressRight);

    void show();
    void hide();

    void setPercent(float percent, const std::string& info);

#if defined(__ANDROID__)
    virtual void reloadTextures(const PFLoader& loader, const std::string& path, const std::string& fileName);
#endif

private:

    float mProgressTop;
    float mProgressBottom;
    float mProgressLeft;
    float mProgressRight;

    Ogre::PanelOverlayElement* mBegin;
    Ogre::PanelOverlayElement* mMiddle;
    Ogre::PanelOverlayElement* mEnd;

    Ogre::TextAreaOverlayElement * mInfo;

#if defined(__ANDROID__)
    Ogre::String mTextureNameEnd;
    Ogre::String mTextureNameMiddle;
#endif

    Ogre::String mMaterialNameEnd;
    Ogre::String mMaterialNameMiddle;


    float mPercent;
};

class UIBackgroundLoaderProgressTracks : public UIBackgroundLoaderProgress
{
public:

    UIBackgroundLoaderProgressTracks(const ModeContext& modeContext, 
        const PFLoader& loader,
        const std::string& path, const std::string& fileName,
        float progressTop, float progressBottom, float progressLeft, float progressRight);

    void show(const std::string& trackName, bool showAIStrength, AIStrength strength);
    void hide();

#if defined(__ANDROID__)
    virtual void reloadTextures(const PFLoader& loader, const std::string& path, const std::string& fileName);
#endif

private:

#if defined(__ANDROID__)
    std::vector<Ogre::String> mTextureTrackNames;
    std::vector<Ogre::String> mTextureTrackTitlesNames;

    Ogre::String mTextureAIEasyName;
    Ogre::String mTextureAIMediumName;
    Ogre::String mTextureAIHardName;
    Ogre::String mTextureAIInsaneName;
#endif

    std::map<std::string, Ogre::String> mMaterialTrackNames;
    std::map<std::string, Ogre::String> mMaterialTrackTitlesNames;

    Ogre::String mMaterialAIEasyName;
    Ogre::String mMaterialAIMediumName;
    Ogre::String mMaterialAIHardName;
    Ogre::String mMaterialAIInsaneName;

    Ogre::PanelOverlayElement* mTrack;
    Ogre::PanelOverlayElement* mTrackTitle;
    Ogre::PanelOverlayElement* mUIStrength;
};

#endif