#include "UIEditBox.h"

#include "../../loaders/TextureLoader.h"

#include "../../tools/OgreTools.h"

#if defined(__ANDROID__)
    #include <android/keycodes.h>
#endif

Ogre::NameGenerator UIEditBox::nameGenTextures("UIEditBox/Texture");
Ogre::NameGenerator UIEditBox::nameGenMaterials("UIEditBox/Material");
Ogre::NameGenerator UIEditBox::nameGenPanel("UIEditBox/Panel");
Ogre::NameGenerator UIEditBox::nameGenText("UIEditBox/Text");


UIEditBox::UIEditBox(size_t maxSymbols) : mCaption(""), mCaptionToDisplay(""),
    mIsShown(true), mIsCaretShown(true), mIsActive(false),
    mMaxSymbols(maxSymbols),
    mBackground(NULL), mText(NULL),
    mCaretSize(9.0f),
    mCharType(Any)
{
    mTextureName = nameGenTextures.generate();
    mMaterialName = nameGenMaterials.generate();
    mPanelName = nameGenPanel.generate();
    mTextName = nameGenText.generate();
}

void UIEditBox::loadBackground(const PFLoader& pfLoaderGameshell, const std::string& filename)
{
    TextureLoader().load( pfLoaderGameshell, 
        "data/gameshell", filename, 
        mTextureName, TEMP_RESOURCE_GROUP_NAME);

    {
        std::vector<Ogre::String> texName;
        texName.push_back(mTextureName);
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

void UIEditBox::setBackgroundMaterial(const std::string& name)
{
    mMaterialName = name;
}

void UIEditBox::init(const Ogre::Matrix4& screenAdaptionRelative, 
                     Ogre::OverlayContainer* mainBackground,
                     const Ogre::Vector4& dimensions,
                     Ogre::Real fontSize, 
                     bool isActive)
{
    Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f; 

    mIsActive = isActive;

    {
        Ogre::Vector4 background = screenAdaptionRelative * Ogre::Vector4(dimensions.x, dimensions.y, dimensions.x + dimensions.z, dimensions.y + dimensions.w);

        mBackground = createPanel(mPanelName, background, mMaterialName);
        mBackground->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        mainBackground->addChild(mBackground);
    }

    {
        Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(2.0f, 0.0f, 0.0f, 0.0f);
        mText = createTextArea(mTextName, 0.0f, 0.0f, textBoxPos.x, textBoxPos.y); 
        mText->setCaption(mCaption);
        mText->setCharHeight(fontSize * viewportHeight / 1024.0f);
        mText->setSpaceWidth(9.0f);
        mText->setAlignment(Ogre::TextAreaOverlayElement::Left);
        mText->setFontName("SdkTrays/Caption");
        mText->setColour(Ogre::ColourValue::White);
        mBackground->addChild(mText);

        mCaretSize = (fontSize / 2.0f) * viewportHeight / 1024.0f;
    }
}

void UIEditBox::frameStarted(const Ogre::FrameEvent &evt)
{
    const unsigned long blinkThreshold = 500; //ms
    if(mIsShown)
    {
        if(mIsCaretShown)
        {
            unsigned long lastBlink = mBlinkTimer.getMilliseconds();
            if(lastBlink >= blinkThreshold)
            {
                mIsCaretShown = false;
                mBlinkTimer.reset();
            }
        }
        else
        {
            unsigned long lastBlink = mBlinkTimer.getMilliseconds();
            if(lastBlink >= blinkThreshold)
            {
                mIsCaretShown = true;
                mBlinkTimer.reset();
            }
        }

        if(mText)
        {
            if(mIsCaretShown && mIsActive)
                mText->setCaption(mCaptionToDisplay + "|");
            else
                mText->setCaption(mCaptionToDisplay);
        }
    }
}

void UIEditBox::keyUp(MyGUI::KeyCode _key, wchar_t _char)
{
    if(mIsActive)
    {
#if defined(__ANDROID__)
        if (_key == MyGUI::KeyCode::Enum(AKEYCODE_DEL))
#else
        if (_key == MyGUI::KeyCode::Backspace)
#endif
        {
            if(mCaption.length() > 0)
            {
                mText->setWidth(0);
                mCaption = mCaption.substr(0, mCaption.length() - 1);
                adjustCaptionLength();
            }
        }
        else if (isLegalCharacter(_key))
        {
            if(mCaption.length() < mMaxSymbols)
            {
                mCaption = mCaption.asWStr() + _char;
                adjustCaptionLength();
            }
        }
    }
}

void UIEditBox::adjustCaptionLength()
{
    mCaptionToDisplay = mCaption;
    mText->setWidth(0);
    mText->setCaption(mCaptionToDisplay);
    mText->_update();
    float width = mText->getWidth();
    while(width >= (mBackground->getWidth() - mCaretSize))
    {
        mCaptionToDisplay = mCaptionToDisplay.substr(1, mCaptionToDisplay.length());
        mText->setWidth(0);
        mText->setCaption(mCaptionToDisplay);
        mText->_update();
        width = mText->getWidth();
    }
}

bool UIEditBox::isLegalCharacter(MyGUI::KeyCode _key) const
{
    bool ret = false;

    if(mCharType == Any)
    {
#if defined(__ANDROID__)
        if(
            _key == MyGUI::KeyCode::Enum(AKEYCODE_DEL)          ||
            _key == MyGUI::KeyCode::Enum(AKEYCODE_COMMA)        ||
            _key == MyGUI::KeyCode::Enum(AKEYCODE_PERIOD)       ||
            _key == MyGUI::KeyCode::Enum(AKEYCODE_SPACE)        ||
            (_key >= MyGUI::KeyCode::Enum(AKEYCODE_MINUS) && _key <= MyGUI::KeyCode::Enum(AKEYCODE_AT)) ||
            (_key >= MyGUI::KeyCode::Enum(AKEYCODE_0) && _key <= MyGUI::KeyCode::Enum(AKEYCODE_9))      ||
            (_key >= MyGUI::KeyCode::Enum(AKEYCODE_A) && _key <= MyGUI::KeyCode::Enum(AKEYCODE_Z))
        ) ret = true;
#else
        if(
            _key != MyGUI::KeyCode::None            &&
            _key != MyGUI::KeyCode::Escape          &&
            _key != MyGUI::KeyCode::Delete          &&
            _key != MyGUI::KeyCode::Insert          &&
            _key != MyGUI::KeyCode::Return          &&
            _key != MyGUI::KeyCode::NumpadEnter     &&
            _key != MyGUI::KeyCode::ArrowRight      &&
            _key != MyGUI::KeyCode::ArrowLeft       &&
            _key != MyGUI::KeyCode::ArrowUp         &&
            _key != MyGUI::KeyCode::ArrowDown       &&
            _key != MyGUI::KeyCode::Home            &&
            _key != MyGUI::KeyCode::End             &&
            _key != MyGUI::KeyCode::Tab             &&
            _key != MyGUI::KeyCode::PageUp          &&
            _key != MyGUI::KeyCode::PageDown        &&
            _key != MyGUI::KeyCode::LeftControl     &&
            _key != MyGUI::KeyCode::RightControl    &&
            _key != MyGUI::KeyCode::LeftShift       &&
            _key != MyGUI::KeyCode::RightShift      &&
            _key != MyGUI::KeyCode::LeftAlt         &&
            _key != MyGUI::KeyCode::RightAlt        &&
            _key != MyGUI::KeyCode::F1              &&
            _key != MyGUI::KeyCode::F2              &&
            _key != MyGUI::KeyCode::F3              &&
            _key != MyGUI::KeyCode::F4              &&
            _key != MyGUI::KeyCode::F5              &&
            _key != MyGUI::KeyCode::F6              &&
            _key != MyGUI::KeyCode::F7              &&
            _key != MyGUI::KeyCode::F8              &&
            _key != MyGUI::KeyCode::F9              &&
            _key != MyGUI::KeyCode::F10             &&
            _key != MyGUI::KeyCode::F11             &&
            _key != MyGUI::KeyCode::F12             &&
            _key != MyGUI::KeyCode::Capital         &&
            _key != MyGUI::KeyCode::NumLock         &&
            _key != MyGUI::KeyCode::ScrollLock
        ) ret = true;
#endif
    }

    if(mCharType == IP)
    {
#if defined(__ANDROID__)
        if(
            _key == MyGUI::KeyCode::Enum(AKEYCODE_PERIOD) ||
            (_key >= MyGUI::KeyCode::Enum(AKEYCODE_0) && _key <= MyGUI::KeyCode::Enum(AKEYCODE_9))
        ) ret = true;
#else
        if(
            _key == MyGUI::KeyCode::Period  ||
            (_key >= MyGUI::KeyCode::One && _key <= MyGUI::KeyCode::Zero)
        ) ret = true;
#endif
    }

    return ret;
}

void UIEditBox::mouseReleased(const Ogre::Vector2& pos)
{
    if(mBackground && mIsShown && OgreBites::Widget::isCursorOver(mBackground, pos, 0))
    {
        mIsActive = true;
        mIsCaretShown = true;
        mBlinkTimer.reset();
    }
    else
        mIsActive = false;
}

void UIEditBox::setText(const Ogre::DisplayString& str)
{
    mCaption = str;

    if(mCaption.length() < mMaxSymbols)
    {
        adjustCaptionLength();
    }
}

void UIEditBox::setColor(const Ogre::ColourValue& color)
{
    mText->setColour(color);
}

void UIEditBox::show()
{
    if(mBackground)
        mBackground->show();

    if(mText)
        mText->show();

    mIsShown = true;
    mIsCaretShown = true;
    mBlinkTimer.reset();
}

void UIEditBox::hide()
{
    if(mBackground)
        mBackground->hide();

    if(mText)
        mText->hide();

    mIsShown = false;
}