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

    Ogre::TextAreaOverlayElement * getTextArea()const {return mTextArea;}

    void setLabelOnAction(UILabelOnAction* onAction) {mOnAction = onAction;}

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

    void show();
    void hide();

    bool isVisible() const {return mIsShown;}

private:

    static Ogre::NameGenerator nameGenTextArea;

    Ogre::String mLabelName;

    Ogre::TextAreaOverlayElement * mTextArea;

    UILabelOnAction * mOnAction;

    bool mIsShown;
    bool mIsPressed;
    bool mIsOver;
};

#endif
