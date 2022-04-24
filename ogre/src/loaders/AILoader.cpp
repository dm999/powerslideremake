
#include "AILoader.h"

#include "../tools/Conversions.h"
#include "../tools/Randomizer.h"

AILoader::AILoader() : mNameGenNodes("Scene/Node/AI/Name")
{}

void AILoader::load(GameState& gameState, Ogre::SceneManager* sceneMgr, bool isDebugAI) const
{
    std::string folderWithData = "chareasy";

    switch(gameState.getAIStrength())
    {
    case Medium :
        folderWithData = "charmedium";
        break;
    case Hard :
        folderWithData = "charhard";
        break;
    case Insane :
        folderWithData = "charinsane";
        break;
    }

    std::vector<AIWhole> aiWhole;
    for(size_t w = 0; w < GameState::mRaceGridCarsMax; ++w)
    {
        aiWhole.push_back(AIWhole());

        //read SLOT
        {
            Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()) + "/ai/" + folderWithData, "slot" + Conversions::DMToString(w));
            if(fileToLoad.get() && fileToLoad->isReadable())
            {

                Ogre::uint32 someBuf[8];
                fileToLoad->read(someBuf, 4 * 8);

                Ogre::uint32 slotMatrixSize = someBuf[2];


                aiWhole[w].slotMatrix.resize(slotMatrixSize);
                aiWhole[w].activation.resize(5);
                aiWhole[w].remapper.resize(slotMatrixSize);

                for(size_t q = 0; q < slotMatrixSize; ++q)
                {
                    aiWhole[w].slotMatrix[q].resize(39);
                    fileToLoad->read(&aiWhole[w].slotMatrix[q][0], 4 * 39);
                    aiWhole[w].slotMatrix[q][0] += Randomizer::GetInstance().GetRandomFloat(-1.0f, 1.0f);
                }

                for(size_t q = 0; q < 5; ++q)
                {
                    aiWhole[w].activation[q].resize(31);
                    fileToLoad->read(&aiWhole[w].activation[q][0], 4 * 31);
                }

                for(size_t q = 0; q < 2; ++q)
                {
                    float someBufMore[48];
                    fileToLoad->read(someBufMore, 4 * 48);
                }

                for(size_t q = 0; q < 5; ++q)
                {
                    float someBufMore[12];
                    fileToLoad->read(someBufMore, 4 * 12);
                }

                for(size_t q = 0; q < 3; ++q)
                {
                    float someBufMore[3];
                    fileToLoad->read(someBufMore, 4 * 3);
                }

                {
                    float someData;
                    fileToLoad->read(&someData, 4);
                }

                {
                    fileToLoad->read(&aiWhole[w].multiplier, 4 * 3);
                }

                for(size_t q = 0; q < slotMatrixSize; ++q)
                {
                    aiWhole[w].remapper[q].resize(6);
                    fileToLoad->read(&aiWhole[w].remapper[q][0], 4 * 6);
                }


                fileToLoad->close();
            }
        }//read SLOT END

        //read REC
        {
            Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()) + "/ai/" + folderWithData, "rec" + Conversions::DMToString(w));

            if(fileToLoad.get() && fileToLoad->isReadable())
            {
                //gameState.getAICar(w).clearAIData();

                float someBuf[9];
                fileToLoad->read(someBuf, 4 * 9);

                //someBuf[0] == 12345
                aiWhole[w].hackType = *reinterpret_cast<int *>(&someBuf[1]);
                aiWhole[w].accelerationCoeff = someBuf[2];
                aiWhole[w].velocityScale = someBuf[3];
                aiWhole[w].tractionCoeff = someBuf[4];
                aiWhole[w].aiImpulseScale = someBuf[5];
                aiWhole[w].hack1 = someBuf[6];
                aiWhole[w].hack2 = someBuf[7];
                aiWhole[w].hackMultiplier = someBuf[8];

                Ogre::uint32 someData;
                fileToLoad->read(&someData, 4);

                while(someData)
                {
                    Ogre::uint32 someData2;
                    fileToLoad->read(&someData2, 4);

                    for(size_t q = 0; q < someData2; ++q)
                    {
                        AIData aiData;

                        float someBuf2[9];//x, y, z, normx?, normy?, normz?, unknown, unknown, unknown
                        fileToLoad->read(someBuf2, 36);

                        aiData.pos.x = someBuf2[0];
                        aiData.pos.y = someBuf2[1];
                        aiData.pos.z = -someBuf2[2];//original data is left hand

                        aiData.tangent.x = someBuf2[3];
                        aiData.tangent.y = someBuf2[4];
                        aiData.tangent.z = -someBuf2[5];//original data is left hand

                        aiData.magic.x = someBuf2[6];
                        aiData.magic.y = someBuf2[7];
                        aiData.magic.z = -someBuf2[8];//original data is left hand

                        aiWhole[w].aiData.push_back(aiData);
                    }

                    std::reverse(aiWhole[w].aiData.begin() + 1, aiWhole[w].aiData.end());
                    for(size_t q = 0; q < someData2; ++q)
                    {
                        aiWhole[w].aiData[q].tangent = Ogre::Quaternion(Ogre::Degree(180.0f), Ogre::Vector3::UNIT_Y) * aiWhole[w].aiData[q].tangent;
                        //aiWhole[w].aiData[q].magic = Ogre::Quaternion(Ogre::Degree(180.0f), Ogre::Vector3::UNIT_Y) * aiWhole[w].aiData[q].magic;
                    }

                    fileToLoad->read(&someData, 4);
                }

                if(isDebugAI)
                {
                    Ogre::SimpleSpline spline;

                    for(size_t q = 0; q < aiWhole[w].aiData.size(); ++q)
                    {
                        //spheres
                        Ogre::Real scale = 0.1f;
                        Ogre::String debugSphereName = mNameGenNodes.generate();
                        Ogre::Entity * debugSphere = sceneMgr->createEntity(debugSphereName, Ogre::SceneManager::PT_SPHERE);
                        debugSphere->setMaterialName("BaseWhiteNoLighting");
                        Ogre::SceneNode * debugSphereNode = sceneMgr->getRootSceneNode()->createChildSceneNode(debugSphereName);
                        debugSphereNode->attachObject(debugSphere);
                        debugSphereNode->setPosition(aiWhole[w].aiData[q].pos);
                        debugSphereNode->setScale(scale, scale, scale);
                        debugSphere->setCastShadows(false);

                        //dirs
                        Ogre::Real length = 15.0f;
                        Ogre::String debugDirName = mNameGenNodes.generate();

                        Ogre::ManualObject * manual =  sceneMgr->createManualObject(debugDirName); 
                        Ogre::SceneNode* lltDirNode = sceneMgr->getRootSceneNode()->createChildSceneNode(debugDirName); 
                         
                        manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST); 

                        spline.addPoint(aiWhole[w].aiData[q].pos);

                        manual->position(aiWhole[w].aiData[q].pos);
                        manual->position(aiWhole[w].aiData[q].pos + aiWhole[w].aiData[q].tangent * length);

                        manual->position(aiWhole[w].aiData[q].pos);
                        manual->position(aiWhole[w].aiData[q].pos + aiWhole[w].aiData[q].magic * length);

                        manual->end(); 
                        manual->setCastShadows(false);
                        lltDirNode->attachObject(manual);

                    }

                    spline.addPoint(aiWhole[w].aiData[0].pos);

                    //lines
                    Ogre::String debugDirName = mNameGenNodes.generate();
                    Ogre::ManualObject * manual =  sceneMgr->createManualObject(debugDirName); 
                    Ogre::SceneNode* lltDirNode = sceneMgr->getRootSceneNode()->createChildSceneNode(debugDirName); 
                    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST); 
                    for(size_t q = 0; q < aiWhole[w].aiData.size() - 1; ++q)
                    {
#if 1
                        manual->position(aiWhole[w].aiData[q].pos);
                        manual->position(aiWhole[w].aiData[q + 1].pos);
#else
                        for(float w = 0; w < 1.0f; w += 0.1f)
                        {
                            manual->position(spline.interpolate(q, w));
                            manual->position(spline.interpolate(q, w + 0.1f));
                        }
#endif
                    }
#if 1
                    manual->position(aiWhole[w].aiData[aiWhole[w].aiData.size() - 1].pos);
                    manual->position(aiWhole[w].aiData[0].pos);
#else
                    for(float w = 0; w < 1.0f; w += 0.1f)
                    {
                        manual->position(spline.interpolate(aiWhole[w].aiData.size() - 1, w));
                        manual->position(spline.interpolate(aiWhole[w].aiData.size() - 1, w + 0.1f));
                    }
#endif
                    manual->end(); 
                    manual->setCastShadows(false);
                    lltDirNode->attachObject(manual);
                }

                fileToLoad->close();

            }
        }//read REC END

    }

    for(size_t w = 0; w < gameState.getAICountInRace(); ++w)
    {
        size_t slotIndex = gameState.getAICar(w).getSlotIndex();
        gameState.getAICar(w).setAIData(aiWhole[slotIndex], sceneMgr, isDebugAI);
    }
}