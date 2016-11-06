#include "pcheader.h"

#include "VehicleSceneObjectListener.h"


VehicleSceneObjectListener::VehicleSceneObjectListener(const Ogre::MovableObject *movableObj, Ogre::SceneManager *sceneManager)
    : mMovableObj(movableObj),
      mSceneMgr(sceneManager),
      mParentNode(movableObj->getParentSceneNode()),
      mLightListUpdated(0)
{
}

const Ogre::LightList* VehicleSceneObjectListener::objectQueryLights(const Ogre::MovableObject *movableObj)
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
