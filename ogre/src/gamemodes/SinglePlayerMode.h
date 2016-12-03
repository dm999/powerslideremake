#ifndef SINGLEPLAYERMODE_H
#define SINGLEPLAYERMODE_H

#include "BaseRaceMode.h"

class SinglePlayerMode : public BaseRaceMode
{
public:

    SinglePlayerMode(const ModeContext& modeContext);
    virtual ~SinglePlayerMode(){}
};


#endif