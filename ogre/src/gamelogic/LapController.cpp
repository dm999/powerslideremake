#include "../pcheader.h"

#include "LapController.h"

#include "../pscar/PSBaseCar.h"

namespace
{
    struct LapControllerData
    {
        size_t plainIndex;
        size_t currentLap;
        Ogre::Real lapPosition;
    };

    bool sortByPosition(const LapControllerData& a, const LapControllerData& b)
    {
        return a.lapPosition > b.lapPosition;
    }
}

LapController::LapController()
{
}

void LapController::addCar(PSBaseCar * car)
{
    mCars.push_back(car);

    mLapPositions.push_back(0);
}

void LapController::removeCar(PSBaseCar * car)
{
    size_t foundIndex = 0;
    bool found = false;
    for(size_t q = 0; q < mCars.size(); ++q)
    {
        if(mCars[q] == car)
        {
            found = true;
            foundIndex = q;
            break;
        }
    }

    if(found)
    {
        mCars.erase(mCars.begin() + foundIndex);
        mLapPositions.erase(mLapPositions.begin() + foundIndex);
    }
}

void LapController::calculateLapPositions()
{
    typedef std::set<size_t, std::greater<size_t> > dictionaryH;
    dictionaryH dictionaryHelper;

    typedef std::multimap<size_t, LapControllerData> dictionary;
    dictionary dictionaryData;

    for(size_t q = 0; q < mLapPositions.size(); ++q)
    {
        LapControllerData data;
        data.plainIndex = q;
        data.currentLap = mCars[q]->getCurrentLap();
        data.lapPosition = mCars[q]->getLapPosition();

        dictionaryHelper.insert(data.currentLap);
        dictionaryData.insert(std::make_pair(data.currentLap, data));
    }

    size_t globalIndex = 1;

    for(dictionaryH::iterator i = dictionaryHelper.begin(), j = dictionaryHelper.end();
        i != j; ++i)
    {
        std::pair<dictionary::iterator, dictionary::iterator> sameVals;
        sameVals = dictionaryData.equal_range(*i);
        dictionary::iterator ii(sameVals.first), jj(sameVals.second);

        std::vector<LapControllerData> lapData;

        for(;ii != jj; ++ii)
        {
            lapData.push_back((*ii).second);
        }

        std::sort(lapData.begin(), lapData.end(), sortByPosition);

        for(size_t q = 0; q < lapData.size(); ++q, ++globalIndex)
            mLapPositions[lapData[q].plainIndex] = globalIndex;
    }

}

size_t LapController::getTotalPosition(size_t index)const
{
    assert(index < mLapPositions.size());

    return mLapPositions[index];
}