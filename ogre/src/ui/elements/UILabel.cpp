#include "UILabel.h"

#include "../UIMainMenuLabels.h"

Ogre::NameGenerator UILabel::nameGenTextArea("UILabel/TextArea");

UILabel::UILabel() : 
    mOnAction(NULL), mIsShown(true), mIsPressed(false), mIsOver(false)
{
    mLabelName = nameGenTextArea.generate();
}

void UILabel::init(Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top)
{
    mTextArea = createTextArea(mLabelName, width, height, left, top); 
}

void UILabel::mousePressed(const Ogre::Vector2& pos)
{
    if(mTextArea && mIsShown)
    {
        if(OgreBites::Widget::isCursorOver(mTextArea, pos, 0))
        {
            mIsPressed = true;

            if(mOnAction)
            {
                mOnAction->onLabelPressed(this);
            }
        }
    }
}

void UILabel::mouseReleased(const Ogre::Vector2& pos)
{
    if(mTextArea && mIsShown && mIsPressed)
    {
        mIsPressed = false;

        if(mOnAction)
        {
            mOnAction->onLabelReleased(this);
        }
    }
}

void UILabel::mouseMoved(const Ogre::Vector2& pos)
{
    if(mTextArea && mIsShown)
    {
        if(OgreBites::Widget::isCursorOver(mTextArea, pos, 0))
        {
            mTextArea->setColour(Ogre::ColourValue::White);

            if(mOnAction && !mIsOver)
            {
                mOnAction->onLabelOver(this);
            }

            mIsOver = true;
        }
        else
        {
            mTextArea->setColour(UIMainMenuLabels::mInactiveLabel);
            mIsOver = false;
        }
    }
    
}

void UILabel::show()
{
    if(mTextArea)
        mTextArea->show();

    mIsShown = true;
}

void UILabel::hide()
{
    if(mTextArea)
        mTextArea->hide();

    mIsShown = false;
}