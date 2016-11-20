#ifndef DE2LOADER_H
#define DE2LOADER_H

#include <vector>

#include "../mesh/MshData.h"
#include "DE2.h"

class DE2Loader
{
public:
    DE2Loader(){}

    bool load(std::vector<MSHData>& parts, FILE * fileToLoad);

    const DE2::DE2_File& getDE2() const {return mDE2;}

private:

    DE2::DE2_File mDE2;
};

#endif