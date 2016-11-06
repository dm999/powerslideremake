#include "pcheader.h"

#include "CustomTrayManager.h"

void CustomTrayManager::resourceGroupScriptingStarted(const Ogre::String& groupName, size_t scriptCount)
{
    mLoadInc = mGroupInitProportion / scriptCount;
    mLoadBar->setCaption("Parsing...");

    windowOnTimerUpdater();
}

void CustomTrayManager::scriptParseStarted(const Ogre::String& scriptName, bool& skipThisScript)
{
    mLoadBar->setComment(scriptName);

    windowOnTimerUpdater();
}

void CustomTrayManager::scriptParseEnded(const Ogre::String& scriptName, bool skipped)
{
    mLoadBar->setProgress(mLoadBar->getProgress() + mLoadInc);

    windowOnTimerUpdater();
}

void CustomTrayManager::resourceGroupLoadStarted(const Ogre::String& groupName, size_t resourceCount)
{
    mLoadInc = mGroupLoadProportion / resourceCount;
    mLoadBar->setCaption("Loading...");

    windowOnTimerUpdater();
}

void CustomTrayManager::resourceLoadStarted(const Ogre::ResourcePtr& resource)
{
    mLoadBar->setComment(resource->getName());

    windowOnTimerUpdater();
}

void CustomTrayManager::resourceLoadEnded()
{
    mLoadBar->setProgress(mLoadBar->getProgress() + mLoadInc);

    windowOnTimerUpdater();
}

void CustomTrayManager::windowOnTimerUpdater()
{
    const unsigned long timePeriodToRefresh = 5; // ms

    if(mTimer.getMilliseconds() > timePeriodToRefresh)
    {
        windowUpdate();
        mTimer.reset();
    }
}