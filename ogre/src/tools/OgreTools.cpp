#include "../pcheader.h"

#include "OgreTools.h"
#include "Tools.h"

Ogre::MaterialPtr CloneMaterial(const Ogre::String& newMaterialName, const Ogre::String& oldMaterialName, 
                                const std::vector<Ogre::String> texturesNames, 
                                float scale,
                                const Ogre::String& groupName){

    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(oldMaterialName);

    Ogre::MaterialPtr materialNew = material->clone(newMaterialName, true, groupName);

    for(size_t q = 0; q < texturesNames.size(); ++q){
        Ogre::TextureUnitState * state = materialNew->getTechnique(0)->getPass(0)->getTextureUnitState(q);
        state->setTextureName(texturesNames[q]);
        state->setTextureScale(scale, scale);
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
        Ogre::SubMesh * submesh = mesh->getSubMesh(i);
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
        Ogre::SubMesh * submesh = mesh->getSubMesh(i);
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

            Ogre::Vector3 vertex;
            vertex.x = (*pReal);
            ++pReal;
            vertex.y = (*pReal);
            ++pReal;
            vertex.z = (*pReal);

            res += vertex;
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

    const size_t last_slash_idx = fullName.rfind('\\');
    if (std::string::npos != last_slash_idx){
        res = fullName.substr(0, last_slash_idx);
    }else{
        size_t last_slash_idx = fullName.rfind('/');

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

void AdjustSuspension(  Ogre::Entity* cockpit,
                        Ogre::SceneNode * wheels[4],
                        const Ogre::Vector3& chassisOffset,
                        const Ogre::Quaternion& chassisRot,
                        const std::vector<std::vector<size_t> >& suspensionIndices,
                        const std::vector<std::vector<Ogre::Vector3> >& suspensionPointOriginalPos,
                        const Ogre::Vector3& frontLOriginalPos,
                        const Ogre::Vector3& frontROriginalPos,
                        const Ogre::Vector3& backLOriginalPos,
                        const Ogre::Vector3& backROriginalPos)
{
    Ogre::Vector3 posWheelBackR = wheels[0]->getPosition();
    Ogre::Vector3 posWheelBackL = wheels[1]->getPosition();
    Ogre::Vector3 posWheelFrontR = wheels[2]->getPosition();
    Ogre::Vector3 posWheelFrontL = wheels[3]->getPosition();

    Ogre::Quaternion chassisRotInverse = chassisRot.Inverse();

#if 0
    Ogre::Vector3 posDiffBackR = posWheelBackR - (chassisOffset + chassisRot * backROriginalPos);
    Ogre::Vector3 posDiffBackL = posWheelBackL - (chassisOffset + chassisRot * backLOriginalPos);
    Ogre::Vector3 posDiffFrontR = posWheelFrontR - (chassisOffset + chassisRot * frontROriginalPos);
    Ogre::Vector3 posDiffFrontL = posWheelFrontL - (chassisOffset + chassisRot * frontLOriginalPos);
#endif

    Ogre::Vector3 posDiffBackR = chassisRotInverse * (posWheelBackR - chassisOffset) - backROriginalPos;
    Ogre::Vector3 posDiffBackL = chassisRotInverse * (posWheelBackL - chassisOffset) - backLOriginalPos;
    Ogre::Vector3 posDiffFrontR = chassisRotInverse * (posWheelFrontR - chassisOffset) - frontROriginalPos;
    Ogre::Vector3 posDiffFrontL = chassisRotInverse * (posWheelFrontL - chassisOffset) - frontLOriginalPos;

    Ogre::Mesh* mesh = cockpit->getMesh().get();

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
                *pReal++ = suspensionPointOriginalPos[q][w].x + posDiffBackR.x;
                *pReal++ = suspensionPointOriginalPos[q][w].y + posDiffBackR.y;
                *pReal++ = suspensionPointOriginalPos[q][w].z + posDiffBackR.z;
            }
            if(q == 2)
            {
                *pReal++ = suspensionPointOriginalPos[q][w].x + posDiffBackL.x;
                *pReal++ = suspensionPointOriginalPos[q][w].y + posDiffBackL.y;
                *pReal++ = suspensionPointOriginalPos[q][w].z + posDiffBackL.z;
            }
            if(q == 1)
            {
                *pReal++ = suspensionPointOriginalPos[q][w].x + posDiffFrontR.x;
                *pReal++ = suspensionPointOriginalPos[q][w].y + posDiffFrontR.y;
                *pReal++ = suspensionPointOriginalPos[q][w].z + posDiffFrontR.z;
            }
            if(q == 0)
            {
                *pReal++ = suspensionPointOriginalPos[q][w].x + posDiffFrontL.x;
                *pReal++ = suspensionPointOriginalPos[q][w].y + posDiffFrontL.y;
                *pReal++ = suspensionPointOriginalPos[q][w].z + posDiffFrontL.z;
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
        chromaKeyedImg.resize(newSize, newSize);
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
    sprintf(buf, "%02x", static_cast<size_t>(r));
    ret += buf;
    sprintf(buf, "%02x", static_cast<size_t>(g));
    ret += buf;
    sprintf(buf, "%02x", static_cast<size_t>(b));
    ret += buf;

    return ret;
}