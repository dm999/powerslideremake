#include "pcheader.h"

#include "PlayerVehicleSceneObjectListener.h"


PlayerVehicleSceneObjectListener::PlayerVehicleSceneObjectListener(const Ogre::MovableObject *movableObj, 
    Ogre::SceneManager *sceneManager,
    const std::vector<LightEclusion>& excl)
    : ExclusionBoxesObjectListener(excl),
      mMovableObj(movableObj),
      mSceneMgr(sceneManager),
      mParentNode(movableObj->getParentSceneNode()),
      mLightListUpdated(0)
{
}

const Ogre::LightList* PlayerVehicleSceneObjectListener::objectQueryLights(const Ogre::MovableObject *movableObj)
{
    // This listener is for one object only
    if(movableObj != mMovableObj)
        return NULL;


    // If object is attached to scene (it has a parent)
    if (mParentNode) {
        // Make sure we only update if needed
        Ogre::ulong frame = mSceneMgr->_getLightsDirtyCounter();
        if (mLightListUpdated != frame) {
            mLightListUpdated = frame;

            const Ogre::Vector3& scl = mParentNode->_getDerivedScale();
            Ogre::Real factor = std::max(std::max(scl.x, scl.y), scl.z);
            Ogre::Real radius = movableObj->getBoundingRadius() * factor;
            Ogre::uint32 lightMask = movableObj->getLightMask();
            Ogre::Vector3 position = mParentNode->_getDerivedPosition();

            const Ogre::LightList& candidateLights = mSceneMgr->_getLightsAffectingFrustum();

            mLightsCache.clear();
            mLightsCache.reserve(candidateLights.size());

            mLights.clear();
            mLights.reserve(candidateLights.size());

            size_t globalLightsCount = 0;

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
                    // only add in-range lights
                    Ogre::Real range = lt->getAttenuationRange();
                    Ogre::Real maxDist = range + radius;
                    if (lt->tempSquareDist <= Ogre::Math::Sqr(maxDist))
                    {
                        mLights.push_back(lt);

                        //d.polubotko: make sure global lights at top of list
                        if(lt->getAttenuationRange() == 20000.0f)
                        {
                            mLightsCache.push_back(lt);
                            std::swap(mLightsCache[globalLightsCount], mLightsCache[mLightsCache.size() - 1]);
                            ++globalLightsCount;
                        }
                        else
                        {
                            mLightsCache.push_back(lt);
                        }

                    }
                }
            }

            doCPUVertexColor();

            // Sort (stable to guarantee ordering on directional lights)
            if (mSceneMgr->isShadowTechniqueTextureBased())
            {
                // Note that if we're using texture shadows, we actually want to use
                // the first few lights unchanged from the frustum list, matching the
                // texture shadows that were generated
                // Thus we only allow object-relative sorting on the remainder of the list
                if (mLightsCache.size() > mSceneMgr->getShadowTextureCount())
                {
                    //Ogre::LightList::iterator start = mLightsCache.begin();
                    //std::advance(start, mSceneMgr->getShadowTextureCount() + globalLightsCount);
                    //std::stable_sort(start, mLightsCache.end(), Ogre::SceneManager::lightLess());
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
        }
    } else {
        mLightsCache.clear();
    }

    return &mLightsCache;    // Return cached list
}

void PlayerVehicleSceneObjectListener::doCPUVertexColor()
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

        Ogre::ColourValue diffuse(1.0f, 1.0f, 1.0f, 1.0f);

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
                if(lightIndex < mLightExclusions.size() && lightIndex >= 0 && !mLightExclusions[lightIndex].exclusions.empty())
                {
                    LightEclusion excl = mLightExclusions[lightIndex];

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
                    }

                    if(isInAnyBox)
                    {
                        diffuse *= finalBright;
                    }
                }

            }
        }

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
