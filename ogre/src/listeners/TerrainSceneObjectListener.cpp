
#include "TerrainSceneObjectListener.h"

#include "../tools/Tools.h"
#include "../customs/CustomSceneManager.h"

ExclusionBoxesObjectListener::ExclusionBoxesObjectListener(const std::vector<LightEclusion>& excl)
    : mLightExclusions(excl)
{}

bool ExclusionBoxesObjectListener::isBoxContainsPoint(const Ogre::Vector3& min, const Ogre::Vector3& max, const Ogre::Vector3& point, Ogre::Real margin)
{
    bool res = false;

        if(
            point.x > (min.x + margin) && point.x < (max.x - margin) &&
            point.y > (min.y + margin) && point.y < (max.y - margin) &&
            point.z > (min.z + margin) && point.z < (max.z - margin))
    {
        res = true;
    }

    return res;
}

bool ExclusionBoxesObjectListener::isBoxFalloff(const Ogre::Vector3& min, const Ogre::Vector3& max, const Ogre::Vector3& point, Ogre::Real margin)
{
    bool res = false;

        if(
            point.x <= (min.x + margin) || point.x >= (max.x - margin) ||
            point.y <= (min.y + margin) || point.y >= (max.y - margin) ||
            point.z <= (min.z + margin) || point.z >= (max.z - margin))
    {
        res = true;
    }

    return res;
}

size_t ExclusionBoxesObjectListener::getDE2LightIndex(const std::string& lightName)
{
    size_t res = 0;

    std::map<std::string, size_t>::const_iterator found = mLightNameToDE2Name.find(lightName);
    if(found == mLightNameToDE2Name.end())
    {
        sscanf(lightName.c_str(), "Lamp_%d", &res);
        --res;
        mLightNameToDE2Name.insert(std::make_pair(lightName, res));
    }
    else
    {
        res = (*found).second;
    }

    return res;
}

TerrainSceneObjectListener::TerrainSceneObjectListener(const Ogre::MovableObject *movableObj, 
                                                       Ogre::SceneManager *sceneManager,
                                                       const std::vector<LightEclusion>& excl)
    : ExclusionBoxesObjectListener(excl),
      mMovableObj(movableObj),
      mSceneMgr(sceneManager),
      mParentNode(movableObj->getParentSceneNode()),
      mIsLightListCreated(false)
{
}

const Ogre::LightList* TerrainSceneObjectListener::objectQueryLights(const Ogre::MovableObject *movableObj)
{
    // This listener is for one object only
    if(movableObj != mMovableObj)
        return NULL;


    // If object is attached to scene (it has a parent)
    if (mParentNode) {
        // Make sure we only update if needed
        Ogre::ulong frame = mSceneMgr->_getLightsDirtyCounter();
        if(!mIsLightListCreated)
        {
            const Ogre::Vector3& scl = mParentNode->_getDerivedScale();
            Ogre::Real factor = std::max(std::max(scl.x, scl.y), scl.z);
            Ogre::Real radius = movableObj->getBoundingRadius() * factor;
            Ogre::uint32 lightMask = movableObj->getLightMask();
            Ogre::Vector3 position = mParentNode->_getDerivedPosition();

            Ogre::LightList candidateLights = mSceneMgr->_getLightsAffectingFrustum();
            CustomSceneManager * manager = dynamic_cast<CustomSceneManager *>(mSceneMgr);
            if(manager)
            {
                candidateLights = manager->_getFullOrderedLightList();
            }

            mLightsCache.clear();
            mLightsCache.reserve(candidateLights.size());

            mLights.clear();
            mLights.reserve(candidateLights.size());

            size_t globalLightsCount = 0;

            size_t lightIndexOnlyPoint = 0;

            Ogre::LightList::const_iterator it;
            for (it = candidateLights.begin(); it != candidateLights.end(); ++it)
            {
                Ogre::Light* lt = *it;
                // check whether or not this light is suppose to be taken into consideration for the current light mask set for this operation
                if(!(lt->getLightMask() & lightMask))
                    continue; //skip this light

                // Calc squared distance
                lt->_calcTempSquareDist(position);

                if (lt->getType() == Ogre::Light::LT_DIRECTIONAL)
                {
                    // Always included
                    mLightsCache.push_back(lt);
                }
                else if(lt->getType() == Ogre::Light::LT_SPOTLIGHT)
                {
                    continue;
                }
                else
                {
                    ++lightIndexOnlyPoint;
                    // only add in-range lights
                    Ogre::Real range = lt->getAttenuationRange();
                    Ogre::Real maxDist = range + radius;

                    /*
                    //if exclusion box covered all verts skip light
                    if(
                        //lt->tempSquareDist > Ogre::Math::Sqr(maxDist) &&
                        isMeshFullyCoveredByBox(lightIndexOnlyPoint - 1) &&
                        lt->getAttenuationRange() != 20000.0f
                    )
                    {
                        continue;
                    }*/
#if 0
                    OverlapStatus status = exclusionBoxAndMeshRelation(lightIndexOnlyPoint - 1);
                    if(status == FULLY_OVERLAP)
                    {
                        continue;
                    }

                    if(status == PARTLY_OVERLAP)
                    {
                        adjustVertexColors(lightIndexOnlyPoint - 1);
                    }

                    //if(checkSphereAndTriangleIntersection(lt->getPosition(), lt->getAttenuationRange()))
                    if(checkSphereAndTriangleIntersectionBullet(lt->getPosition(), lt->getAttenuationRange()))
                    {
                        //d.polubotko: make sure global lights at top of list
                        if(lt->getAttenuationRange() == 20000.0f)
                        {
                            mLightsCache.push_back(lt);
                            std::swap(mLightsCache[globalLightsCount], mLightsCache[mLightsCache.size() - 1]);
                            ++globalLightsCount;
                        }
                        else
                        {
#endif 
                            mLightsCache.push_back(lt);
                            mLights.push_back(lt);
#if 0
                        }
                    }
#endif
                }
            }

            doCPUVertexColor();

            //if(mLightsCache.size() > 20) assert(false && "TerrainSceneObjectListener: too much lights in list");
            //const Ogre::Entity * ent = dynamic_cast<const Ogre::Entity *>(mMovableObj);
            //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TerrainSceneObjectListener::objectQueryLights]: light cache size " + Ogre::StringConverter::toString(mLightsCache.size()) + " " + ent->getMesh()->getName());

#if 0
            for (Ogre::LightList::iterator li = mLightsCache.begin(); li != mLightsCache.end(); ++li)
            {
                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TerrainSceneObjectListener::objectQueryLights]: " + (*li)->getName());
            }
#endif

            // Sort (stable to guarantee ordering on directional lights)
            if (mSceneMgr->isShadowTechniqueTextureBased())
            {
                // Note that if we're using texture shadows, we actually want to use
                // the first few lights unchanged from the frustum list, matching the
                // texture shadows that were generated
                // Thus we only allow object-relative sorting on the remainder of the list
                if (mLightsCache.size() > mSceneMgr->getShadowTextureCount())
                {
                    Ogre::LightList::iterator start = mLightsCache.begin();
                    std::advance(start, mSceneMgr->getShadowTextureCount() + globalLightsCount);
                    std::stable_sort(start, mLightsCache.end(), Ogre::SceneManager::lightLess());
                }
            }
            else
            {
                Ogre::LightList::iterator start = mLightsCache.begin();
                std::advance(start, globalLightsCount);
                std::stable_sort(start, mLightsCache.end(), Ogre::SceneManager::lightLess());
            }

            // Now assign indexes in the list so they can be examined if needed
            size_t lightIndex = 0;
            for (Ogre::LightList::iterator li = mLightsCache.begin(); li != mLightsCache.end(); ++li, ++lightIndex)
            {
                (*li)->_notifyIndexInFrame(lightIndex);
            }

            mIsLightListCreated = true;
        }
    } else {
        mLightsCache.clear();
    }

    return &mLightsCache;    // Return cached list
}

void TerrainSceneObjectListener::doCPUVertexColor()
{
    const Ogre::Entity * ent = dynamic_cast<const Ogre::Entity *>(mMovableObj);

    Ogre::Vector3 derivedPos = mParentNode->_getDerivedPosition();

    Ogre::Mesh* mesh = ent->getMesh().get();

    //d.polubotko: assume used shared buffer
    Ogre::VertexData* vertex_data = mesh->sharedVertexData;
    const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
    Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
    unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));

    size_t countInBox = 0;

    for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
    {

        Ogre::Vector3 pt;
        Ogre::Vector3 normal;
        Ogre::RGBA* pRGBA = 0;

        Ogre::VertexDeclaration::VertexElementList elems = vertex_data->vertexDeclaration->findElementsBySource(posElem->getSource());
        Ogre::VertexDeclaration::VertexElementList::iterator i, iend;
        for (i = elems.begin(); i != elems.end(); ++i)
        {
            Ogre::VertexElement& elem = *i;

            if (elem.getSemantic() == Ogre::VES_POSITION)
            {
                float* pReal;
                elem.baseVertexPointerToElement(vertex, &pReal);

                pt.x = (*pReal++);
                pt.y = (*pReal++);
                pt.z = (*pReal++);
            }

            if (elem.getSemantic() == Ogre::VES_NORMAL)
            {
                float* pReal;
                elem.baseVertexPointerToElement(vertex, &pReal);

                normal.x = (*pReal++);
                normal.y = (*pReal++);
                normal.z = (*pReal++);
            }

            if (elem.getSemantic() == Ogre::VES_DIFFUSE)
            {
                elem.baseVertexPointerToElement(vertex, &pRGBA);
            }
        }

        Ogre::ColourValue diffuse(0.0f, 0.0f, 0.0f, 1.0f);

        float finalAttenuation = 1.0f;
        bool isChangeFinalAttenuation = true;

        Ogre::Vector3 ptGlobal = pt + derivedPos;

        //light iteration
        for (size_t lightIndex = 0; lightIndex < mLights.size(); ++lightIndex)
        {
            Ogre::Light* curLight = mLights[lightIndex];

            Ogre::Vector3 lightPosition = curLight->getPosition();
            Ogre::Real lightOutRange = curLight->getAttenuationRange();
            Ogre::Real distance = lightPosition.distance(ptGlobal);

            if(distance < lightOutRange)
            {

                size_t de2LightIndex = getDE2LightIndex(curLight->getName());

                Ogre::Real lightInRange = curLight->getAttenuationConstant();
                Ogre::ColourValue lightColor = curLight->getDiffuseColour();

                Ogre::Vector3 L = (lightPosition - ptGlobal) / lightOutRange;

                Ogre::Vector3 lightDir = L.normalisedCopy();
                float NdotL = std::max(normal.dotProduct(lightDir), 0.0f);

                
                float attenuation = std::max(0.0f, 1.0f - L.dotProduct(L));
                float attenuationMul = Tools::clamp((lightOutRange - distance) / (lightOutRange - lightInRange), 0.0f, 1.0f);
                attenuation *= attenuationMul;

                Ogre::ColourValue lightAffect = lightColor * attenuation;

                if(!mLightExclusions[de2LightIndex].exclusions.empty())
                {
                    LightEclusion excl = mLightExclusions[de2LightIndex];

                    bool isInAnyBox = false;
                    Ogre::Real finalBright = 1.0f;

                    //iterate over boxes for current light
                    for(size_t q = 0; q < excl.exclusions.size(); ++q)
                    {
                        Ogre::Vector3 ptTransformed = excl.exclusions[q].ogreInverseTransform * ptGlobal;
                        bool insideOutBox = isBoxContainsPoint(excl.exclusions[q].min, excl.exclusions[q].max, ptTransformed);
                        bool insideFalloff = isBoxFalloff(excl.exclusions[q].min, excl.exclusions[q].max, ptTransformed, excl.exclusions[q].falloff);

                            if(insideOutBox)
                            {
                                isInAnyBox = true;

                                if(insideFalloff)
                                {
                                    //calculate attenuation for shadow
                                    float minDist = 0.0f;


#if 0
                                    float distX = excl.exclusions[q].max.x - Ogre::Math::Abs(ptTransformed.x);
                                    float distY = excl.exclusions[q].max.y - Ogre::Math::Abs(ptTransformed.y);
                                    float distZ = excl.exclusions[q].max.z - Ogre::Math::Abs(ptTransformed.z);

                                    minDist = std::min(std::min(distX, distY), distZ);
#endif

#if 1
                                    float distX = 0.0f;
                                    float distY = 0.0f;
                                    float distZ = 0.0f;

                                    if(ptTransformed.x > 0.0f)
                                        distX = excl.exclusions[q].max.x - ptTransformed.x;
                                    else
                                        distX = ptTransformed.x - excl.exclusions[q].min.x;

                                    if(ptTransformed.y >= (excl.exclusions[q].max.y  * 0.5f))
                                        distY = excl.exclusions[q].max.y - ptTransformed.y;
                                    else
                                        distY = ptTransformed.y;

                                    if(ptTransformed.z > 0.0f)
                                        distZ = excl.exclusions[q].max.z - ptTransformed.z;
                                    else
                                        distZ = ptTransformed.z - excl.exclusions[q].min.z;



                                    minDist = std::min(std::min(distX, distY), distZ);
#endif

                                    float shadowAttenuation =   (1.0f - minDist * excl.exclusions[q].reciprocalFalloff) *
                                                                (1.0f - excl.exclusions[q].minExclusion) +
                                                                excl.exclusions[q].minExclusion;
                                    //diffuse += (lightAffect * (1.0f - shadowAttenuation));
                                    //diffuse += Ogre::ColourValue(1.0f, 0.0f, 0.0f);
                                    if(finalBright > shadowAttenuation)
                                    {
                                        finalBright = shadowAttenuation;
                                    }
                                }
                                else
                                {
                                    //use ambient color only
                                    //diffuse += Ogre::ColourValue(0.0f, 1.0f, 0.0f);
                                    if(finalBright > excl.exclusions[q].minExclusion)
                                    {
                                        finalBright = excl.exclusions[q].minExclusion;
                                    }
                                }
                            }
                    }//all exclusions

                    if(isInAnyBox)
                    {
                        if(finalBright < finalAttenuation && isChangeFinalAttenuation)
                            finalAttenuation = finalBright;
                    }
                    else//not in box but light affects
                    {
                        finalAttenuation = 1.0f;
                        isChangeFinalAttenuation = false;
                    }
                }//exclusions non empty
                else//no exclusions but light affect
                {
                    finalAttenuation = 1.0f;
                    isChangeFinalAttenuation = false;
                }

                diffuse += (lightAffect * NdotL);

            }//light affect point
        }//all lights

        finalAttenuation = Ogre::Math::Clamp(finalAttenuation, 0.2f, 1.0f);
        diffuse = diffuse * Ogre::ColourValue(finalAttenuation, finalAttenuation, finalAttenuation);

        //color normalization

        if(diffuse.r > 1.0f || diffuse.g > 1.0f || diffuse.b > 1.0f)
        {
            Ogre::Vector3 normColor(diffuse.r, diffuse.g, diffuse.b);
            normColor.normalise();
            diffuse.r = normColor.x;
            diffuse.g = normColor.y;
            diffuse.b = normColor.z;
        }

        (*pRGBA) = diffuse.getAsABGR();
    }

    vbuf->unlock();
}

OverlapStatus TerrainSceneObjectListener::exclusionBoxAndMeshRelation(size_t lightIndex)
{
    OverlapStatus res = DONT_OVERLAP;

    const Ogre::Entity * ent = dynamic_cast<const Ogre::Entity *>(mMovableObj);

    if(lightIndex < mLightExclusions.size() && lightIndex >= 0 && ent)
    {
        if(!mLightExclusions[lightIndex].exclusions.empty())
        {
            Ogre::Vector3 derivedPos = mParentNode->_getDerivedPosition();

            LightEclusion excl = mLightExclusions[lightIndex];

            Ogre::Mesh* mesh = ent->getMesh().get();

            //d.polubotko: assume used shared buffer
            Ogre::VertexData* vertex_data = mesh->sharedVertexData;
            const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            size_t countInBox = 0;

            for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {

                Ogre::Vector3 pt;

                float* pReal;
                posElem->baseVertexPointerToElement(vertex, &pReal);

                pt.x = (*pReal++);
                pt.y = (*pReal++);
                pt.z = (*pReal++);

                bool isPointInSomeBox = false;

                //iterate over boxes for current light
                for(size_t q = 0; q < excl.exclusions.size(); ++q)
                {

                    Ogre::Vector3 ptGlobal = pt + derivedPos;
                    Ogre::Vector3 ptTransformed = excl.exclusions[q].ogreInverseTransform * ptGlobal;

                    if(isBoxContainsPoint(excl.exclusions[q].min, excl.exclusions[q].max, ptTransformed))
                    {
                        ++countInBox;
                        break;
                    }
                }
            }

            vbuf->unlock();

            if(vertex_data->vertexCount != 0)
            {
                if(vertex_data->vertexCount == countInBox)
                {
                    res = FULLY_OVERLAP;
                }
                else if(countInBox == 0)
                {
                    res = DONT_OVERLAP;
                }
                else
                {
                    res = PARTLY_OVERLAP;
                }
            }
        }
    }


    return res;
}

OverlapStatus TerrainSceneObjectListener::lightSphereAndMeshRelation(const Ogre::Vector3& lightPosition, Ogre::Real lightRange)
{
    OverlapStatus res = DONT_OVERLAP;

    const Ogre::Entity * ent = dynamic_cast<const Ogre::Entity *>(mMovableObj);

    Ogre::Vector3 derivedPos = mParentNode->_getDerivedPosition();

    Ogre::Mesh* mesh = ent->getMesh().get();

    //d.polubotko: assume used shared buffer
    Ogre::VertexData* vertex_data = mesh->sharedVertexData;
    const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
    Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
    unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

    size_t countInSphere = 0;

    for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
    {

        Ogre::Vector3 pt;

        float* pReal;
        posElem->baseVertexPointerToElement(vertex, &pReal);

        pt.x = (*pReal++);
        pt.y = (*pReal++);
        pt.z = (*pReal++);

        Ogre::Vector3 ptGlobal = pt + derivedPos;

        Ogre::Real sqrDistance = lightPosition.squaredDistance(ptGlobal);

        if(sqrDistance < Ogre::Math::Sqr(lightRange))
        {
            ++countInSphere;
        }

    }

    vbuf->unlock();

    if(vertex_data->vertexCount != 0)
    {
        if(vertex_data->vertexCount == countInSphere)
        {
            res = FULLY_OVERLAP;
        }
        else if(countInSphere == 0)
        {
            res = DONT_OVERLAP;
        }
        else
        {
            res = PARTLY_OVERLAP;
        }
    }

    return res;
}

bool TerrainSceneObjectListener::isMeshFullyCoveredByBox(size_t lightIndex)
{
    const Ogre::Entity * ent = dynamic_cast<const Ogre::Entity *>(mMovableObj);

    bool isMeshCoveredByBoxes = true;

    if(lightIndex < mLightExclusions.size() && lightIndex >= 0 && ent)
    {
        if(!mLightExclusions[lightIndex].exclusions.empty())
        {
            Ogre::Vector3 derivedPos = mParentNode->_getDerivedPosition();

            LightEclusion excl = mLightExclusions[lightIndex];

            Ogre::Mesh* mesh = ent->getMesh().get();

            //d.polubotko: assume used shared buffer
            Ogre::VertexData* vertex_data = mesh->sharedVertexData;
            const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {

                Ogre::Vector3 pt;

                float* pReal;
                posElem->baseVertexPointerToElement(vertex, &pReal);

                pt.x = (*pReal++);
                pt.y = (*pReal++);
                pt.z = (*pReal++);

                bool isPointInSomeBox = false;

                //iterate over boxes for current light
                for(size_t q = 0; q < excl.exclusions.size(); ++q)
                {

                    Ogre::Vector3 ptGlobal = pt + derivedPos;
                    Ogre::Vector3 ptTransformed = excl.exclusions[q].ogreInverseTransform * ptGlobal;

                    if(isBoxContainsPoint(excl.exclusions[q].min, excl.exclusions[q].max, ptTransformed))
                    {
                        isPointInSomeBox = true;
                        break;
                    }
                }

                if(!isPointInSomeBox)
                {
                    isMeshCoveredByBoxes = false;
                }
            }

            vbuf->unlock();
        }
        else
        {
            isMeshCoveredByBoxes = false;
        }
    }
    else
    {
        isMeshCoveredByBoxes = false;
    }

    return isMeshCoveredByBoxes;
}

void TerrainSceneObjectListener::adjustVertexColors(size_t lightIndex)
{
    const Ogre::Entity * ent = dynamic_cast<const Ogre::Entity *>(mMovableObj);

    if(lightIndex < mLightExclusions.size() && lightIndex >= 0 && ent)
    {
        if(!mLightExclusions[lightIndex].exclusions.empty())
        {
            Ogre::Vector3 derivedPos = mParentNode->_getDerivedPosition();

            LightEclusion excl = mLightExclusions[lightIndex];

            Ogre::Mesh* mesh = ent->getMesh().get();

            //d.polubotko: assume used shared buffer
            Ogre::VertexData* vertex_data = mesh->sharedVertexData;
            const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));

            for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {

                Ogre::Vector3 pt;
                Ogre::RGBA* pRGBA = 0;

                Ogre::VertexDeclaration::VertexElementList elems = vertex_data->vertexDeclaration->findElementsBySource(posElem->getSource());
                Ogre::VertexDeclaration::VertexElementList::iterator i, iend;
                for (i = elems.begin(); i != elems.end(); ++i)
                {
                    Ogre::VertexElement& elem = *i;

                    if (elem.getSemantic() == Ogre::VES_POSITION)
                    {
                        float* pReal;
                        elem.baseVertexPointerToElement(vertex, &pReal);

                        pt.x = (*pReal++);
                        pt.y = (*pReal++);
                        pt.z = (*pReal++);
                    }

                    if (elem.getSemantic() == Ogre::VES_DIFFUSE)
                    {
                        elem.baseVertexPointerToElement(vertex, &pRGBA);
                    }
                }

                //iterate over boxes for current light
                for(size_t q = 0; q < excl.exclusions.size(); ++q)
                {

                    Ogre::Vector3 ptGlobal = pt + derivedPos;
                    Ogre::Vector3 ptTransformed = excl.exclusions[q].ogreInverseTransform * ptGlobal;

                    if(isBoxContainsPoint(excl.exclusions[q].min, excl.exclusions[q].max, ptTransformed) && pRGBA)
                    {
                        float minDist = 0.0f;

                        float distX = excl.exclusions[q].max.x - Ogre::Math::Abs(ptTransformed.x);
                        float distY = excl.exclusions[q].max.y - Ogre::Math::Abs(ptTransformed.y);
                        float distZ = excl.exclusions[q].max.z - Ogre::Math::Abs(ptTransformed.z);

                        minDist = std::min(std::min(distX, distY), distZ);

                        if(minDist > excl.exclusions[q].falloff)
                        {
                            Ogre::ColourValue colour;
                            colour.setAsRGBA(*pRGBA);

                            if(excl.exclusions[q].minExclusion < colour.r)
                            {
                                colour.r = excl.exclusions[q].minExclusion;
                                colour.g = excl.exclusions[q].minExclusion;
                                colour.b = excl.exclusions[q].minExclusion;
                                colour.a = excl.exclusions[q].minExclusion;
#if 0
                                colour.r = 0.0f;
                                colour.g = 1.0f;
                                colour.b = 0.0f;
                                colour.a = 0.0f;
#endif
                                (*pRGBA) = colour.getAsABGR();
                            }
                        }
                        else
                        {
                            float bright =  (1.0f - minDist * excl.exclusions[q].reciprocalFalloff) *
                                            (1.0f - excl.exclusions[q].minExclusion) +
                                            excl.exclusions[q].minExclusion;

                            Ogre::ColourValue colour;
                            colour.setAsRGBA(*pRGBA);

                            if(bright < colour.r)
                            {
                                colour.r = bright;
                                colour.g = bright;
                                colour.b = bright;
                                colour.a = bright;
#if 0
                                colour.r = 1.0f;
                                colour.g = 0.0f;
                                colour.b = 0.0f;
                                colour.a = 1.0f;
#endif
                                (*pRGBA) = colour.getAsABGR();
                            }
                        }
                    }
                }
            }

            vbuf->unlock();
        }
    }
}

bool TerrainSceneObjectListener::checkSphereAndTriangleIntersection(const Ogre::Vector3& sphereCenter, Ogre::Real radius)
{
    bool res = false;
    const Ogre::Entity * ent = dynamic_cast<const Ogre::Entity *>(mMovableObj);
    if(ent)
    {
        Ogre::Mesh* mesh = ent->getMesh().get();
        for(size_t i = 0; i < mesh->getNumSubMeshes(); i++)
        {
            Ogre::SubMesh* submesh = mesh->getSubMesh(i);

            Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
            const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            Ogre::IndexData* index_data = submesh->indexData;
            size_t numTris = index_data->indexCount / 3;
            unsigned short* pShort;
            unsigned int* pInt;
            Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
            bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
            if (use32bitindexes) pInt = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
            else pShort = static_cast<unsigned short*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            for(size_t k = 0; k < numTris; ++k)
            {

                unsigned int vindex = use32bitindexes? *pInt++ : *pShort++;
                unsigned char* vertexOffsetA = vertex + vbuf->getVertexSize() * vindex;

                vindex = use32bitindexes? *pInt++ : *pShort++;
                unsigned char* vertexOffsetB = vertex + vbuf->getVertexSize() * vindex;

                vindex = use32bitindexes? *pInt++ : *pShort++;
                unsigned char* vertexOffsetC = vertex + vbuf->getVertexSize() * vindex;

                Ogre::Real* pRealA, * pRealB, * pRealC;
                posElem->baseVertexPointerToElement(vertexOffsetA, &pRealA);
                posElem->baseVertexPointerToElement(vertexOffsetB, &pRealB);
                posElem->baseVertexPointerToElement(vertexOffsetC, &pRealC);

                Ogre::Vector3 ptA;
                ptA.x = (*pRealA++);
                ptA.y = (*pRealA++);
                ptA.z = (*pRealA++);

                Ogre::Vector3 ptB;
                ptB.x = (*pRealB++);
                ptB.y = (*pRealB++);
                ptB.z = (*pRealB++);

                Ogre::Vector3 ptC;
                ptC.x = (*pRealC++);
                ptC.y = (*pRealC++);
                ptC.z = (*pRealC++);

                OgreTriangle tri;
                tri.A = ptA;
                tri.B = ptB;
                tri.C = ptC;

                res = Tools::CheckSphereIntersectionWithTriangle(sphereCenter, radius, tri);

                if(res) break;
            }

            

            


            ibuf->unlock();
            vbuf->unlock();

            if(res) break;
        }

    }

    return res;
}