#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "../UIBase.h"


class PFLoader;

class UIButton;

class UIButtonOnAction
{
public:

    virtual void onButtonPressed(UIButton * button) = 0;
    virtual void onButtonReleased(UIButton * button) = 0;
};

class UIButton : public UIBase
{
public:

    UIButton();
    virtual ~UIButton(){}

    void loadBackground(const PFLoader& pfLoaderGameshell, const std::string& textureName);
    void setBackgroundMaterial(const std::string& name);

    void init(const Ogre::Matrix4& screenAdaptionRelative, 
        Ogre::OverlayContainer* mainBackground, 
        const Ogre::Vector4& dimensions); // original sizes (640x480), before adaptation

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

    void show();
    void hide();

    bool isVisible() const {return mIsShown;}

    Ogre::String getMaterialName()const {return mMaterialName;}

    void setButtonOnAction(UIButtonOnAction* onAction) {mOnAction = onAction;}

protected:

    bool mIsShown;
    bool mIsPressed;

    Ogre::String mMaterialName;

    Ogre::String mPanelName;

    Ogre::PanelOverlayElement* mBackground;

    UIButtonOnAction * mOnAction;

    static Ogre::NameGenerator nameGenMaterials;
    static Ogre::NameGenerator nameGenPanel;
};

class UIButtonTick : public UIButton
{
public:

    void init(const Ogre::Matrix4& screenAdaptionRelative, 
        Ogre::OverlayContainer* mainBackground, 
        const Ogre::Vector4& dimensions,// original sizes (640x480), before adaptation
        bool isChecked,
        bool isActive); 

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos) {}

    bool getChecked() const {return mIsChecked;}

private:

    bool mIsChecked;
    bool mIsActive;
};

#endif