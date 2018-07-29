#include "UILabel.h"

#include "../UIMainMenuLabels.h"

Ogre::NameGenerator UILabel::nameGenTextArea("UILabel/TextArea");

UILabel::UILabel() : 
    mOnAction(NULL), mIsShown(true), mIsPressed(false), mIsOver(false), mIsActive(true)
{
    mLabelName = nameGenTextArea.generate();
}

void UILabel::init(Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top)
{
    mTextArea = createTextArea(mLabelName, width, height, left, top); 
}

void UILabel::mousePressed(const Ogre::Vector2& pos)
{
    if(mTextArea && mIsShown && mIsActive)
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
    if(mTextArea && mIsShown && mIsPressed && mIsActive)
    {
        mIsPressed = false;

        mTextArea->setColour(UIMainMenuLabels::mInactiveLabel);

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
        if(mIsActive)
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
    
}

void UILabel::setActive(bool isActive)
{
    mIsActive = isActive;

    if(mTextArea)
    {
        if(mIsActive)
        {
            mTextArea->setColour(UIMainMenuLabels::mInactiveLabel);
        }
        else
        {
            mTextArea->setColour(UIMainMenuLabels::mDisabledLabel);
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