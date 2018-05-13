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

    void show(const ModeContext& modeContext, bool showAIStrength);
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

    Ogre::TextAreaOverlayElement * mBestTime;
};

class UIBackgroundLoaderProgressTracksChampionship : public UIBackgroundLoaderProgress
{
public:

    UIBackgroundLoaderProgressTracksChampionship(const ModeContext& modeContext, 
        const PFLoader& loader,
        const std::string& path, const std::string& fileName,
        float progressTop, float progressBottom, float progressLeft, float progressRight);

    void show(size_t trackIndex, const ModeContext& modeContext, bool showAIStrength);
    void hide();

#if defined(__ANDROID__)
    virtual void reloadTextures(const PFLoader& loader, const std::string& path, const std::string& fileName);
#endif

private:

    void loadThirds(const PFLoader& loader);
    void loadFifths(const PFLoader& loader);
    void loadEighths(const PFLoader& loader);

    void loadTextureAndCreateMaterial(
    const PFLoader& loader,
    const std::string& loadingFilePath, const std::string& fileName,
#if defined(__ANDROID__)
    std::vector<Ogre::String>& textureList,
#endif
    std::vector<Ogre::String>& materialList
        );

#if defined(__ANDROID__)
    std::vector<Ogre::String> mTextureTrackNamesThirdsDark;
    std::vector<Ogre::String> mTextureTrackNamesThirdsLight;
    std::vector<Ogre::String> mTextureTrackNamesFifthsDark;
    std::vector<Ogre::String> mTextureTrackNamesFifthsLight;
    std::vector<Ogre::String> mTextureTrackNamesEighthsDark;
    std::vector<Ogre::String> mTextureTrackNamesEighthsLight;

    std::vector<Ogre::String> mTextureTrackTitlesNames;

    Ogre::String mTextureAIEasyName;
    Ogre::String mTextureAIMediumName;
    Ogre::String mTextureAIHardName;
    Ogre::String mTextureAIInsaneName;
#endif

    std::vector<Ogre::String> mMaterialTrackNamesThirdsDark;
    std::vector<Ogre::String> mMaterialTrackNamesThirdsLight;
    std::vector<Ogre::String> mMaterialTrackNamesFifthsDark;
    std::vector<Ogre::String> mMaterialTrackNamesFifthsLight;
    std::vector<Ogre::String> mMaterialTrackNamesEighthsDark;
    std::vector<Ogre::String> mMaterialTrackNamesEighthsLight;

    std::map<std::string, Ogre::String> mMaterialTrackTitlesNames;

    Ogre::String mMaterialAIEasyName;
    Ogre::String mMaterialAIMediumName;
    Ogre::String mMaterialAIHardName;
    Ogre::String mMaterialAIInsaneName;

    Ogre::TextAreaOverlayElement * mBestTime;

    static const int mMaxTracks = 8;
    Ogre::PanelOverlayElement* mTrack[mMaxTracks];
    Ogre::PanelOverlayElement* mTrackTitle;
    Ogre::PanelOverlayElement* mUIStrength;

    AIStrength mStrength;

};
#endif