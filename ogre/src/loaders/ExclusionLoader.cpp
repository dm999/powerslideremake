
#include "ExclusionLoader.h"

ExclusionLoader::ExclusionLoader() : mNameGenNodes("Scene/Node/Exclusion/Name")
{}

void ExclusionLoader::load(GameState& gameState, Ogre::SceneManager* sceneMgr, bool isDebugExclusion) const
{
    std::string excludeFile = gameState.getSTRPowerslide().getExclusionFile(gameState.getTrackName());

    Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile(
        "data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()), 
        excludeFile);

    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        gameState.getExclusions().clear();

        while(1)
        {
            LightEclusion lightExcl;

            int count;
            char buf[1025];
            fileToLoad->readLine(buf, 1024);
            int err = sscanf(buf, "%d\n", &count);
            if(err == 0) break;

            for(int q = 0; q < count; ++q)
            {
                float min[3], max[3], trans[4][3], falloff, min_exclude;
                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "Min : %f,%f,%f\n", &min[0], &min[1], &min[2]);

                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "Max : %f,%f,%f\n", &max[0], &max[1], &max[2]);

                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "Trans : %f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                                                &trans[0][0], &trans[0][1], &trans[0][2],
                                                &trans[1][0], &trans[1][1], &trans[1][2],
                                                &trans[2][0], &trans[2][1], &trans[2][2],
                                                &trans[3][0], &trans[3][1], &trans[3][2]);

                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "Falloff : %f\n", &falloff);

                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "Min exclude : %f\n", &min_exclude);

                Exclusion excl;

                excl.min.x = min[0];
                excl.min.y = min[2];
                excl.min.z = min[1];

                excl.max.x = max[0];
                excl.max.y = max[2];
                excl.max.z = max[1];

                excl.transform = Ogre::Matrix4(
                trans[0][0], trans[1][0], trans[2][0], -trans[3][0],
                trans[0][1], trans[1][1], trans[2][1], -trans[3][2],
                trans[0][2], trans[1][2], trans[2][2], trans[3][1],
                0.0f, 0.0f, 0.0f, 1.0f);

                excl.falloff = falloff;
                excl.reciprocalFalloff = 1.0f / excl.falloff;

                excl.minExclusion = min_exclude;

                Ogre::Vector3 translation = excl.transform.getTrans();
                Ogre::Quaternion rot = excl.transform.extractQuaternion();

                Ogre::Radian angle;
                Ogre::Vector3 axis;
                rot.ToAngleAxis(angle, axis);
                rot.FromAngleAxis(angle, Ogre::Vector3(-axis.x, -axis.z, axis.y));

                Ogre::Matrix4 transform;
                Ogre::Vector3 derivedPosition = rot * translation;
                transform.makeTransform(derivedPosition, Ogre::Vector3::UNIT_SCALE, rot);

                excl.ogreInverseTransform = transform.inverse();

                lightExcl.exclusions.push_back(excl);

            }

            gameState.getExclusions().push_back(lightExcl);
        }

        if(isDebugExclusion)
        {
            if(!gameState.getExclusions().empty())
            {
                for(size_t q = 0; q < gameState.getExclusions().size(); ++q)
                {
                    for(size_t w = 0; w < gameState.getExclusions()[q].exclusions.size(); ++w)
                    {
                        Ogre::String name = mNameGenNodes.generate();
                        Ogre::ManualObject* exclObject =  sceneMgr->createManualObject(name); 
                        Ogre::SceneNode* exclNodeRoot = sceneMgr->getRootSceneNode()->createChildSceneNode(mNameGenNodes.generate()); 
                        Ogre::SceneNode* exclNode = exclNodeRoot->createChildSceneNode(name); 
                         
                        exclObject->begin("Basewhite", Ogre::RenderOperation::OT_LINE_LIST); 


                        //lower
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].min.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].min.z); 
                                                                                                                           
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].min.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].max.z); 
                                                                                                                           
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].max.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].max.z); 
                                                                                                                           
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].max.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].min.z); 


                        //upper
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].min.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].min.z); 
                                                                                                                           
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].min.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].max.z); 
                                                                                                                           
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].max.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].max.z); 
                                                                                                                           
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].max.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].min.z); 


                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].min.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].min.z); 
                                                                                                                           
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].max.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].min.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].max.z); 
                                                                                                                           
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].min.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].min.z); 
                                                                                                                           
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].min.y, gameState.getExclusions()[q].exclusions[w].max.z); 
                        exclObject->position(gameState.getExclusions()[q].exclusions[w].max.x, gameState.getExclusions()[q].exclusions[w].max.y, gameState.getExclusions()[q].exclusions[w].max.z); 


                        exclObject->end(); 
                        exclObject->setCastShadows(false);
                        //mLLTObject->setVisible(false);
                         
                        exclNode->attachObject(exclObject);

                        Ogre::Vector3 trans = gameState.getExclusions()[q].exclusions[w].transform.getTrans();
                        Ogre::Quaternion rot = gameState.getExclusions()[q].exclusions[w].transform.extractQuaternion();

                        Ogre::Radian angle;
                        Ogre::Vector3 axis;
                        rot.ToAngleAxis(angle, axis);
                        rot.FromAngleAxis(angle, Ogre::Vector3(-axis.x, -axis.z, axis.y));


                        exclNode->setPosition(trans);
                        exclNodeRoot->setOrientation(rot);
                    }
                }
            }
        }

        fileToLoad->close();
    }
}