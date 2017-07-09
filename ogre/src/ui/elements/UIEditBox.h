#ifndef UIEDITBOX_H
#define UIEDITBOX_H

#include "../UIBase.h"

#include "../../includes/MyGUI_KeyCode.h"

class PFLoader;

class UIEditBox : public UIBase
{
public:

    UIEditBox();
    virtual ~UIEditBox(){}

    void init(const PFLoader& pfLoaderGameshell, 
        const Ogre::Matrix4& screenAdaptionRelative, 
        Ogre::PanelOverlayElement* mainBackground, 
        float left, float top, bool isActive = false);

    void frameStarted(const Ogre::FrameEvent &evt);
    void keyUp(MyGUI::KeyCode _key, wchar_t _char);
    void mouseReleased(const Ogre::Vector2& pos);

    void show();
    void hide();

private:

    Ogre::DisplayString mCaption;
    bool mIsShown;
    bool mIsCaretShown;
    bool mIsActive;

    Ogre::String mTextureName;
    Ogre::String mMaterialName;

    Ogre::String mPanelName;
    Ogre::String mTextName;

    Ogre::PanelOverlayElement* mBackground;
    Ogre::TextAreaOverlayElement * mText;

    Ogre::Real mCaretSize;

    Ogre::Timer mBlinkTimer;

    static Ogre::NameGenerator nameGenTextures;
    static Ogre::NameGenerator nameGenMaterials;
    static Ogre::NameGenerator nameGenPanel;
    static Ogre::NameGenerator nameGenText;
};

#endif