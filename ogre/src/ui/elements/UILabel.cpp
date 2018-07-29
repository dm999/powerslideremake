#include "UILabel.h"

#include "../UIMainMenuLabels.h"

Ogre::NameGenerator UILabel::nameGenTextArea("UILabel/TextArea");

const Ogre::ColourValue UILabel::mDisabledLabel(0.25f, 0.25f, 0.25f);
const Ogre::ColourValue UILabel::mInactiveLabel(0.50f, 0.50f, 0.50f);

UILabel::UILabel() : 
    mOnAction(NULL), mIsShown(true), mIsPressed(false), mIsOver(false), mIsActive(true), mIsFixed(false)
{
    mLabelName = nameGenTextArea.generate();
}

void UILabel::init(Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top)
{
    mTextArea = createTextArea(mLabelName, width, height, left, top); 

    mTextArea->setFontName("SdkTrays/Caption");
}

void UILabel::mousePressed(const Ogre::Vector2& pos)
{
    if(mTextArea && mIsShown && mIsActive && !mIsFixed)
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
    if(mTextArea && mIsShown && mIsPressed && mIsActive && !mIsFixed)
    {
        mIsPressed = false;

        mTextArea->setColour(mInactiveLabel);

        if(mOnAction)
        {
            mOnAction->onLabelReleased(this);
        }
    }
}

void UILabel::mouseMoved(const Ogre::Vector2& pos)
{
    if(mTextArea && mIsShown && !mIsFixed)
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
                mTextArea->setColour(mInactiveLabel);
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
            mTextArea->setColour(mInactiveLabel);
        }
        else
        {
            mTextArea->setColour(mDisabledLabel);
        }
    }
}

void UILabel::setFixed(bool isFixed)
{
    mIsFixed = isFixed;

    if(mTextArea)
    {
        if(isFixed)
        {
            mTextArea->setColour(Ogre::ColourValue::White);
        }
        else
        {
            mTextArea->setColour(mInactiveLabel);
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