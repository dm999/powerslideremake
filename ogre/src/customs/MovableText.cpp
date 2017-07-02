/**
* File: MovableText.cpp
*
* description: This create create a billboarding object that display a text.
* 
* @author	2003 by cTh see gavocanov@rambler.ru
* @update	2006 by barraq see nospam@barraquand.com
* @update	2012 to work with newer versions of OGRE by MindCalamity <mindcalamity@gmail.com>
*	- See "Notes" on: http://www.ogre3d.org/tikiwiki/tiki-editpage.php?page=MovableText
*/
 

//unicode support added from
//http://www.ogre3d.org/forums/viewtopic.php?f=2&t=42630


#include "MovableText.h"

using namespace Ogre;
 
#define POS_TEX_BINDING    0
#define COLOUR_BINDING     1
#define UNICODE_NEL 0x0085
#define UNICODE_CR 0x000D
#define UNICODE_LF 0x000A
#define UNICODE_SPACE 0x0020
#define UNICODE_ZERO 0x0030
 
MovableText::MovableText(const String &name, const DisplayString &caption, const String &fontName, Real charHeight, const ColourValue &color)
: mpCam(NULL)
, mpWin(NULL)
, mpFont(NULL)
, mName(name)
, mCaption(caption)
, mFontName(fontName)
, mCharHeight(charHeight)
, mColor(color)
, mType("MovableText")
, mTimeUntilNextToggle(0)
, mSpaceWidth(0)
, mUpdateColors(true)
, mOnTop(false)
, mHorizontalAlignment(H_LEFT)
, mVerticalAlignment(V_BELOW)
, mGlobalTranslation(0.0)
, mLocalTranslation(0.0)
{
    assert(name != "" && "Trying to create MovableText without name");
 
    assert(caption != "" && "Trying to create MovableText without caption");
 
    mRenderOp.vertexData = NULL;
    this->setFontName(mFontName);
    this->_setupGeometry();
}
 
MovableText::~MovableText()
{
    if (mRenderOp.vertexData)
        delete mRenderOp.vertexData;
    // May cause crashing... check this and comment if it does
    if (!mpMaterial.isNull())
        MaterialManager::getSingletonPtr()->remove(mpMaterial->getName());
}
 
void MovableText::setFontName(const String &fontName)
{
    if((Ogre::MaterialManager::getSingletonPtr()->resourceExists(mName + "Material"))) 
    { 
        Ogre::MaterialManager::getSingleton().remove(mName + "Material"); 
    }
 
    if (mFontName != fontName || mpMaterial.isNull() || !mpFont)
    {
        mFontName = fontName;
 
        mpFont = (Font *)FontManager::getSingleton().getByName(mFontName).getPointer();
        if (!mpFont)
            throw Exception(Exception::ERR_ITEM_NOT_FOUND, "Could not find font " + fontName, "MovableText::setFontName");
 
        mpFont->load();
        if (!mpMaterial.isNull())
        {
            MaterialManager::getSingletonPtr()->remove(mpMaterial->getName());
            mpMaterial.setNull();
        }
 
        mpMaterial = mpFont->getMaterial()->clone(mName + "Material");
        if (!mpMaterial->isLoaded())
            mpMaterial->load();
 
        mpMaterial->setDepthCheckEnabled(!mOnTop);
        mpMaterial->setDepthBias(1.0,1.0);
        mpMaterial->setDepthWriteEnabled(mOnTop);
        mpMaterial->setLightingEnabled(false);
        mNeedUpdate = true;
    }
}
 
void MovableText::setCaption(const DisplayString &caption)
{
    if (caption != mCaption)
    {
        mCaption = caption;
        mNeedUpdate = true;
    }
}
 
void MovableText::setColor(const ColourValue &color)
{
    if (color != mColor)
    {
        mColor = color;
        mUpdateColors = true;
    }
}
 
void MovableText::setCharacterHeight(Real height)
{
    if (height != mCharHeight)
    {
        mCharHeight = height;
        mNeedUpdate = true;
    }
}
 
void MovableText::setSpaceWidth(Real width)
{
    if (width != mSpaceWidth)
    {
        mSpaceWidth = width;
        mNeedUpdate = true;
    }
}
 
void MovableText::setTextAlignment(const HorizontalAlignment& horizontalAlignment, const VerticalAlignment& verticalAlignment)
{
    if(mHorizontalAlignment != horizontalAlignment)
    {
        mHorizontalAlignment = horizontalAlignment;
        mNeedUpdate = true;
    }
    if(mVerticalAlignment != verticalAlignment)
    {
        mVerticalAlignment = verticalAlignment;
        mNeedUpdate = true;
    }
}
 
void MovableText::setGlobalTranslation( Vector3 trans )
{
    mGlobalTranslation = trans;
}
 
void MovableText::setLocalTranslation( Vector3 trans )
{
    mLocalTranslation = trans;
}
 
void MovableText::showOnTop(bool show)
{
    if( mOnTop != show && !mpMaterial.isNull() )
    {
        mOnTop = show;
        mpMaterial->setDepthBias(1.0,1.0);
        mpMaterial->setDepthCheckEnabled(!mOnTop);
        mpMaterial->setDepthWriteEnabled(mOnTop);
    }
}
void MovableText::_setupGeometry()
{
    assert(mpFont);
    assert(!mpMaterial.isNull());

    unsigned int vertexCount = static_cast<unsigned int>(mCaption.size() * 6);

    if (mRenderOp.vertexData)
    {
        // Removed this test as it causes problems when replacing a caption
        // of the same size: replacing "Hello" with "hello"
        // as well as when changing the text alignment
        //if (mRenderOp.vertexData->vertexCount != vertexCount)
        {
            delete mRenderOp.vertexData;
            mRenderOp.vertexData = NULL;
            mUpdateColors = true;
        }
    }

    if (!mRenderOp.vertexData)
        mRenderOp.vertexData = new VertexData();

    mRenderOp.indexData = 0;
    mRenderOp.vertexData->vertexStart = 0;
    mRenderOp.vertexData->vertexCount = vertexCount;
    mRenderOp.operationType = RenderOperation::OT_TRIANGLE_LIST; 
    mRenderOp.useIndexes = false; 

    VertexDeclaration  *decl = mRenderOp.vertexData->vertexDeclaration;
    VertexBufferBinding   *bind = mRenderOp.vertexData->vertexBufferBinding;
    size_t offset = 0;

    // create/bind positions/tex.ccord. buffer
    if (!decl->findElementBySemantic(VES_POSITION))
        decl->addElement(POS_TEX_BINDING, offset, VET_FLOAT3, VES_POSITION);

    offset += VertexElement::getTypeSize(VET_FLOAT3);

    if (!decl->findElementBySemantic(VES_TEXTURE_COORDINATES))
        decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

    HardwareVertexBufferSharedPtr ptbuf = HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(POS_TEX_BINDING),
        mRenderOp.vertexData->vertexCount,
        HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
    bind->setBinding(POS_TEX_BINDING, ptbuf);

    // Colours - store these in a separate buffer because they change less often
    if (!decl->findElementBySemantic(VES_DIFFUSE))
        decl->addElement(COLOUR_BINDING, 0, VET_COLOUR, VES_DIFFUSE);

    HardwareVertexBufferSharedPtr cbuf = HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(COLOUR_BINDING),
        mRenderOp.vertexData->vertexCount,
        HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
    bind->setBinding(COLOUR_BINDING, cbuf);

//    size_t charlen = mCaption.size();
    float *pPCBuff = static_cast<float*>(ptbuf->lock(HardwareBuffer::HBL_DISCARD));

    float largestWidth = 0;
    float left = 0 * 2.0 - 1.0;
    float top = -((0 * 2.0) - 1.0);

    Real spaceWidth = mSpaceWidth;
    // Derive space width from a capital A
    if (spaceWidth == 0)
        spaceWidth = mpFont->getGlyphAspectRatio(UNICODE_ZERO) * mCharHeight * 2.0f;

    // for calculation of AABB
    Ogre::Vector3 min, max, currPos;
    Ogre::Real maxSquaredRadius;
    bool first = true;

    // Use iterator
    DisplayString::iterator i, iend;
    iend = mCaption.end();
    bool newLine = true;
    Real len = 0.0f;

    Real verticalOffset = 0;
    switch (mVerticalAlignment)
    {
    case MovableText::V_ABOVE:
        verticalOffset = mCharHeight;
        break;
    case MovableText::V_CENTER:
        verticalOffset = 0.5f * mCharHeight;
        break;
    case MovableText::V_BELOW:
        verticalOffset = 0;
        break;
    }

    // Raise the first line of the caption
    top += verticalOffset;
    for (i = mCaption.begin(); i != iend; ++i)
    {      
      Font::CodePoint character = OGRE_DEREF_DISPLAYSTRING_ITERATOR(i);
      if (character == UNICODE_CR
        || character == UNICODE_NEL
        || character == UNICODE_LF)
            top += verticalOffset * 2.0f;
    }

    for (i = mCaption.begin(); i != iend; ++i)
    {
        if (newLine)
        {
            len = 0.0f;
            for (DisplayString::iterator j = i; j != iend; j++)
            {
              Font::CodePoint character = OGRE_DEREF_DISPLAYSTRING_ITERATOR(j);
                if (character == UNICODE_CR
                || character == UNICODE_NEL
                || character == UNICODE_LF)
              {
                break;
              }
              else if (character == UNICODE_SPACE) // space
              {
                len += mSpaceWidth;
              }
              else
              {
                len += mpFont->getGlyphAspectRatio(character) * mCharHeight * 2.0f;
              }
            }
            newLine = false;
        }
        
      Font::CodePoint character = OGRE_DEREF_DISPLAYSTRING_ITERATOR(i);
      if (character == UNICODE_CR
        || character == UNICODE_NEL
        || character == UNICODE_LF)
      {
        left = 0 * 2.0 - 1.0;
        top -= mCharHeight * 2.0f;
        newLine = true;
        // Also reduce tri count
        mRenderOp.vertexData->vertexCount -= 6;

        // consume CR/LF in one
        if (character == UNICODE_CR)
        {
          DisplayString::iterator peeki = i;
          peeki++;
          if (peeki != iend && OGRE_DEREF_DISPLAYSTRING_ITERATOR(peeki) == UNICODE_LF)
          {
            i = peeki; // skip both as one newline
            // Also reduce tri count
            mRenderOp.vertexData->vertexCount -= 6;
          }

        }
        continue;
      }
      else if (character == UNICODE_SPACE) // space
      {
        // Just leave a gap, no tris
        left += mSpaceWidth;
        // Also reduce tri count
        mRenderOp.vertexData->vertexCount -= 6;
        continue;
      }

        Real horiz_height = mpFont->getGlyphAspectRatio(character);
        Real u1, u2, v1, v2; 
        Ogre::Font::UVRect utmp;
        utmp = mpFont->getGlyphTexCoords(character);
        u1 = utmp.left;
        u2 = utmp.right;
        v1 = utmp.top;
        v2 = utmp.bottom;

        // each vert is (x, y, z, u, v)
        //-------------------------------------------------------------------------------------
        // First tri
        //
        // Upper left
        if(mHorizontalAlignment == MovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u1;
        *pPCBuff++ = v1;

        // Deal with bounds
        if(mHorizontalAlignment == MovableText::H_LEFT)
            currPos = Ogre::Vector3(left, top, -1.0);
        else
            currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
        if (first)
        {
            min = max = currPos;
            maxSquaredRadius = currPos.squaredLength();
            first = false;
        }
        else
        {
            min.makeFloor(currPos);
            max.makeCeil(currPos);
            maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
        }

        top -= mCharHeight * 2.0f;

        // Bottom left
        if(mHorizontalAlignment == MovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u1;
        *pPCBuff++ = v2;

        // Deal with bounds
        if(mHorizontalAlignment == MovableText::H_LEFT)
            currPos = Ogre::Vector3(left, top, -1.0);
        else
            currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
        min.makeFloor(currPos);
        max.makeCeil(currPos);
        maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

        top += mCharHeight * 2.0f;
        left += horiz_height * mCharHeight * 2.0f;

        // Top right
        if(mHorizontalAlignment == MovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u2;
        *pPCBuff++ = v1;
        //-------------------------------------------------------------------------------------

        // Deal with bounds
        if(mHorizontalAlignment == MovableText::H_LEFT)
            currPos = Ogre::Vector3(left, top, -1.0);
        else
            currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
        min.makeFloor(currPos);
        max.makeCeil(currPos);
        maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

        //-------------------------------------------------------------------------------------
        // Second tri
        //
        // Top right (again)
        if(mHorizontalAlignment == MovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u2;
        *pPCBuff++ = v1;

        currPos = Ogre::Vector3(left, top, -1.0);
        min.makeFloor(currPos);
        max.makeCeil(currPos);
        maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

        top -= mCharHeight * 2.0f;
        left -= horiz_height  * mCharHeight * 2.0f;

        // Bottom left (again)
        if(mHorizontalAlignment == MovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u1;
        *pPCBuff++ = v2;

        currPos = Ogre::Vector3(left, top, -1.0);
        min.makeFloor(currPos);
        max.makeCeil(currPos);
        maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

        left += horiz_height  * mCharHeight * 2.0f;

        // Bottom right
        if(mHorizontalAlignment == MovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u2;
        *pPCBuff++ = v2;
        //-------------------------------------------------------------------------------------

        currPos = Ogre::Vector3(left, top, -1.0);
        min.makeFloor(currPos);
        max.makeCeil(currPos);
        maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

        // Go back up with top
        top += mCharHeight * 2.0f;

        float currentWidth = (left + 1)/2 - 0;
        if (currentWidth > largestWidth)
            largestWidth = currentWidth;
    }

    // Unlock vertex buffer
    ptbuf->unlock();

    // update AABB/Sphere radius
    mAABB = Ogre::AxisAlignedBox(min, max);
    mRadius = Ogre::Math::Sqrt(maxSquaredRadius);

    if (mUpdateColors)
        this->_updateColors();

    mNeedUpdate = false;
}

 
void MovableText::_updateColors(void)
{
    assert(mpFont);
    assert(!mpMaterial.isNull());
 
    // Convert to system-specific
    RGBA color;
    Root::getSingleton().convertColourValue(mColor, &color);
    HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(COLOUR_BINDING);
    RGBA *pDest = static_cast<RGBA*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
    for (int i = 0; i < (int)mRenderOp.vertexData->vertexCount; ++i)
        *pDest++ = color;
    vbuf->unlock();
    mUpdateColors = false;
}
 
const Quaternion& MovableText::getWorldOrientation(void) const
{
    assert(mpCam);
    return const_cast<Quaternion&>(mpCam->getDerivedOrientation());
}
 
// Add to build on Shoggoth:
void MovableText::visitRenderables(Ogre::Renderable::Visitor* visitor, 
        bool debugRenderables)
{
}
 
const Vector3& MovableText::getWorldPosition(void) const
{
    assert(mParentNode);
    return mParentNode->_getDerivedPosition();
}
 
void MovableText::getWorldTransforms(Matrix4 *xform) const 
{
    if (this->isVisible() && mpCam)
    {
        Matrix3 rot3x3, scale3x3 = Matrix3::IDENTITY;
 
        // store rotation in a matrix
        mpCam->getDerivedOrientation().ToRotationMatrix(rot3x3);
 
        // parent node position
        Vector3 ppos = mParentNode->_getDerivedPosition() + Vector3::UNIT_Y*mGlobalTranslation;
        ppos += rot3x3*mLocalTranslation;
 
        // apply scale
        scale3x3[0][0] = mParentNode->_getDerivedScale().x / 2;
        scale3x3[1][1] = mParentNode->_getDerivedScale().y / 2;
        scale3x3[2][2] = mParentNode->_getDerivedScale().z / 2;
 
        // apply all transforms to xform       
        *xform = (rot3x3 * scale3x3);
        xform->setTrans(ppos);
    }
}
 
void MovableText::getRenderOperation(RenderOperation &op)
{
    if (this->isVisible())
    {
        if (mNeedUpdate)
            this->_setupGeometry();
        if (mUpdateColors)
            this->_updateColors();
        op = mRenderOp;
    }
}
 
void MovableText::_notifyCurrentCamera(Camera *cam)
{
    mpCam = cam;
}
 
void MovableText::_updateRenderQueue(RenderQueue* queue)
{
    if (this->isVisible())
    {
        if (mNeedUpdate)
            this->_setupGeometry();
        if (mUpdateColors)
            this->_updateColors();
 
        queue->addRenderable(this, mRenderQueueID, OGRE_RENDERABLE_DEFAULT_PRIORITY);
        //queue->addRenderable(this, mRenderQueueID, RENDER_QUEUE_SKIES_LATE);
    }
}