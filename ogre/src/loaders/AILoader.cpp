
#include "AILoader.h"

#include "../tools/Conversions.h"

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

        typedef unsigned int DWORD;

        //read SLOT
        {
            Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()) + "/ai/" + folderWithData, "slot" + Conversions::DMToString(w));
            if(fileToLoad.get() && fileToLoad->isReadable())
            {

                DWORD someBuf[8];
                fileToLoad->read(someBuf, 4 * 8);

                DWORD slotMatrixSize = someBuf[2];


                aiWhole[w].slotMatrix.resize(slotMatrixSize);
                aiWhole[w].activation.resize(5);
                aiWhole[w].remapper.resize(slotMatrixSize);

                for(size_t q = 0; q < slotMatrixSize; ++q)
                {
                    aiWhole[w].slotMatrix[q].resize(39);
                    fileToLoad->read(&aiWhole[w].slotMatrix[q][0], 4 * 39);
                    aiWhole[w].slotMatrix[q][0] += Tools::randomSmallValue();
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
                    float someBufMore[3];
                    fileToLoad->read(someBufMore, 4 * 3);
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

                aiWhole[w].hackType = *reinterpret_cast<int *>(&someBuf[1]);
                aiWhole[w].hack1 = someBuf[6];
                aiWhole[w].hack2 = someBuf[7];

                DWORD someData;
                fileToLoad->read(&someData, 4);

                while(someData)
                {
                    DWORD someData2;
                    fileToLoad->read(&someData2, 4);

                    for(size_t q = 0; q < someData2; ++q)
                    {
                        AIData aiData;

                        float someBuf2[9];//x, y, z, normx?, normy?, normz?, unknown, unknown, unknown
                        fileToLoad->read(someBuf2, 36);

                        aiData.pos.x = someBuf2[0];
                        aiData.pos.y = someBuf2[1];
                        aiData.pos.z = -someBuf2[2];

                        aiData.tangent.x = someBuf2[3];
                        aiData.tangent.y = someBuf2[4];
                        aiData.tangent.z = -someBuf2[5];

                        aiData.magic.x = someBuf2[6];
                        aiData.magic.y = someBuf2[7];
                        aiData.magic.z = -someBuf2[8];

                        aiWhole[w].aiData.push_back(aiData);
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

    for(size_t w = 0; w < gameState.getAICount(); ++w)
    {
        if(w != 2)
            gameState.getAICar(w).setAIData(aiWhole[w], sceneMgr, isDebugAI);
        else
            gameState.getAICar(w).setAIData(aiWhole[6], sceneMgr, isDebugAI);
    }
}