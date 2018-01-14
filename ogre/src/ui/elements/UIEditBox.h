#ifndef UIEDITBOX_H
#define UIEDITBOX_H

#include "../UIBase.h"

#include "../../includes/MyGUI_KeyCode.h"

class PFLoader;

class UIEditBox : public UIBase
{
public:

    enum EditBoxCharType{
        Any,
        IP
    };

    UIEditBox(size_t maxSymbols = 256);
    virtual ~UIEditBox(){}

    void loadBackground(const PFLoader& pfLoaderGameshell, const std::string& filename);
    void setBackgroundMaterial(const std::string& name);

    void init(const Ogre::Matrix4& screenAdaptionRelative, 
        Ogre::PanelOverlayElement* mainBackground, 
        const Ogre::Vector4& dimensions, // original sizes (640x480), before adaptation
        Ogre::Real fontSize, // original sizes (640x480), before adaptation
        bool isActive = false);

    void frameStarted(const Ogre::FrameEvent &evt);
    void keyUp(MyGUI::KeyCode _key, wchar_t _char);
    void mouseReleased(const Ogre::Vector2& pos);

    void setText(const Ogre::DisplayString& str);
    Ogre::DisplayString getText() const { return mCaption;}

    void setColor(const Ogre::ColourValue& color);

    void setCharType(EditBoxCharType type) {mCharType = type;}

    void show();
    void hide();

    Ogre::String getMaterialName()const {return mMaterialName;}

    bool isActive() const{return mIsActive;}

private:

    void adjustCaptionLength();

    bool isLegalCharacter(MyGUI::KeyCode _key) const;

    Ogre::DisplayString mCaption;
    Ogre::DisplayString mCaptionToDisplay;

    bool mIsShown;
    bool mIsCaretShown;
    bool mIsActive;

    Ogre::String mTextureName;
    Ogre::String mMaterialName;

    Ogre::String mPanelName;
    Ogre::String mTextName;

    size_t mMaxSymbols;

    Ogre::PanelOverlayElement* mBackground;
    Ogre::TextAreaOverlayElement * mText;

    Ogre::Real mCaretSize;

    Ogre::Timer mBlinkTimer;

    EditBoxCharType mCharType;

    static Ogre::NameGenerator nameGenTextures;
    static Ogre::NameGenerator nameGenMaterials;
    static Ogre::NameGenerator nameGenPanel;
    static Ogre::NameGenerator nameGenText;
};

#endif