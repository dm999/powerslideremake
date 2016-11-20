
#include "../pcheader.h"

#include "CustomSceneManager.h"

#include "../includes/CommonIncludes.h"

const Ogre::String CustomSceneManagerFactory::FACTORY_TYPE_NAME = "CustomSceneManager";

CustomSceneManager::CustomSceneManager(const Ogre::String& name) : OctreeSceneManager(name)
{}

Ogre::LightList CustomSceneManager::_getFullOrderedLightList()
{
#if 0
    Ogre::SceneManager::MovableObjectCollection* lights = getMovableObjectCollection(Ogre::LightFactory::FACTORY_TYPE_NAME);

    Ogre::SceneManager::LightInfoList testLightInfos;
    {
        OGRE_LOCK_MUTEX(lights->mutex);

        testLightInfos.reserve(lights->map.size());
        Ogre::SceneManager::MovableObjectIterator it(lights->map.begin(), lights->map.end());

        while(it.hasMoreElements())
        {
            Ogre::Light* l = static_cast<Ogre::Light*>(it.getNext());

            if (l->isVisible())
            {
                Ogre::SceneManager::LightInfo lightInfo;
                lightInfo.light = l;
                lightInfo.type = l->getType();
                lightInfo.lightMask = l->getLightMask();
                lightInfo.range = l->getAttenuationRange();
                lightInfo.position = l->getDerivedPosition();
                testLightInfos.push_back(lightInfo);
            }
        }
    } // release lock on lights collection

    Ogre::LightList totalLightList;
    totalLightList.resize(testLightInfos.size());
    Ogre::SceneManager::LightInfoList::const_iterator i;
    Ogre::LightList::iterator j = totalLightList.begin();

    for (i = testLightInfos.begin(); i != testLightInfos.end(); ++i, ++j)
    {
        *j = i->light;
    }
#endif

    Ogre::LightList totalLightList;
    totalLightList.resize(mOrderedLightList.size());
    Ogre::LightList::iterator j = totalLightList.begin();

    for(size_t q = 0; q < mOrderedLightList.size(); ++q, ++j)
    {
        *j = mOrderedLightList[q];
    }

    return totalLightList;
}

Ogre::Light* CustomSceneManager::createLight(const Ogre::String& name)
{
    Ogre::Light* newLight = Ogre::OctreeSceneManager::createLight(name);
    mOrderedLightList.push_back(newLight);
    return newLight;
}

#if 0
const Ogre::Pass* CustomSceneManager::_setPass(const Ogre::Pass* pass, bool evenIfSuppressed, bool shadowDerivation)
{
    //If using late material resolving, swap now.
    if (isLateMaterialResolving()) 
    {
        Ogre::Technique* lateTech = pass->getParent()->getParent()->getBestTechnique();
        if (lateTech->getNumPasses() > pass->getIndex())
        {
            pass = lateTech->getPass(pass->getIndex());
        }
        //Should we warn or throw an exception if an illegal state was achieved?
    }

    if (!mSuppressRenderStateChanges || evenIfSuppressed)
    {
        if (mIlluminationStage == IRS_RENDER_TO_TEXTURE && shadowDerivation)
        {
            // Derive a special shadow caster pass from this one
            pass = deriveShadowCasterPass(pass);
        }
        else if (mIlluminationStage == IRS_RENDER_RECEIVER_PASS && shadowDerivation)
        {
            pass = deriveShadowReceiverPass(pass);
        }

        // Tell params about current pass
        mAutoParamDataSource->setCurrentPass(pass);

        bool passSurfaceAndLightParams = true;
        bool passFogParams = true;

        if (pass->hasVertexProgram())
        {
            bindGpuProgram(pass->getVertexProgram()->_getBindingDelegate());
            // bind parameters later 
            // does the vertex program want surface and light params passed to rendersystem?
            passSurfaceAndLightParams = pass->getVertexProgram()->getPassSurfaceAndLightStates();
        }
        else
        {
            // Unbind program?
            if (mDestRenderSystem->isGpuProgramBound(Ogre::GPT_VERTEX_PROGRAM))
            {
                mDestRenderSystem->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
            }
            // Set fixed-function vertex parameters
        }

        if (pass->hasGeometryProgram())
        {
            bindGpuProgram(pass->getGeometryProgram()->_getBindingDelegate());
            // bind parameters later 
        }
        else
        {
            // Unbind program?
            if (mDestRenderSystem->isGpuProgramBound(Ogre::GPT_GEOMETRY_PROGRAM))
            {
                mDestRenderSystem->unbindGpuProgram(Ogre::GPT_GEOMETRY_PROGRAM);
            }
            // Set fixed-function vertex parameters
        }

        if (passSurfaceAndLightParams)
        {
            // Set surface reflectance properties, only valid if lighting is enabled
            if (pass->getLightingEnabled())
            {
                mDestRenderSystem->_setSurfaceParams( 
                    pass->getAmbient(), 
                    pass->getDiffuse(), 
                    pass->getSpecular(), 
                    pass->getSelfIllumination(), 
                    pass->getShininess(),
            pass->getVertexColourTracking() );
            }

            // Dynamic lighting enabled?
            mDestRenderSystem->setLightingEnabled(pass->getLightingEnabled());
        }

        // Using a fragment program?
        if (pass->hasFragmentProgram())
        {
            bindGpuProgram(pass->getFragmentProgram()->_getBindingDelegate());
            // bind parameters later 
            passFogParams = pass->getFragmentProgram()->getPassFogStates();
        }
        else
        {
            // Unbind program?
            if (mDestRenderSystem->isGpuProgramBound(Ogre::GPT_FRAGMENT_PROGRAM))
            {
                mDestRenderSystem->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
            }

            // Set fixed-function fragment settings
        }

        if (passFogParams)
        {
            // New fog params can either be from scene or from material
            Ogre::FogMode newFogMode;
            Ogre::ColourValue newFogColour;
            Ogre::Real newFogStart, newFogEnd, newFogDensity;
            if (pass->getFogOverride())
            {
                // New fog params from material
                newFogMode = pass->getFogMode();
                newFogColour = pass->getFogColour();
                newFogStart = pass->getFogStart();
                newFogEnd = pass->getFogEnd();
                newFogDensity = pass->getFogDensity();
            }
            else
            {
                // New fog params from scene
                newFogMode = mFogMode;
                newFogColour = mFogColour;
                newFogStart = mFogStart;
                newFogEnd = mFogEnd;
                newFogDensity = mFogDensity;
            }

            /* In D3D, it applies to shaders prior
            to version vs_3_0 and ps_3_0. And in OGL, it applies to "ARB_fog_XXX" in
            fragment program, and in other ways, them maybe access by gpu program via
            "state.fog.XXX".
            */
            mDestRenderSystem->_setFog(
                newFogMode, newFogColour, newFogDensity, newFogStart, newFogEnd);
        }
        // Tell params about ORIGINAL fog
        // Need to be able to override fixed function fog, but still have
        // original fog parameters available to a shader than chooses to use
        mAutoParamDataSource->setFog(
            mFogMode, mFogColour, mFogDensity, mFogStart, mFogEnd);

        // The rest of the settings are the same no matter whether we use programs or not

        // Set scene blending
        if ( pass->hasSeparateSceneBlending( ) )
        {
            mDestRenderSystem->_setSeparateSceneBlending(
                pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
                pass->getSourceBlendFactorAlpha(), pass->getDestBlendFactorAlpha(),
                pass->getSceneBlendingOperation(), 
                pass->hasSeparateSceneBlendingOperations() ? pass->getSceneBlendingOperation() : pass->getSceneBlendingOperationAlpha() );
        }
        else
        {
            if(pass->hasSeparateSceneBlendingOperations( ) )
            {
                mDestRenderSystem->_setSeparateSceneBlending(
                    pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
                    pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
                    pass->getSceneBlendingOperation(), pass->getSceneBlendingOperationAlpha() );
            }
            else
            {
                mDestRenderSystem->_setSceneBlending(
                    pass->getSourceBlendFactor(), pass->getDestBlendFactor(), pass->getSceneBlendingOperation() );
            }
        }

        // Set point parameters
        mDestRenderSystem->_setPointParameters(
            pass->getPointSize(),
            pass->isPointAttenuationEnabled(), 
            pass->getPointAttenuationConstant(), 
            pass->getPointAttenuationLinear(), 
            pass->getPointAttenuationQuadratic(), 
            pass->getPointMinSize(), 
            pass->getPointMaxSize());

        if (mDestRenderSystem->getCapabilities()->hasCapability(Ogre::RSC_POINT_SPRITES))
            mDestRenderSystem->_setPointSpritesEnabled(pass->getPointSpritesEnabled());

        // Texture unit settings

        Ogre::Pass::ConstTextureUnitStateIterator texIter =  pass->getTextureUnitStateIterator();
        size_t unit = 0;
        // Reset the shadow texture index for each pass
        size_t startLightIndex = pass->getStartLight();
        size_t shadowTexUnitIndex = 0;
        size_t shadowTexIndex = mShadowTextures.size();
        if (mShadowTextureIndexLightList.size() > startLightIndex)
            shadowTexIndex = mShadowTextureIndexLightList[startLightIndex];
        while(texIter.hasMoreElements())
        {
            Ogre::TextureUnitState* pTex = texIter.getNext();
            if (!pass->getIteratePerLight() && 
                isShadowTechniqueTextureBased() && 
                pTex->getContentType() == Ogre::TextureUnitState::CONTENT_SHADOW)
            {
                // Need to bind the correct shadow texture, based on the start light
                // Even though the light list can change per object, our restrictions
                // say that when texture shadows are enabled, the lights up to the
                // number of texture shadows will be fixed for all objects
                // to match the shadow textures that have been generated
                // see Listener::sortLightsAffectingFrustum and
                // MovableObject::Listener::objectQueryLights
                // Note that light iteration throws the indexes out so we don't bind here
                // if that's the case, we have to bind when lights are iterated
                // in renderSingleObject

                Ogre::TexturePtr shadowTex;
                if (shadowTexIndex < mShadowTextures.size())
                {
                    shadowTex = getShadowTexture(shadowTexIndex);
                    // Hook up projection frustum
                    Ogre::Camera *cam = shadowTex->getBuffer()->getRenderTarget()->getViewport(0)->getCamera();
                    // Enable projective texturing if fixed-function, but also need to
                    // disable it explicitly for program pipeline.
                    pTex->setProjectiveTexturing(!pass->hasVertexProgram(), cam);
                    mAutoParamDataSource->setTextureProjector(cam, shadowTexUnitIndex);
                }
                else
                {
                    // Use fallback 'null' shadow texture
                    // no projection since all uniform colour anyway
                    shadowTex = mNullShadowTexture;
                    pTex->setProjectiveTexturing(false);
                    mAutoParamDataSource->setTextureProjector(0, shadowTexUnitIndex);

                }
                pTex->_setTexturePtr(shadowTex);

                ++shadowTexIndex;
                ++shadowTexUnitIndex;
            }
            else if (mIlluminationStage == IRS_NONE && pass->hasVertexProgram())
            {
                // Manually set texture projector for shaders if present
                // This won't get set any other way if using manual projection
                Ogre::TextureUnitState::EffectMap::const_iterator effi = 
                    pTex->getEffects().find(Ogre::TextureUnitState::ET_PROJECTIVE_TEXTURE);
                if (effi != pTex->getEffects().end())
                {
                    mAutoParamDataSource->setTextureProjector(effi->second.frustum, unit);
                }
            }
            if (pTex->getContentType() == Ogre::TextureUnitState::CONTENT_COMPOSITOR)
            {
                Ogre::CompositorChain* currentChain = _getActiveCompositorChain();
                if (!currentChain)
                {
                    OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE,
                        "A pass that wishes to reference a compositor texture "
                        "attempted to render in a pipeline without a compositor",
                        "SceneManager::_setPass");
                }
                Ogre::CompositorInstance* refComp = currentChain->getCompositor(pTex->getReferencedCompositorName());
                if (refComp == 0)
                {
                    OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND,
                        "Invalid compositor content_type compositor name",
                        "SceneManager::_setPass");
                }
                Ogre::TexturePtr refTex = refComp->getTextureInstance(
                    pTex->getReferencedTextureName(), pTex->getReferencedMRTIndex());
                if (refTex.isNull())
                {
                    OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND,
                        "Invalid compositor content_type texture name",
                        "SceneManager::_setPass");
                }
                pTex->_setTexturePtr(refTex);
            }
            mDestRenderSystem->_setTextureUnitSettings(unit, *pTex);
            ++unit;
        }
        // Disable remaining texture units
        mDestRenderSystem->_disableTextureUnitsFrom(pass->getNumTextureUnitStates());

        // Set up non-texture related material settings
        // Depth buffer settings
        mDestRenderSystem->_setDepthBufferFunction(pass->getDepthFunction());
        mDestRenderSystem->_setDepthBufferCheckEnabled(pass->getDepthCheckEnabled());
        mDestRenderSystem->_setDepthBufferWriteEnabled(pass->getDepthWriteEnabled());
        mDestRenderSystem->_setDepthBias(pass->getDepthBiasConstant(), 
            pass->getDepthBiasSlopeScale());
        // Alpha-reject settings
        mDestRenderSystem->_setAlphaRejectSettings(
            pass->getAlphaRejectFunction(), pass->getAlphaRejectValue(), pass->isAlphaToCoverageEnabled());
        // Set colour write mode
        // Right now we only use on/off, not per-channel
        bool colWrite = pass->getColourWriteEnabled();
        mDestRenderSystem->_setColourBufferWriteEnabled(colWrite, colWrite, colWrite, colWrite);
        // Culling mode
        if (isShadowTechniqueTextureBased() 
            && mIlluminationStage == IRS_RENDER_TO_TEXTURE
            && mShadowCasterRenderBackFaces
            && pass->getCullingMode() == Ogre::CULL_CLOCKWISE)
        {
            // render back faces into shadow caster, can help with depth comparison
            mPassCullingMode = Ogre::CULL_ANTICLOCKWISE;
        }
        else
        {
            mPassCullingMode = pass->getCullingMode();
        }
        mDestRenderSystem->_setCullingMode(mPassCullingMode);
        
        // Shading
        mDestRenderSystem->setShadingType(pass->getShadingMode());
        // Polygon mode
        mDestRenderSystem->_setPolygonMode(pass->getPolygonMode());

        // set pass number
        mAutoParamDataSource->setPassNumber( pass->getIndex() );

        // mark global params as dirty
        mGpuParamsDirty |= (Ogre::uint16)Ogre::GPV_GLOBAL;

    }

    return pass;
}

void CustomSceneManager::updateGpuProgramParameters(const Ogre::Pass* pass)
{
    if (pass->isProgrammable())
    {

        if (!mGpuParamsDirty)
            return;

        if (mGpuParamsDirty)
            pass->_updateAutoParams(mAutoParamDataSource, mGpuParamsDirty);

        if (pass->hasVertexProgram())
        {
            mDestRenderSystem->bindGpuProgramParameters(Ogre::GPT_VERTEX_PROGRAM, 
                pass->getVertexProgramParameters(), mGpuParamsDirty);
        }

        if (pass->hasGeometryProgram())
        {
            mDestRenderSystem->bindGpuProgramParameters(Ogre::GPT_GEOMETRY_PROGRAM,
                pass->getGeometryProgramParameters(), mGpuParamsDirty);
        }

        if (pass->hasFragmentProgram())
        {
            mDestRenderSystem->bindGpuProgramParameters(Ogre::GPT_FRAGMENT_PROGRAM, 
                pass->getFragmentProgramParameters(), mGpuParamsDirty);
        }

        mGpuParamsDirty = 0;
    }
}
#endif

Ogre::ParticleSystem* CustomSceneManager::createParticleSystem(const Ogre::String& name, const Ogre::String& templateName)
{
    Ogre::NameValuePairList params;
    params["templateName"] = templateName;
    params["resourceGroup"] = TEMP_RESOURCE_GROUP_NAME;

    return static_cast<Ogre::ParticleSystem*>(createMovableObject(name, Ogre::ParticleSystemFactory::FACTORY_TYPE_NAME, &params));
}