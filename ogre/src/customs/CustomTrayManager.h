
#ifndef CUSTOMTRAYMANAGER_H
#define CUSTOMTRAYMANAGER_H

#include "../includes/OgreInclude.h"
#include "../includes/OISInclude.h"
#include "OgreTrays.h"

class CustomTrayManager : public OgreBites::TrayManager
{
public :

    CustomTrayManager(const Ogre::String& name, Ogre::RenderWindow* window, TrayListener* listener = 0) 
        : OgreBites::TrayManager(name, window, listener){}


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