
#ifndef CUSTOMTRAYMANAGER_H
#define CUSTOMTRAYMANAGER_H

#include "OgreInclude.h"
#include "OISInclude.h"
#include "OgreBulletInclude.h"
#include "SdkTrays.h"

class CustomTrayManager : public OgreBites::SdkTrayManager
{
public :

    CustomTrayManager(const Ogre::String& name, Ogre::RenderWindow* window, OgreBites::InputContext inputContext, SdkTrayListener* listener = 0) 
        : OgreBites::SdkTrayManager(name, window, inputContext, listener){}


    virtual ~CustomTrayManager(){}

    void resourceGroupScriptingStarted(const Ogre::String& groupName, size_t scriptCount);

    void scriptParseStarted(const Ogre::String& scriptName, bool& skipThisScript);
    void scriptParseEnded(const Ogre::String& scriptName, bool skipped);

    void resourceGroupLoadStarted(const Ogre::String& groupName, size_t resourceCount);

    void resourceLoadStarted(const Ogre::ResourcePtr& resource);
    void resourceLoadEnded();

private:

    Ogre::Timer mTimer;

    void windowOnTimerUpdater();
};

#endif