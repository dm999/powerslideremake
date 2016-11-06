
#ifndef CUSTOMSCENEMANAGER_H
#define CUSTOMSCENEMANAGER_H

#include "OgreInclude.h"

class CustomSceneManager : public Ogre::OctreeSceneManager
{
private:
    std::vector<Ogre::Light *> mOrderedLightList;
public:
    CustomSceneManager(const Ogre::String& name);

    virtual ~CustomSceneManager(){}

    Ogre::LightList _getFullOrderedLightList();

    virtual Ogre::Light* createLight(const Ogre::String& name);

    virtual Ogre::ParticleSystem* createParticleSystem(const Ogre::String& name, const Ogre::String& templateName);

#if 0
    virtual const Ogre::Pass* _setPass(const Ogre::Pass* pass, bool evenIfSuppressed = false, bool shadowDerivation = true);
    void updateGpuProgramParameters(const Ogre::Pass* pass);
#endif
};

class CustomSceneManagerFactory : public Ogre::SceneManagerFactory
{
protected:

    void initMetaData(void) const
    {
        mMetaData.typeName = FACTORY_TYPE_NAME;
        mMetaData.description = "Custom scene manager";
        mMetaData.sceneTypeMask = 0xFFFF; // support all types
        mMetaData.worldGeometrySupported = false;
    }

public:
    CustomSceneManagerFactory() {}
    virtual ~CustomSceneManagerFactory() {}
    /// Factory type name
    static const Ogre::String FACTORY_TYPE_NAME;

    Ogre::SceneManager* createInstance(const Ogre::String& instanceName)
    {
        return OGRE_NEW CustomSceneManager(instanceName);
    }

    void destroyInstance(Ogre::SceneManager* instance)
    {
        OGRE_DELETE instance;
    }
};

#endif