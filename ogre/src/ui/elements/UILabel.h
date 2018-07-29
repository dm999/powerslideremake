#ifndef UILABEL_H
#define UILABEL_H

#include "../UIBase.h"

class UILabel;

class UILabelOnAction
{
public:

    virtual void onLabelPressed(UILabel * label) = 0;
    virtual void onLabelReleased(UILabel * label) = 0;
    virtual void onLabelOver(UILabel * label) = 0;
};

class UILabel : public UIBase
{
public:

    UILabel();
    virtual ~UILabel(){}

    void init(Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top);

    void setActive(bool isActive);
    void setFixed(bool isFixed);

    Ogre::TextAreaOverlayElement * getTextArea()const {return mTextArea;}

    void setLabelOnAction(UILabelOnAction* onAction) {mOnAction = onAction;}

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

    void show();
    void hide();

    bool isVisible() const {return mIsShown;}

    static const Ogre::ColourValue mDisabledLabel;
    static const Ogre::ColourValue mInactiveLabel;

private:

    static Ogre::NameGenerator nameGenTextArea;

    Ogre::String mLabelName;

    Ogre::TextAreaOverlayElement * mTextArea;

    UILabelOnAction * mOnAction;

    bool mIsShown;
    bool mIsPressed;
    bool mIsOver;
    bool mIsActive;//switch states if true, otherwise grey
    bool mIsFixed;//always white if true
};

#endif
