#ifndef TERRAINDATA_H
#define TERRAINDATA_H

struct TerrainData
{
    static const int mTerrainsAmount = 16;

    char mVelocityIndex;
    char mUnknownIndex;
    char mSoundIndex;
    char mSoundIndex2;
    char mSoundIndex3;
    float mRollResistance;
    float mLongtitudinalGripMultiplier;
    float mLatitudinalGripMultiplier;
    float mCombinedMultiplier;
    float mParameter;
    float mParameter2;
};

#endif
