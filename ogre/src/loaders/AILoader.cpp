#include "../pcheader.h"

#include "AILoader.h"

#include "../Conversions.h"

AILoader::AILoader() : mNameGenNodes("Scene/Node/AI/Name")
{}

void AILoader::load(GameState& gameState, Ogre::SceneManager* sceneMgr, bool isDebugAI, AIStrength aiStrength) const
{
    std::string folderWithData = "chareasy";

    switch(aiStrength)
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

    for(size_t w = 0; w < gameState.getAICount(); ++w)
    {

        FILE * fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getTrackName() + "/ai/" + folderWithData, "rec" + Conversions::DMToString(w));

        if(fileToLoad)
        {
            //gameState.getAICar(w).clearAIData();

            std::vector<AIData> aIData;

            float someBuf[9];
            fread(someBuf, 36, 1, fileToLoad);

            typedef unsigned int DWORD;

            DWORD someData;
            fread(&someData, 4, 1, fileToLoad);

            while(someData)
            {
                DWORD someData2;
                fread(&someData2, 4, 1, fileToLoad);

                for(size_t q = 0; q < someData2; ++q)
                {
                    AIData aiData;

                    float someBuf2[9];//x, y, z, normx?, normy?, normz?, unknown, unknown, unknown
                    fread(someBuf2, 36, 1, fileToLoad);

                    aiData.pos.x = someBuf2[0];
                    aiData.pos.y = someBuf2[1];
                    aiData.pos.z = -someBuf2[2];

                    aiData.dir.x = someBuf2[3];
                    aiData.dir.y = someBuf2[4];
                    aiData.dir.z = -someBuf2[5];

                    aIData.push_back(aiData);
                }

                fread(&someData, 4, 1, fileToLoad);
            }


            gameState.getAICar(w).setAIData(aIData, sceneMgr, isDebugAI);

            if(isDebugAI)
            {
                for(size_t q = 0; q < aIData.size(); ++q)
                {
                    //spheres
                    Ogre::Real scale = 0.1f;
                    Ogre::String debugSphereName = mNameGenNodes.generate();
                    Ogre::Entity * debugSphere = sceneMgr->createEntity(debugSphereName, Ogre::SceneManager::PT_SPHERE);
                    debugSphere->setMaterialName("BaseWhiteNoLighting");
                    Ogre::SceneNode * debugSphereNode = sceneMgr->getRootSceneNode()->createChildSceneNode(debugSphereName);
                    debugSphereNode->attachObject(debugSphere);
                    debugSphereNode->setPosition(aIData[q].pos);
                    debugSphereNode->setScale(scale, scale, scale);
                    debugSphere->setCastShadows(false);

                    //dirs
                    Ogre::Real length = 15.0f;
                    Ogre::String debugDirName = mNameGenNodes.generate();

                    Ogre::ManualObject * manual =  sceneMgr->createManualObject(debugDirName); 
                    Ogre::SceneNode* lltDirNode = sceneMgr->getRootSceneNode()->createChildSceneNode(debugDirName); 
                     
                    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST); 


                    manual->position(aIData[q].pos);
                    manual->position(aIData[q].pos + aIData[q].dir * length);

                    manual->end(); 
                    manual->setCastShadows(false);
                    lltDirNode->attachObject(manual);
                }
            }

            fclose(fileToLoad);
        }
    }
}