
#include "OgreTools.h"
#include "Tools.h"
#include "Conversions.h"

#include "../loaders/PFLoader.h"

Ogre::MaterialPtr CloneMaterial(const Ogre::String& newMaterialName, const Ogre::String& oldMaterialName, 
                                const std::vector<Ogre::String> texturesNames, 
                                float scale,
                                const Ogre::String& groupName){

    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(oldMaterialName);

    Ogre::MaterialPtr materialNew = material->clone(newMaterialName, true, groupName);

    for(size_t q = 0; q < texturesNames.size(); ++q){
        Ogre::TextureUnitState * state = materialNew->getTechnique(0)->getPass(0)->getTextureUnitState(static_cast<Ogre::ushort>(q));
        state->setTextureName(texturesNames[q]);
        state->setTextureScale(scale, scale);

        //d.polubotko: adjust OgreGLHardwearePixelBuffer.cpp in ogre 1.9.0 - add line 674
        //if((mUsage & TU_AUTOMIPMAP))
        //OGRE_CHECK_GL_ERROR(glGenerateMipmap(mFaceTarget));
#if defined(__ANDROID__)
        //state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
        /*
        if(state->getTextureFiltering(Ogre::FT_MIN) == Ogre::FO_ANISOTROPIC || state->getTextureFiltering(Ogre::FT_MAG) == Ogre::FO_ANISOTROPIC)
        {
            if(!Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_ANISOTROPY))
            {
                state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
            }
            else
            {
                Ogre::Real maxAniso = const_cast<Ogre::RenderSystemCapabilities *>(Ogre::Root::getSingleton().getRenderSystem()->getCapabilities())->getMaxSupportedAnisotropy();
                if(maxAniso < 1.0f)
                {
                    state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
                }
                else
                {
                    state->setTextureAnisotropy(static_cast<unsigned int>(maxAniso));
                }
            }
        }*/
#endif
    }

    return materialNew;
}

void TranslateMesh(const Ogre::Entity *ent, const Ogre::Vector3 &translate)
{
    bool added_shared = false;
    Ogre::Mesh* mesh = ent->getMesh().get();
    Ogre::Vector3 Minimum = Ogre::Vector3(std::numeric_limits<float>::max());
    Ogre::Vector3 Maximum = Ogre::Vector3(std::numeric_limits<float>::min());
    // Run through the submeshes, modifying the data
    for(size_t i = 0; i < mesh->getNumSubMeshes(); i++)
    {
        Ogre::SubMesh * submesh = mesh->getSubMesh(static_cast<Ogre::ushort>(i));
        Ogre::VertexData * vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
        if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
            }
            const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
            // lock buffer for read and write access
            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));
            float* pReal;
            for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                // modify x coord
                (*pReal) += translate[0];
                ++pReal;
                // modify y coord
                (*pReal) += translate[1];
                ++pReal;
                // modify z coord
                (*pReal) += translate[2];
                pReal-=2;
                Minimum[0]=Minimum[0]<(*pReal) ? Minimum[0] : (*pReal);
                Maximum[0]=Maximum[0]>(*pReal) ? Maximum[0] : (*pReal);
                ++pReal;
                Minimum[1]=Minimum[1]<(*pReal) ? Minimum[1] : (*pReal);
                Maximum[1]=Maximum[1]>(*pReal) ? Maximum[1] : (*pReal);
                ++pReal;
                Minimum[2]=Minimum[2]<(*pReal) ? Minimum[2] : (*pReal);
                Maximum[2]=Maximum[2]>(*pReal) ? Maximum[2] : (*pReal);
            }
            vbuf->unlock();
        }
    }
    mesh->_setBounds(Ogre::AxisAlignedBox(Minimum,Maximum),false);
}

Ogre::Vector3 FindCentroid(const Ogre::Entity *ent)
{
    Ogre::Vector3 res = Ogre::Vector3::ZERO;
    size_t vertexNum = 0;

    Ogre::Mesh* mesh = ent->getMesh().get();
    // Run through the submeshes, modifying the data
    for(size_t i = 0; i < mesh->getNumSubMeshes(); i++)
    {
        Ogre::SubMesh * submesh = mesh->getSubMesh(static_cast<Ogre::ushort>(i));
        Ogre::VertexData * vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
        Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
        // lock buffer for read and write access
        unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        float* pReal;

        vertexNum += vertex_data->vertexCount;
        for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
        {
            posElem->baseVertexPointerToElement(vertex, &pReal);

            Ogre::Vector3 vertex2;
            vertex2.x = (*pReal);
            ++pReal;
            vertex2.y = (*pReal);
            ++pReal;
            vertex2.z = (*pReal);

            res += vertex2;
        }
        vbuf->unlock();
    }

    res /= static_cast<float>(vertexNum);
    return res;
}

void BuildTangents(Ogre::Entity* entity)
{
    Ogre::MeshPtr tmpMesh = entity->getMesh();
    unsigned short tmpSrc, tmpDest;
    if (false == tmpMesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, tmpSrc, tmpDest))
    {
        tmpMesh->buildTangentVectors(Ogre::VES_TANGENT, 0, tmpDest);
    }
}

void AddColorAttrib(Ogre::Entity* entity)
{
    Ogre::ColourValue colour(1.0f, 1.0f, 1.0f);

    Ogre::Mesh* mesh = entity->getMesh().get();

    //d.polubotko: assume we use shared data (check details in TangentSpaceCalc::insertTangents)
    Ogre::VertexData * mVData = mesh->sharedVertexData;

    Ogre::VertexDeclaration* vDecl = mVData->vertexDeclaration;
    Ogre::VertexBufferBinding *vBind = mVData->vertexBufferBinding;

    const Ogre::VertexElement* prevTexCoordElem = mVData->vertexDeclaration->findElementBySemantic(Ogre::VES_TEXTURE_COORDINATES, 0);

    Ogre::HardwareVertexBufferSharedPtr targetBuffer, origBuffer;
    unsigned char* pSrc = NULL;

    origBuffer = mVData->vertexBufferBinding->getBuffer(prevTexCoordElem->getSource());

    targetBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                origBuffer->getVertexSize() + Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR),
                origBuffer->getNumVertices(),
                origBuffer->getUsage(),
                origBuffer->hasShadowBuffer() );

    const Ogre::VertexElement *colorElem;
    colorElem = &(vDecl->addElement(
                prevTexCoordElem->getSource(),
                origBuffer->getVertexSize(),
                Ogre::VET_COLOUR,
                Ogre::VES_DIFFUSE));

    pSrc = static_cast<unsigned char*>(origBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
    vBind->setBinding(prevTexCoordElem->getSource(), targetBuffer);


    unsigned char* pDest = static_cast<unsigned char*>(targetBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    size_t origVertSize = origBuffer->getVertexSize();
    size_t newVertSize = targetBuffer->getVertexSize();

    for (size_t v = 0; v < origBuffer->getNumVertices(); ++v)
    {
        memcpy(pDest, pSrc, origVertSize);
        pSrc += origVertSize;

        Ogre::RGBA* pRGBA = 0;

        colorElem->baseVertexPointerToElement(pDest, &pRGBA);

        *pRGBA = colour.getAsRGBA();

        pDest += newVertSize;
    }

    targetBuffer->unlock();
    origBuffer->unlock();


#if 0
    // Run through the submeshes, modifying the data
    for(size_t i = 0; i < mesh->getNumSubMeshes(); i++)
    {
        Ogre::SubMesh * submesh = mesh->getSubMesh(i);
        
        Ogre::VertexData * vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        
        const Ogre::VertexElement* colorElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_DIFFUSE);
        Ogre::HardwareVertexBufferSharedPtr cbuf = vertex_data->vertexBufferBinding->getBuffer(colorElem->getSource());

        unsigned char* color = static_cast<unsigned char*>(cbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));
        Ogre::RGBA* pRGBA = 0;

        for(size_t j = 0; j < vertex_data->vertexCount; ++j, color += cbuf->getVertexSize())
        {
            colorElem->baseVertexPointerToElement(color, &pRGBA);

            *pRGBA = colour.getAsRGBA();
        }
        cbuf->unlock();
    }
#endif
}

void AddjustNormals(Ogre::Entity* entity, Ogre::Real threshold)
{
    //http://www.bytehazard.com/articles/vertnorm.html
    //https://knowledge.autodesk.com/search-result/caas/CloudHelp/cloudhelp/2015/ENU/Max-SDK/files/GUID-0FCB4578-77F8-4F05-99CD-349E85F13639-htm.html

    Ogre::Mesh* mesh = entity->getMesh().get();

    //d.polubotko: assume used shared buffer
    Ogre::VertexData* vertex_data = mesh->sharedVertexData;
    const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
    const Ogre::VertexElement* posNormal = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_NORMAL);
    Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
    unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));

    size_t countInBox = 0;

    typedef std::vector<NormalAdjustmentHelper> helper;
    helper normalHelper;

    for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
    {

        Ogre::Vector3 pt;
        Ogre::Vector3 normal;

        float* pReal;
        posElem->baseVertexPointerToElement(vertex, &pReal);
        pt.x = (*pReal++);
        pt.y = (*pReal++);
        pt.z = (*pReal++);

        float* pRealNormalBase;
        float* pRealNormal;
        posNormal->baseVertexPointerToElement(vertex, &pRealNormalBase);
        pRealNormal = pRealNormalBase;
        normal.x = (*pRealNormal++);
        normal.y = (*pRealNormal++);
        normal.z = (*pRealNormal++);

        helper::iterator i = std::find(normalHelper.begin(), normalHelper.end(), pt);
        if(i != normalHelper.end())
        {
            (*i).normals.push_back(normal);
            (*i).normalsOffset.push_back(pRealNormalBase);
        }
        else
        {
            NormalAdjustmentHelper newHelper;
            newHelper.point = pt;
            newHelper.normals.push_back(normal);
            newHelper.normalsOffset.push_back(pRealNormalBase);
            normalHelper.push_back(newHelper);
        }

    }

    for(size_t q = 0; q < normalHelper.size(); ++q)
    {
        NormalAdjustmentHelper& helperRecord = normalHelper[q];

        if(helperRecord.normals.size() > 1)
        {
            //TODO(d.polubotko): consider refactoring to faster approach
            for(size_t w = 0; w < helperRecord.normals.size(); ++w)
            {
                Ogre::Vector3 baseNormal = helperRecord.normals[w];

                for(size_t e = 0; e < helperRecord.normals.size(); ++e)
                {
                    if(e == w) continue;

                    Ogre::Vector3 curNormal = helperRecord.normals[e];
                    Ogre::Real dot = baseNormal.dotProduct(curNormal);
                    if(dot > threshold && dot < 1.0f)
                    {
                        Ogre::Vector3 averagedNormal = (baseNormal + curNormal) / 2.0f;
                        helperRecord.normals[e] = averagedNormal;
                        helperRecord.normals[w] = averagedNormal;
                        baseNormal = averagedNormal;
                    }
                }
            }
        }
    }

    for(size_t q = 0; q < normalHelper.size(); ++q)
    {
        NormalAdjustmentHelper helperRecord = normalHelper[q];

        for(size_t w = 0; w < helperRecord.normals.size(); ++w)
        {
            float* pRealNormal = helperRecord.normalsOffset[w];
            Ogre::Vector3 resultedNormal = helperRecord.normals[w];
            
            (*pRealNormal++) = resultedNormal.x;
            (*pRealNormal++) = resultedNormal.y;
            (*pRealNormal++) = resultedNormal.z;
        }
    }

    vbuf->unlock();

}

std::string extractFilePath(const std::string& fullName)
{
    std::string res;

    //http://stackoverflow.com/questions/8518743/get-directory-from-file-path-c

    size_t last_slash_idx = fullName.rfind('\\');
    if (std::string::npos != last_slash_idx){
        res = fullName.substr(0, last_slash_idx);
    }else{
        last_slash_idx = fullName.rfind('/');

        if (std::string::npos != last_slash_idx){
            res = fullName.substr(0, last_slash_idx);
        }
    }

    return res;
}

bool CompareVectors(const Ogre::Vector3& a, const Ogre::Vector3& b, float epsilon)
{
    return  (fabs(a.x - b.x) < epsilon) &&
            (fabs(a.y - b.y) < epsilon) &&
            (fabs(a.z - b.z) < epsilon);
}

void AdjustBufferToUseShadow(   Ogre::Entity* entity, 
                                const std::vector<std::vector<Ogre::Vector3> >& suspensionData, 
                                std::vector<std::vector<size_t> >& suspensionIndices,
                                std::vector<std::vector<Ogre::Vector3> >& suspensionPointOriginalPos)
{

    suspensionIndices.clear();
    suspensionIndices.resize(4);

    suspensionPointOriginalPos.clear();
    suspensionPointOriginalPos.resize(4);

    Ogre::Mesh* mesh = entity->getMesh().get();

    //d.polubotko: assume we use shared data (check details in TangentSpaceCalc::insertTangents)
    Ogre::VertexData * mVData = mesh->sharedVertexData;

    Ogre::VertexDeclaration* vDecl = mVData->vertexDeclaration;
    Ogre::VertexBufferBinding *vBind = mVData->vertexBufferBinding;

    const Ogre::VertexElement* prevPostElem = mVData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION, 0);

    Ogre::HardwareVertexBufferSharedPtr targetBuffer, origBuffer;
    unsigned char* pSrc = NULL;

    origBuffer = mVData->vertexBufferBinding->getBuffer(prevPostElem->getSource());

    targetBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                origBuffer->getVertexSize(),
                origBuffer->getNumVertices(),
                Ogre::HardwareBuffer::HBU_WRITE_ONLY,
                true );

    pSrc = static_cast<unsigned char*>(origBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
    vBind->setBinding(prevPostElem->getSource(), targetBuffer);


    unsigned char* pDest = static_cast<unsigned char*>(targetBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    size_t origVertSize = origBuffer->getVertexSize();

    for (size_t v = 0; v < origBuffer->getNumVertices(); ++v)
    {
        memcpy(pDest, pSrc, origVertSize);

        Ogre::Vector3 pt;

        float* pReal;
        prevPostElem->baseVertexPointerToElement(pSrc, &pReal);
        pt.x = (*pReal++);
        pt.y = (*pReal++);
        pt.z = (*pReal++);

        for(size_t q = 0; q < suspensionData.size(); ++q)
        {
            for(size_t w = 0; w < suspensionData[q].size(); ++w)
            {
                if(CompareVectors(suspensionData[q][w], pt))
                {
                    suspensionIndices[q].push_back(v);
                    suspensionPointOriginalPos[q].push_back(pt);
                }
            }
        }

        pSrc += origVertSize;
        pDest += origVertSize;
    }

    targetBuffer->unlock();
    origBuffer->unlock();

    for(size_t q = 0; q < suspensionData.size(); ++q)
    {
        //assert(suspensionData[q].size() == suspensionIndices[q].size() && "AdjustBufferToUseShadow not all suspension points found");
        if(suspensionData[q].size() != 0)
        {
            assert(0 != suspensionIndices[q].size() && "AdjustBufferToUseShadow not all suspension points found");
        }
    }
}

void AdjustSuspension(Ogre::Mesh* mesh,
                        const std::vector<std::vector<size_t> >& suspensionIndices,
                        const std::vector<std::vector<Ogre::Vector3> >& suspensionPointOriginalPos,
                        const Ogre::Real frontLSuspHeight,
                        const Ogre::Real frontRSuspHeight,
                        const Ogre::Real backLSuspHeight,
                        const Ogre::Real backRSuspHeight)
{

    //d.polubotko: assume used shared buffer
    Ogre::VertexData* vertex_data = mesh->sharedVertexData;
    const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
    Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
    unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));

    for(size_t q = 0; q < suspensionIndices.size(); ++q)
    {
        for(size_t w = 0; w < suspensionIndices[q].size(); ++w)
        {
            unsigned char* vertexOfset = vertex + vbuf->getVertexSize() * suspensionIndices[q][w];

            float* pReal;
            posElem->baseVertexPointerToElement(vertexOfset, &pReal);
            if(q == 3)
            {
                *pReal++;
                *pReal++ = suspensionPointOriginalPos[q][w].y + backRSuspHeight;
                *pReal++;
            }
            if(q == 2)
            {
                *pReal++;
                *pReal++ = suspensionPointOriginalPos[q][w].y + backLSuspHeight;
                *pReal++;
            }
            if(q == 1)
            {
                *pReal++;
                *pReal++ = suspensionPointOriginalPos[q][w].y + frontRSuspHeight;
                *pReal++;
            }
            if(q == 0)
            {
                *pReal++;
                *pReal++ = suspensionPointOriginalPos[q][w].y + frontLSuspHeight;
                *pReal++;
            }
        }
    }

    vbuf->unlock();
}

Ogre::Real GetSignedAngle(const Ogre::Vector3& a, const Ogre::Vector3& b)
{
    Ogre::Quaternion rot = a.getRotationTo(b);
    Ogre::Degree angle;
    Ogre::Vector3 axis;
    
    rot.ToAngleAxis(angle, axis);
    Ogre::Real res = angle.valueDegrees();
    if(axis.y > 0.0f) res = -res;
    return res;
}

Ogre::Vector3 GetAngleBetweenQuaternions(const Ogre::Quaternion& from, const Ogre::Quaternion& to)
{
    Ogre::Vector3 res;

    Ogre::Radian rotAngle;
    Ogre::Real valRadians;

    Ogre::Real dot = to.Dot(from);
    Ogre::Quaternion angleDiffQ = to * from.Inverse();
    angleDiffQ.ToAngleAxis(rotAngle, res);
    valRadians = rotAngle.valueRadians();
    if(dot < 0.0f)
    {
        angleDiffQ = to * (-from.Inverse());
        angleDiffQ.ToAngleAxis(rotAngle, res);
        valRadians = rotAngle.valueRadians();
    }
    res *= valRadians;

    return res;
}

Ogre::Vector3 ProjectPointOnLine(const Ogre::Vector3& a, const Ogre::Vector3& b, const Ogre::Vector3& c)
{
    //http://www.ogre3d.org/forums/viewtopic.php?f=2&t=79096
    Ogre::Vector3 delta = c-b;     // Get the vector from b to c.
    Ogre::Real lengthbc = delta.normalise();     // Get the length of the vector from b to c and normalise it.
    Ogre::Real t = (a-b).dotProduct(delta);     // t is the distance between b and c that is perpendicular to point a.

    if(t<0) t=0;     // You can leave this out if you want the line to be infinite (it just goes through b, it doesn't stop there)
    if(t>lengthbc) t=lengthbc;     // You can leave this out if you want the line to be infinite (it just goes through c, it doesn't stop there)

    //Ogre::Real distance = a.distance(b+delta*t);     // b+delta*t gives you the nearest point on the bc line to point a. Now get the distance between them.
    //return distance;

    //d.polubotko: point within line
    return b+delta*t;
}

Ogre::Vector3 NearestPoint(const Ogre::Vector3 &pt1, const Ogre::Vector3 &pt2, const Ogre::Vector3 &testPoint)
{
    //http://www.ogre3d.org/tikiwiki/Nearest+point+on+a+line

    const Ogre::Vector3 A = testPoint - pt1;
    const Ogre::Vector3 u = (pt2-pt1).normalisedCopy();
 
    return pt1 + (A.dotProduct(u)) * u;
}

//http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Creating+transparency+based+on+a+key+colour+in+code
Ogre::TexturePtr loadChromaKeyedTexture(const Ogre::String& filename,
         const Ogre::ColourValue& keyCol,
         const Ogre::String& resGroup, 
         const Ogre::String& prefix,
         int numMipmaps,
         float threshold,
         bool resize,
         size_t newSize) 
 { 
     using namespace Ogre; 
     using std::fabs; 

     const PixelFormat targetTextureFormat = PF_A8R8G8B8;

     Image srcImg; 
     srcImg.load (filename, resGroup); 
     uint width = srcImg.getWidth (), height = srcImg.getHeight ();
     // Since Ogre 1.6 Shoggoth, the OGRE_ALLOC_T memory macro must be used:
     uchar* pixelData = OGRE_ALLOC_T (uchar, PixelUtil::getMemorySize (width, height, 1, targetTextureFormat), MEMCATEGORY_GENERAL);
     ulong pxDataIndex = 0, pxDataIndexStep = PixelUtil::getNumElemBytes (targetTextureFormat); 
 
     for(uint y = 0; y < height; ++y) 
     { 
         for(uint x = 0; x < width; ++x) 
         { 
             ColourValue pixCol = srcImg.getColourAt (x, y, 0); 
             ColourValue diffCol = pixCol - keyCol; 
             pixCol.a = ( (fabs(diffCol.r)<threshold) && (fabs(diffCol.g)<threshold) && (fabs(diffCol.b)<threshold) ) 
                 ? 0.0f : 1.0f; 
             Ogre::PixelUtil::packColour (pixCol, targetTextureFormat, static_cast<void*> (pixelData + pxDataIndex) ); 
             pxDataIndex += pxDataIndexStep; 
         } 
     } 
 
     Image chromaKeyedImg;
     chromaKeyedImg.loadDynamicImage (pixelData, width, height, 1, targetTextureFormat, true); 
     if(resize)
        chromaKeyedImg.resize(static_cast<Ogre::ushort>(newSize), static_cast<Ogre::ushort>(newSize));
     String resName = prefix + filename;
     // You could save the chroma keyed image at this point for caching:
     // chromaKeyedImg.save(resName); 
     return TextureManager::getSingleton ().loadImage( resName , resGroup, chromaKeyedImg, TEX_TYPE_2D, numMipmaps );
}

std::string OgreColorToString(const Ogre::ColourValue& color)
{
    std::string ret;

    Ogre::Real r = color.r * 255.0f;
    Ogre::Real g = color.g * 255.0f;
    Ogre::Real b = color.b * 255.0f;

    char buf[10];
    sprintf(buf, "%02x", static_cast<Ogre::uint32>(r));
    ret += buf;
    sprintf(buf, "%02x", static_cast<Ogre::uint32>(g));
    ret += buf;
    sprintf(buf, "%02x", static_cast<Ogre::uint32>(b));
    ret += buf;

    return ret;
}

//https://www.opengl.org/discussion_boards/showthread.php/184412-Converting-to-and-from-color-space-in-GLSL-only-works-for-few-colors
Ogre::Vector3 RGBToXYZ(Ogre::Vector3 RGB, float gamma)
{
   if (RGB.x > 0.04045f) {
        RGB.x = pow(((RGB.x + 0.055f) / 1.055f), gamma);
    } else {
        RGB.x = RGB.x / 12.92f;
    }
    if (RGB.y > 0.04045f) {
        RGB.y = pow(((RGB.y + 0.055f) / 1.055f), gamma);
    } else {
        RGB.y = RGB.y / 12.92f;
    }

    if (RGB.z > 0.04045f) {
        RGB.z = pow(((RGB.z + 0.055f) / 1.055f), gamma);
    } else {
        RGB.z = RGB.z / 12.92f;
    }

    RGB.x = RGB.x * 100.0f;
    RGB.y = RGB.y * 100.0f;
    RGB.z = RGB.z * 100.0f;

    //Observer. = 2.0?, Illuminant = D65
    float X = RGB.x * 0.4124f + RGB.y * 0.3576f + RGB.z * 0.1805f;
    float Y = RGB.x * 0.2126f + RGB.y * 0.7152f + RGB.z * 0.0722f;
    float Z = RGB.x * 0.0193f + RGB.y * 0.1192f + RGB.z * 0.9505f;

    return Ogre::Vector3(X, Y, Z);
}

Ogre::Vector3 XYZToRGB(const Ogre::Vector3& XYZ, float gamma)
{
    float var_X =  XYZ.x / 100.0f; //X from 0.0 to  95.047      (Observer = 2.0 degrees, Illuminant = D65);
    float var_Y =  XYZ.y / 100.0f; //Y from 0.0 to 100.000;
    float var_Z =  XYZ.z / 100.0f; //Z from 0.0 to 108.883;

    float var_R = var_X * 3.2406f + var_Y * -1.5372f + var_Z * -0.4986f;
    float var_G = var_X * -0.9689f + var_Y * 1.8758f + var_Z * 0.0415f;
    float var_B = var_X * 0.0557f + var_Y * -0.2040f + var_Z * 1.0570f;

    if (var_R > 0.0031308f) {
        var_R = 1.055f * pow(var_R, (1.0f / gamma)) - 0.055f;
    } else {
        var_R = 12.92f * var_R;
    }
    if (var_G > 0.0031308f) {
        var_G = 1.055f * pow(var_G, (1.0f / gamma)) - 0.055f;
    } else {
        var_G = 12.92f * var_G;
    }
    if (var_B > 0.0031308f) {
        var_B = 1.055f * pow(var_B, (1.0f / gamma)) - 0.055f;
    } else {
        var_B = 12.92f * var_B;
    }

    return Ogre::Vector3(var_R, var_G, var_B);
}

size_t getPowerOf2(size_t val)
{
    return static_cast<size_t>(Ogre::Math::Pow(2.0f, ceil(Ogre::Math::Log2(static_cast<Ogre::Real>(val)))));
}

Ogre::DataStreamPtr getReadableFile(const std::string& dataDir, const std::string& fileName)
{
    Ogre::DataStreamPtr stream;

    if(dataDir.empty())
    {
        try{
            stream = Ogre::ResourceGroupManager::getSingleton().openResource( fileName, "PF" );
        }catch(...){}
    }
    else//android
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[getReadableFile]: Load from directory " + Ogre::String(dataDir.c_str()));

        std::ios::openmode mode = std::ios::in | std::ios::binary;
        std::ifstream* roStream = 0;
        roStream = OGRE_NEW_T(std::ifstream, Ogre::MEMCATEGORY_GENERAL)();
        roStream->open((dataDir + "/" + PFLoader::mAndroidStorageDir + "/" + fileName).c_str(), mode);

        if(!roStream->fail())
        {
            Ogre::FileStreamDataStream* streamtmp = 0;
            streamtmp = OGRE_NEW Ogre::FileStreamDataStream(fileName.c_str(), roStream, true);
            stream = Ogre::DataStreamPtr(streamtmp);
        }
    }

    return stream;
}

Ogre::DataStreamPtr getWritibleFile(const std::string& dataDir, const std::string& fileName)
{
    Ogre::DataStreamPtr stream;
    if(dataDir.empty())
    {
        try{
            stream = Ogre::ResourceGroupManager::getSingleton().createResource( fileName, "PF", true );
        }catch(Ogre::Exception& e)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[getWritibleFile]: CreateResource error: " + Ogre::String(e.what()));
        }
    }
    else //android
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[getWritibleFile]: Save to directory " + Ogre::String(dataDir.c_str()));

        std::ios::openmode mode = std::ios::out | std::ios::binary;
        std::fstream* ioStream = 0;
        ioStream = OGRE_NEW_T(std::fstream, Ogre::MEMCATEGORY_GENERAL)();
        ioStream->open((dataDir + "/" + PFLoader::mAndroidStorageDir + "/" + fileName).c_str(), mode);

        if(!ioStream->fail())
        {
            Ogre::FileStreamDataStream* streamtmp = 0;
            streamtmp = OGRE_NEW Ogre::FileStreamDataStream(fileName.c_str(), ioStream, true);
            stream = Ogre::DataStreamPtr(streamtmp);
        }
    }

    return stream;
}

std::string keyCodeToString(OIS::KeyCode code)
{
    std::string ret = "";

    switch (code)
    {
        case OIS::KC_LEFT:
            ret = "Left";
            break;

        case OIS::KC_RIGHT:
            ret = "Right";
            break;

        case OIS::KC_UP:
            ret = "Up";
            break;

        case OIS::KC_DOWN:
            ret = "Down";
            break;

        case OIS::KC_LCONTROL:
            ret = "Left Control";
            break;

        case OIS::KC_RCONTROL:
            ret = "Right Control";
            break;

        case OIS::KC_LSHIFT:
            ret = "Left Shift";
            break;

        case OIS::KC_RSHIFT:
            ret = "Right Shift";
            break;

        case OIS::KC_LMENU:
            ret = "Left Alt";
            break;

        case OIS::KC_RMENU:
            ret = "Right Alt";
            break;

        case OIS::KC_SPACE:
            ret = "Space";
            break;

        case OIS::KC_F1:
        case OIS::KC_F2:
        case OIS::KC_F3:
        case OIS::KC_F4:
        case OIS::KC_F5:
        case OIS::KC_F6:
        case OIS::KC_F7:
        case OIS::KC_F8:
        case OIS::KC_F9:
            ret = "F" + Conversions::DMToString(static_cast<char>('1' + code - OIS::KC_F1));
            break;

        case OIS::KC_F10:
            ret = "F10";
            break;

        case OIS::KC_F11:
        case OIS::KC_F12:
            ret = "F1" + Conversions::DMToString(static_cast<char>('1' + code - OIS::KC_F11));
            break;

        case OIS::KC_1:
        case OIS::KC_2:
        case OIS::KC_3:
        case OIS::KC_4:
        case OIS::KC_5:
        case OIS::KC_6:
        case OIS::KC_7:
        case OIS::KC_8:
        case OIS::KC_9:
            ret = Conversions::DMToString(static_cast<char>('1' + code - OIS::KC_1));
            break;

        case OIS::KC_0:
            ret = "0";
            break;

        case OIS::KC_Q:
            ret = "Q";
            break;
        case OIS::KC_W:
            ret = "W";
            break;
        case OIS::KC_E:
            ret = "E";
            break;
        case OIS::KC_R:
            ret = "R";
            break;
        case OIS::KC_T:
            ret = "T";
            break;
        case OIS::KC_Y:
            ret = "Y";
            break;
        case OIS::KC_U:
            ret = "U";
            break;
        case OIS::KC_I:
            ret = "I";
            break;
        case OIS::KC_O:
            ret = "O";
            break;
        case OIS::KC_P:
            ret = "P";
            break;
        case OIS::KC_A:
            ret = "A";
            break;
        case OIS::KC_S:
            ret = "S";
            break;
        case OIS::KC_D:
            ret = "D";
            break;
        case OIS::KC_F:
            ret = "F";
            break;
        case OIS::KC_G:
            ret = "G";
            break;
        case OIS::KC_H:
            ret = "H";
            break;
        case OIS::KC_J:
            ret = "J";
            break;
        case OIS::KC_K:
            ret = "K";
            break;
        case OIS::KC_L:
            ret = "L";
            break;
        case OIS::KC_Z:
            ret = "Z";
            break;
        case OIS::KC_X:
            ret = "X";
            break;
        case OIS::KC_C:
            ret = "C";
            break;
        case OIS::KC_V:
            ret = "V";
            break;
        case OIS::KC_B:
            ret = "B";
            break;
        case OIS::KC_N:
            ret = "N";
            break;
        case OIS::KC_M:
            ret = "M";
            break;
    }

    return ret;
}

std::string keyCodeToString(OIS::MouseButtonID id)
{
    std::string ret = "";

    switch (id)
    {
        case OIS::MB_Left:
            ret = "Left Mouse Button";
            break;

        case OIS::MB_Right:
            ret = "Right Mouse Button";
            break;

        case OIS::MB_Middle:
            ret = "Middle Mouse Button";
            break;

        case OIS::MB_Button3:
        case OIS::MB_Button4:
        case OIS::MB_Button5:
        case OIS::MB_Button6:
        case OIS::MB_Button7:
            ret = "Mouse Button " + Conversions::DMToString(static_cast<char>('3' + id - OIS::MB_Button3));
            break;
    }

    return ret;
}