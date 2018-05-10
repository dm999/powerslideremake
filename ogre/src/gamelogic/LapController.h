
#ifndef LAPCONTROLLER
#define LAPCONTROLLER

#include "../includes/OgreInclude.h"

class PSBaseCar;

class LapController
{
public:

    LapController();
    ~LapController(){}

    void addCar(PSBaseCar * car);
    void removeCar(PSBaseCar * car);

    size_t getTotalCars() const {return mCars.size();}

    void calculateLapPositions();

    size_t getTotalPosition(size_t index)const;//0 - player

    void clear()
    {
        mLapPositions.clear();
        mCars.clear();
    }

private:

    std::vector<size_t> mLapPositions;
    std::vector<PSBaseCar *> mCars;//player is first
};

#endif