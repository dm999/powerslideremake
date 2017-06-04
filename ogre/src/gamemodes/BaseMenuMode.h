#ifndef BASEMENUMODE_H
#define BASEMENUMODE_H

#include "BaseMode.h"

class LoaderListener;

class BaseMenuMode : public BaseMode
{
public:

    BaseMenuMode(const ModeContext& modeContext, bool isInitialCreation);
    virtual ~BaseMenuMode(){}

    void initData(LoaderListener* loaderListener)override;
    void initCamera()override;
    void clearData()override;

    void frameRenderingQueued(const Ogre::FrameEvent& evt)override;

    void recalculateCharacterNames(const std::vector<std::string>& playersCharacters);

protected:

    virtual void doInitData(LoaderListener* loaderListener) = 0;
    virtual void doClearData() = 0;

    virtual void customFrameRenderingQueued(){}
};

#endif