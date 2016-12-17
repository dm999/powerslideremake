#ifndef BASEMENUMODE_H
#define BASEMENUMODE_H

#include "BaseMode.h"

class BaseMenuMode : public BaseMode
{
public:

    BaseMenuMode(const ModeContext& modeContext);
    virtual ~BaseMenuMode(){}
};

#endif