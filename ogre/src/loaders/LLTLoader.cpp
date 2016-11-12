#include "../pcheader.h"

#include <vector>

#include "LLTLoader.h"

namespace
{
    struct TrackPoint
    {
        typedef unsigned int DWORD;
        Ogre::Vector3 vert; 
        DWORD index; 
        float weight; 
        bool isCheckpoint; 
        Ogre::Vector3 dir;
    };
}//anonimous namespace

LLTLoader::LLTLoader() : mNameGenNodes("Scene/Node/LLT/Name")
{}

void LLTLoader::load(GameState& gameState, Ogre::SceneManager* sceneMgr, bool isDebugLLT) const
{
    typedef unsigned int DWORD;
    FILE * fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getTrackName() + "/graphst", "laptrack.lpf");
    if(fileToLoad)
    {
        bool endendFound = false;
        std::vector<TrackPoint> track;

        DWORD flag;
        fread(&flag, 4, 1, fileToLoad);
        if(flag <= 1)
        {
            char LLTINFO[8];
            fread(LLTINFO, 8, 1, fileToLoad);
            if(std::string(LLTINFO) == "LLTINFO")
            {
                DWORD someCount;    // checkpoints count
                fread(&someCount, 4, 1, fileToLoad);

                DWORD something = 0;
                if(flag >= 1)
                {
                    fread(&something, 4, 1, fileToLoad);
                }

                DWORD someCount3, someCount2;
                fread(&someCount3, 4, 1, fileToLoad);
                fread(&someCount2, 4, 1, fileToLoad); // track points count

                //track points
                for(size_t q = 0; q < someCount2; ++q)
                {
                    struct LLT_INFO_1
                    {
                        DWORD some_flag; // is checkpoint
                        float x, y, z;
                        float weight;
                        DWORD nextIndex, prevIndex;
                        float n1, n2, n3;   // rotation normals for direction arrow???
                    }lltinfo1;

                    fread(&lltinfo1, sizeof(LLT_INFO_1), 1, fileToLoad);

                    TrackPoint tp;
                    tp.vert.x = lltinfo1.x;
                    tp.vert.y = lltinfo1.y;
                    tp.vert.z = -lltinfo1.z,
                    tp.index = lltinfo1.nextIndex;
                    tp.weight = lltinfo1.weight;
                    tp.isCheckpoint = lltinfo1.some_flag == 1 ? true : false;
                    tp.dir.x = lltinfo1.n1;
                    tp.dir.y = lltinfo1.n2;
                    tp.dir.z = -lltinfo1.n3;

                    track.push_back(tp);
                }

                for(size_t q = 0; q < someCount; ++q)
                {
                    DWORD some_index, some_index2;
                    fread(&some_index, 4, 1, fileToLoad);
                    fread(&some_index2, 4, 1, fileToLoad);
                }

                if(something > 0)
                {
                    for(size_t q = 0; q < something; ++q)
                    {
                        float someA, someB, someC, someD, someE;
                        fread(&someA, 4, 1, fileToLoad);
                        fread(&someB, 4, 1, fileToLoad);
                        fread(&someC, 4, 1, fileToLoad);
                        fread(&someD, 4, 1, fileToLoad);
                        fread(&someE, 4, 1, fileToLoad);
                    }
                }

                if(someCount3 > 0)
                {
                    for(size_t q = 0; q < someCount3; ++q)
                    {
                        float someA;
                        DWORD someCountA, someCountB;
                        fread(&someA, 4, 1, fileToLoad);
                        fread(&someCountA, 4, 1, fileToLoad);
                        fread(&someCountB, 4, 1, fileToLoad);

                        if(someCountA > 0)
                        {
                            for(size_t w = 0; w < someCountA; ++w)
                            {
                                DWORD someIndex;
                                fread(&someIndex, 4, 1, fileToLoad);
                            }
                        }

                        if(someCountB > 0)
                        {
                            for(size_t w = 0; w < someCountA; ++w)
                            {
                                DWORD someIndex;
                                fread(&someIndex, 4, 1, fileToLoad);
                            }
                        }
                    }
                }

                char LLTGRID[8];
                fread(LLTGRID, 8, 1, fileToLoad);
                if(std::string(LLTGRID) == "LLTGRID")
                {
                    DWORD LLTGRID_someCount, LLTGRID_someCount2;
                    float LLTGRID_unknown, LLTGRID_unknown2;
                    DWORD LLTGRID_unknown3, LLTGRID_unknown4;
                    DWORD LLTGRID_unknown5, LLTGRID_unknown6;
                    float LLTGRID_unknown7, LLTGRID_unknown8;
                    float LLTGRID_unknown9, LLTGRID_unknown10;

                    fread(&LLTGRID_someCount, 4, 1, fileToLoad);
                    fread(&LLTGRID_someCount2, 4, 1, fileToLoad);
                    fread(&LLTGRID_unknown, 4, 1, fileToLoad);
                    fread(&LLTGRID_unknown2, 4, 1, fileToLoad);
                    fread(&LLTGRID_unknown3, 4, 1, fileToLoad);
                    fread(&LLTGRID_unknown4, 4, 1, fileToLoad);
                    fread(&LLTGRID_unknown5, 4, 1, fileToLoad);
                    fread(&LLTGRID_unknown6, 4, 1, fileToLoad);
                    fread(&LLTGRID_unknown7, 4, 1, fileToLoad);
                    fread(&LLTGRID_unknown8, 4, 1, fileToLoad);
                    fread(&LLTGRID_unknown9, 4, 1, fileToLoad);
                    fread(&LLTGRID_unknown10, 4, 1, fileToLoad);

                    for(size_t q = 0; q < LLTGRID_someCount2; ++q)
                    {
                        DWORD someIndex, someIndex2;
                        fread(&someIndex, 4, 1, fileToLoad);
                        fread(&someIndex2, 4, 1, fileToLoad);
                    }

                    for(size_t q = 0; q < someCount2; ++q)
                    {
                        DWORD someIndex;
                        fread(&someIndex, 4, 1, fileToLoad);
                    }

                    char ENDENDE[8];
                    fread(ENDENDE, 8, 1, fileToLoad);
                    if(std::string(ENDENDE) == "ENDENDE")
                    {
                        endendFound = true;
                    }
                }
            }

        }

        if(endendFound)
        {
            std::vector<Ogre::Vector3> positions(track.size());
            std::vector<Ogre::Vector3> dirs(track.size());
            std::vector<Ogre::Real> weights(track.size());
            std::vector<bool> isCheckpoints(track.size());

            size_t nextIndex = 0;
            for(size_t q = 0; q < track.size(); ++q)
            {
                if(q != 0) nextIndex = track[nextIndex].index;
                positions[q] = track[nextIndex].vert;
                dirs[q] = track[nextIndex].dir;
                weights[q] = track[nextIndex].weight;
                isCheckpoints[q] = track[nextIndex].isCheckpoint;
            }


            gameState.setLLTObject(NULL);

            if(isDebugLLT)
            {
                if(!positions.empty())
                {
                    gameState.setLLTObject(sceneMgr->createManualObject("llt")); 
                    Ogre::SceneNode* lltNode = sceneMgr->getRootSceneNode()->createChildSceneNode("node_llt"); 
                     
                    gameState.getLLTObject()->begin("Basewhite", Ogre::RenderOperation::OT_LINE_LIST); 
                    for(size_t q = 0; q < positions.size(); ++q)
                    {
                        if(q !=  positions.size() - 1)
                        {
                            gameState.getLLTObject()->position(positions[q]); 
                            gameState.getLLTObject()->position(positions[q + 1]); 
                        }
                        else
                        {
                            gameState.getLLTObject()->position(positions[q]); 
                            gameState.getLLTObject()->position(positions[0]); 
                        }
                    }
                    gameState.getLLTObject()->end(); 
                    gameState.getLLTObject()->setCastShadows(false);
                    gameState.getLLTObject()->setVisible(false);
                     
                    lltNode->attachObject(gameState.getLLTObject());
                }

                //checkpoints
                if(!isCheckpoints.empty())
                {
                    for(size_t q = 0; q < isCheckpoints.size(); ++q)
                    {
                        if(isCheckpoints[q])
                        {
#if 1
                            Ogre::Real scale = 0.1f;
                            Ogre::String debugSphereName = mNameGenNodes.generate();
                            Ogre::Entity * debugSphere = sceneMgr->createEntity(debugSphereName, Ogre::SceneManager::PT_SPHERE);
                            debugSphere->setMaterialName("BaseWhiteNoLighting");
                            Ogre::SceneNode * debugSphereNode = sceneMgr->getRootSceneNode()->createChildSceneNode(debugSphereName);
                            debugSphereNode->attachObject(debugSphere);
                            debugSphereNode->setPosition(positions[q]);
                            debugSphereNode->setScale(scale, scale, scale);
                            debugSphere->setCastShadows(false);
#else

                            Ogre::Real widthLeft = 70.0f;
                            Ogre::Real widthRight = 30.0f;
                            Ogre::Real heightUp = 50.0f;
                            Ogre::Real heightDown = 20.0f;
                            Ogre::String debugPlaneName = mNameGenNodes.generate();

                            Ogre::ManualObject * manual =  sceneMgr->createManualObject(debugPlaneName); 
                            Ogre::SceneNode* lltPlaneNode = sceneMgr->getRootSceneNode()->createChildSceneNode(debugPlaneName); 
                            
                            Ogre::Vector3 sideVec = dirs[q].crossProduct(Ogre::Vector3::UNIT_Y);

                            manual->begin("Basewhite", Ogre::RenderOperation::OT_TRIANGLE_LIST); 

                            manual->position((positions[q] - sideVec * widthLeft) - Ogre::Vector3(0.0f, heightDown, 0.0f));
                            manual->position((positions[q] + sideVec * widthRight) - Ogre::Vector3(0.0f, heightDown, 0.0f));
                            manual->position((positions[q] + sideVec * widthRight) + Ogre::Vector3(0.0f, heightUp, 0.0f));

                            manual->position((positions[q] + sideVec * widthRight) + Ogre::Vector3(0.0f, heightUp, 0.0f));
                            manual->position((positions[q] - sideVec * widthLeft) + Ogre::Vector3(0.0f, heightUp, 0.0f));
                            manual->position((positions[q] - sideVec * widthLeft) - Ogre::Vector3(0.0f, heightDown, 0.0f));

                            manual->end(); 
                            manual->setCastShadows(false);
                            lltPlaneNode->attachObject(manual);
#endif
                        }
                    }
                }

                //dirs
                if(!dirs.empty())
                {
                    for(size_t q = 0; q < dirs.size(); ++q)
                    {
                        Ogre::Real length = 5.0f;
                        Ogre::String debugDirName = mNameGenNodes.generate();

                        Ogre::ManualObject * manual =  sceneMgr->createManualObject(debugDirName); 
                        Ogre::SceneNode* lltDirNode = sceneMgr->getRootSceneNode()->createChildSceneNode(debugDirName); 
                         
                        manual->begin("Basewhite", Ogre::RenderOperation::OT_LINE_LIST); 


                        manual->position(positions[q]);
                        manual->position(positions[q] + dirs[q] * length);

                        manual->end(); 
                        manual->setCastShadows(false);
                        lltDirNode->attachObject(manual);
                    }
                }
            }

            for(int w = 0; w < gameState.getMaxAI(); ++w)
            {
                gameState.getAICar(w).getLapUtils().setData(positions, dirs, weights, isCheckpoints, sceneMgr, isDebugLLT);
            }
            /*
            for(int w = 0; w < gameState.getMaxMultiplayer(); ++w)
            {
                gameState.getMultiplayerCar(w).getLapUtils().setData(positions, dirs, weights, isCheckpoints, sceneMgr, isDebugLLT);
            }*/
            gameState.getPlayerCar().getLapUtils().setData(positions, dirs, weights, isCheckpoints, sceneMgr, isDebugLLT);
        }

        fclose(fileToLoad);
    }
    else {assert(false && "No LLT file");}
}