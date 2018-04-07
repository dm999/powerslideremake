#include "UIButton.h"

#include "../../loaders/TextureLoader.h"

#include "../../tools/OgreTools.h"


Ogre::NameGenerator UIButton::nameGenMaterials("UIButton/Material");
Ogre::NameGenerator UIButton::nameGenPanel("UIButton/Panel");

UIButton::UIButton() :
    mIsShown(true), mIsPressed(false),
    mBackground(NULL),
    mOnAction(NULL)
{
    mMaterialName = nameGenMaterials.generate();
    mPanelName = nameGenPanel.generate();
}

void UIButton::loadBackground(const PFLoader& pfLoaderGameshell, const std::string& textureName)
{
    {
        std::vector<Ogre::String> texName;
        texName.push_back(textureName);
        Ogre::MaterialPtr newMat = CloneMaterial(  mMaterialName, 
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

void UIButton::setBackgroundMaterial(const std::string& name)
{
    mMaterialName = name;
}

void UIButton::init(const Ogre::Matrix4& screenAdaptionRelative, 
                    Ogre::OverlayContainer* mainBackground, 
                    const Ogre::Vector4& dimensions)
{
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f; 

    {
        Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(dimensions.x, dimensions.y, dimensions.x + dimensions.z, dimensions.y + dimensions.w);

        mBackground = createPanel(mPanelName, background, mMaterialName);
        mBackground->setUV(0.0f, 0.0f, 1.0f, 15.0f / 60.0f);
        mainBackground->addChild(mBackground);
    }
}

void UIButton::mousePressed(const Ogre::Vector2& pos)
{
    if(mBackground && mIsShown)
    {
        if(OgreBites::Widget::isCursorOver(mBackground, pos, 0))
        {
            mBackground->setUV(0.0f, 30.0f / 60.0f, 1.0f, 45.0f / 60.0f);

            mIsPressed = true;

            if(mOnAction)
            {
                mOnAction->onButtonPressed(this);
            }
        }
    }
}

void UIButton::mouseReleased(const Ogre::Vector2& pos)
{
    if(mBackground && mIsShown && mIsPressed)
    {
        if(OgreBites::Widget::isCursorOver(mBackground, pos, 0))
            mBackground->setUV(0.0f, 15.0f / 60.0f, 1.0f, 30.0f / 60.0f);
        else
            mBackground->setUV(0.0f, 0.0f, 1.0f, 15.0f / 60.0f);

        mIsPressed = false;

        if(mOnAction)
        {
            mOnAction->onButtonReleased(this);
        }
    }
}

void UIButton::mouseMoved(const Ogre::Vector2& pos)
{
    if(mBackground && mIsShown)
    {
        if(OgreBites::Widget::isCursorOver(mBackground, pos, 0))
        {
            if(mIsPressed)
                mBackground->setUV(0.0f, 30.0f / 60.0f, 1.0f, 45.0f / 60.0f);
            else
                mBackground->setUV(0.0f, 15.0f / 60.0f, 1.0f, 30.0f / 60.0f);
        }
        else
        {
            mBackground->setUV(0.0f, 0.0f, 1.0f, 15.0f / 60.0f);
        }
    }
    
}

void UIButton::show()
{
    if(mBackground)
        mBackground->show();

    mIsShown = true;
}

void UIButton::hide()
{
    if(mBackground)
        mBackground->hide();

    mIsShown = false;
}


void UIButtonTick::init(const Ogre::Matrix4& screenAdaptionRelative, 
                        Ogre::OverlayContainer* mainBackground, 
                        const Ogre::Vector4& dimensions,
                        bool isChecked,
                        bool isActive)
{
    UIButton::init(screenAdaptionRelative, mainBackground, dimensions);

    mIsChecked = isChecked;
    mIsActive = isActive;

    if(!mIsChecked)
    {
        mBackground->setUV(0.0f, 15.0f / 60.0f, 1.0f, 30.0f / 60.0f);
    }

    if(!mIsActive && mIsChecked)
    {
        mBackground->setUV(0.0f, 30.0f / 60.0f, 1.0f, 45.0f / 60.0f);
    }

    if(!mIsActive && !mIsChecked)
    {
        mBackground->setUV(0.0f, 45.0f / 60.0f, 1.0f, 60.0f / 60.0f);
    }
}

void UIButtonTick::mousePressed(const Ogre::Vector2& pos)
{
    if(mBackground && mIsShown && mIsActive)
    {
        if(OgreBites::Widget::isCursorOver(mBackground, pos, 0))
        {
            mIsChecked = !mIsChecked;

            if(mIsChecked)
                mBackground->setUV(0.0f, 0.0f / 60.0f, 1.0f, 15.0f / 60.0f);
            else
                mBackground->setUV(0.0f, 15.0f / 60.0f, 1.0f, 30.0f / 60.0f);

            mIsPressed = true;

            if(mOnAction)
            {
                mOnAction->onButtonPressed(this);
            }
        }
    }
}

void UIButtonTick::mouseReleased(const Ogre::Vector2& pos)
{
    if(mBackground && mIsShown && mIsPressed && mIsActive)
    {
        mIsPressed = false;

        if(mOnAction)
        {
            mOnAction->onButtonReleased(this);
        }
    }
}
