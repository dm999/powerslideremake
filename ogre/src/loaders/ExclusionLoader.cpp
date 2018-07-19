
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
                Ogre::Vector3 rotX, rotY, rotZ, pos;

                float min[3], max[3], falloff, min_exclude;
                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "Min : %f,%f,%f\n", &min[0], &min[1], &min[2]);

                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "Max : %f,%f,%f\n", &max[0], &max[1], &max[2]);

                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "Trans : %f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                                                &rotX.x, &rotX.y, &rotX.z,
                                                &rotY.x, &rotY.y, &rotY.z,
                                                &rotZ.x, &rotZ.y, &rotZ.z,
                                                &pos.x, &pos.y, &pos.z);

                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "Falloff : %f\n", &falloff);

                fileToLoad->readLine(buf, 1024);
                sscanf(buf, "Min exclude : %f\n", &min_exclude);

                Exclusion excl;

                //original data is left hand
                excl.min.x = min[0];
                excl.min.y = min[2];
                excl.min.z = min[1];

                excl.max.x = max[0];
                excl.max.y = max[2];
                excl.max.z = max[1];

                excl.transform = Ogre::Matrix4(
                    rotX.x, rotY.x, rotZ.x, -pos.x,
                    rotX.y, rotY.y, rotZ.y, -pos.z,
                    rotX.z, rotY.z, rotZ.z, pos.y,
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

                        const Exclusion& excl = gameState.getExclusions()[q].exclusions[w];

                        //lower
                        exclObject->position(excl.min.x, excl.min.y, excl.min.z); 
                        exclObject->position(excl.max.x, excl.min.y, excl.min.z); 

                        exclObject->position(excl.min.x, excl.min.y, excl.min.z); 
                        exclObject->position(excl.min.x, excl.min.y, excl.max.z); 

                        exclObject->position(excl.max.x, excl.min.y, excl.max.z); 
                        exclObject->position(excl.min.x, excl.min.y, excl.max.z); 

                        exclObject->position(excl.max.x, excl.min.y, excl.max.z); 
                        exclObject->position(excl.max.x, excl.min.y, excl.min.z); 


                        //upper
                        exclObject->position(excl.min.x, excl.max.y, excl.min.z); 
                        exclObject->position(excl.max.x, excl.max.y, excl.min.z); 

                        exclObject->position(excl.min.x, excl.max.y, excl.min.z); 
                        exclObject->position(excl.min.x, excl.max.y, excl.max.z); 

                        exclObject->position(excl.max.x, excl.max.y, excl.max.z); 
                        exclObject->position(excl.min.x, excl.max.y, excl.max.z); 

                        exclObject->position(excl.max.x, excl.max.y, excl.max.z); 
                        exclObject->position(excl.max.x, excl.max.y, excl.min.z); 


                        //sides
                        exclObject->position(excl.min.x, excl.min.y, excl.min.z); 
                        exclObject->position(excl.min.x, excl.max.y, excl.min.z); 

                        exclObject->position(excl.min.x, excl.min.y, excl.max.z); 
                        exclObject->position(excl.min.x, excl.max.y, excl.max.z); 

                        exclObject->position(excl.max.x, excl.min.y, excl.min.z); 
                        exclObject->position(excl.max.x, excl.max.y, excl.min.z); 

                        exclObject->position(excl.max.x, excl.min.y, excl.max.z); 
                        exclObject->position(excl.max.x, excl.max.y, excl.max.z); 


                        exclObject->end(); 
                        exclObject->setCastShadows(false);
                        //mLLTObject->setVisible(false);
                         
                        exclNode->attachObject(exclObject);

                        Ogre::Vector3 trans = excl.transform.getTrans();
                        Ogre::Quaternion rot = excl.transform.extractQuaternion();
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