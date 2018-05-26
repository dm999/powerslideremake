#ifndef UITABLE_H
#define UITABLE_H

#include <string>
#include <vector>

#include "../UIBase.h"

class UITable;

class UITableOnAction
{
public:

    virtual void onTablePressed(UITable * table, size_t row) = 0;
    virtual void onTableReleased(UITable * table, size_t row) = 0;
};

class UITable : public UIBase
{
public:

    UITable();
    virtual ~UITable(){}

    void loadBackground();

    void init(const Ogre::Matrix4& screenAdaptionRelative, 
        Ogre::OverlayContainer* mainBackground, 
        const Ogre::Vector4& dimensions,// original sizes (640x480), before adaptation
        Ogre::Real fontSize);

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

    void show();
    void hide();

    bool isVisible() const {return mIsShown;}

    void setText(const Ogre::String& text, size_t row);

    void setTableOnAction(UITableOnAction* onAction) {mOnAction = onAction;}

private:

    bool mIsShown;
    bool mIsPressed;

    std::vector<Ogre::String> mMaterialNames;

    std::vector<Ogre::String> mPanelNames;
    std::vector<Ogre::String> mTextNames;

    std::vector<Ogre::PanelOverlayElement*> mBackgrounds;
    std::vector<Ogre::TextAreaOverlayElement *> mTexts;

    UITableOnAction * mOnAction;


    static Ogre::NameGenerator nameGenMaterials;
    static Ogre::NameGenerator nameGenPanel;
    static Ogre::NameGenerator nameGenText;
};

#endif