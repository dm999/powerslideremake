#include "UITable.h"

#include "../../loaders/TextureLoader.h"

#include "../../tools/OgreTools.h"

#include "../../GameState.h"

Ogre::NameGenerator UITable::nameGenMaterials("UITable/Material");
Ogre::NameGenerator UITable::nameGenPanel("UITable/Panel");
Ogre::NameGenerator UITable::nameGenText("UITable/Text");

UITable::UITable() : mIsShown(false), mIsPressed(false), mOnAction(NULL) {}

void UITable::loadBackground(bool isRight)
{
    mMaterialNames.clear();

    std::string textureNamePrefix = "OriginalLTable";

    if(isRight)
        textureNamePrefix = "OriginalRTable";

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        mMaterialNames.push_back(nameGenMaterials.generate());

        std::vector<Ogre::String> texName;
        texName.push_back(textureNamePrefix + Conversions::DMToString(q));
        Ogre::MaterialPtr newMat = CloneMaterial(  mMaterialNames[q], 
                            "Test/Diffuse", 
                            texName, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
    }
}

void UITable::init(const Ogre::Matrix4& screenAdaptionRelative, 
                    Ogre::OverlayContainer* mainBackground, 
                    const Ogre::Vector4& dimensions,
                    Ogre::Real fontSize)
{
    mPanelNames.clear();
    mBackgrounds.clear();

    mTextNames.clear();
    mTexts.clear();

    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f; 

    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        {
            Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(dimensions.x, dimensions.y + (dimensions.w * q), dimensions.x + dimensions.z, dimensions.y + (dimensions.w * (q + 1)));

            mPanelNames.push_back(nameGenPanel.generate());

            mBackgrounds.push_back(createPanel(mPanelNames[q], background, mMaterialNames[q]));
            mBackgrounds[q]->setUV(0.0f, 0.0f, 1.0f, 22.0f / 44.0f);
            mainBackground->addChild(mBackgrounds[q]);
        }

        {
            mTextNames.push_back(nameGenText.generate());

            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(2.0f, 2.0f, 0.0f, 0.0f);
            mTexts.push_back(createTextArea(mTextNames[q], 0.0f, 0.0f, textBoxPos.x, textBoxPos.y)); 
            mTexts[q]->setCaption("");
            mTexts[q]->setCharHeight(fontSize * viewportHeight / 1024.0f);
            mTexts[q]->setSpaceWidth(9.0f);
            mTexts[q]->setAlignment(Ogre::TextAreaOverlayElement::Left);
            mTexts[q]->setFontName("SdkTrays/Caption");
            mTexts[q]->setColour(Ogre::ColourValue::White);
            mBackgrounds[q]->addChild(mTexts[q]);
        }
    }
}

void UITable::mousePressed(const Ogre::Vector2& pos)
{
    if(mIsShown)
    {
        for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
        {
            if(mBackgrounds[q] && OgreBites::Widget::isCursorOver(mBackgrounds[q], pos, 0))
            {
                mIsPressed = true;

                if(mOnAction)
                {
                    mOnAction->onTablePressed(this, q);
                }

                break;
            }
        }
    }
}

void UITable::mouseReleased(const Ogre::Vector2& pos)
{
    if(mIsShown && mIsPressed)
    {
        for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
        {
            if(mBackgrounds[q])
            {
                if(OgreBites::Widget::isCursorOver(mBackgrounds[q], pos, 0))
                {
                    if(mTexts[q]->getCaption() != "")
                    {
                        mBackgrounds[q]->setUV(0.0f, 22.0f / 44.0f, 1.0f, 44.0f / 44.0f);

                        if(mOnAction)
                        {
                            mOnAction->onTableReleased(this, q);
                        }
                    }
                }
                else
                {
                    mBackgrounds[q]->setUV(0.0f, 0.0f, 1.0f, 22.0f / 44.0f);
                }

                mIsPressed = false;
            }
        }
    }
}

void UITable::mouseMoved(const Ogre::Vector2& pos)
{
    if(mIsShown)
    {
        for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
        {
            if(mBackgrounds[q])
            {
                if(OgreBites::Widget::isCursorOver(mBackgrounds[q], pos, 0))
                {
                    if(mTexts[q]->getCaption() != "")
                        mBackgrounds[q]->setUV(0.0f, 22.0f / 44.0f, 1.0f, 44.0f / 44.0f);
                }
                else
                {
                    mBackgrounds[q]->setUV(0.0f, 0.0f, 1.0f, 22.0f / 44.0f);
                }
            }
        }
    }
    
}

void UITable::show()
{
    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        if(mBackgrounds[q])
        {
            mBackgrounds[q]->show();
        }

        if(mTexts[q])
        {
            mTexts[q]->show();
        }
    }

    mIsShown = true;
}

void UITable::hide()
{
    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        if(mBackgrounds[q])
        {
            mBackgrounds[q]->hide();
        }

        if(mTexts[q])
        {
            mTexts[q]->hide();
        }
    }

    mIsShown = false;
}

void UITable::clear()
{
    for(size_t q = 0; q < GameState::mRaceGridCarsMax; ++q)
    {
        if(mTexts[q])
        {
            mTexts[q]->setCaption("");
        }
    }
}

void UITable::setText(const Ogre::String& text, size_t row, const Ogre::ColourValue& color)
{
    if(row < GameState::mRaceGridCarsMax)
    {
        if(!mTexts.empty() && mTexts[row])
        {
            mTexts[row]->setColour(color);
            mTexts[row]->setCaption(text);
        }
    }
}

Ogre::String UITable::getText(size_t row)const
{
    Ogre::String ret = "";

    if(row < GameState::mRaceGridCarsMax)
    {
        if(!mTexts.empty() && mTexts[row])
        {
            ret = mTexts[row]->getCaption();
        }
    }

    return ret;
}