#ifndef ToolsH
#define ToolsH

#include "../includes/OgreInclude.h"

Ogre::MaterialPtr CloneMaterial(const Ogre::String& newMaterialName, const Ogre::String& oldMaterialName, 
                                const std::vector<Ogre::String> texturesNames, 
                                float scale = 1.0f,
                                const Ogre::String& groupName = Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

void TranslateMesh(const Ogre::Entity *ent, const Ogre::Vector3 &translate);

bool CompareVectors(const Ogre::Vector3& a, const Ogre::Vector3& b, float epsilon = 0.001f);

Ogre::Vector3 FindCentroid(const Ogre::Entity *ent);

void BuildTangents(Ogre::Entity* entity);

void AddColorAttrib(Ogre::Entity* entity);

void AddjustNormals(Ogre::Entity* entity, Ogre::Real threshold);

std::string extractFilePath(const std::string& fullName);

void AdjustBufferToUseShadow(   Ogre::Entity* entity, 
                                const std::vector<std::vector<Ogre::Vector3> >& suspensionData, 
                                std::vector<std::vector<size_t> >& suspensionIndices,
                                std::vector<std::vector<Ogre::Vector3> >& suspensionPointOriginalPos);


void AdjustSuspension(  Ogre::Entity* cockpit,
                        Ogre::SceneNode * wheels[4],
                        const Ogre::Vector3& chassisOffset,
                        const Ogre::Quaternion& chassisRot,
                        const std::vector<std::vector<size_t> >& suspensionIndices,
                        const std::vector<std::vector<Ogre::Vector3> >& suspensionPointOriginalPos,
                        const Ogre::Vector3& frontLOriginalPos,
                        const Ogre::Vector3& frontROriginalPos,
                        const Ogre::Vector3& backLOriginalPos,
                        const Ogre::Vector3& backROriginalPos);

/**
 * a, b - should be normalized direction vectors
 */
Ogre::Real GetSignedAngle(const Ogre::Vector3& a, const Ogre::Vector3& b);

/**
 * a - point, b, c - start/end points of line
 * return - point within line
 */
Ogre::Vector3 ProjectPointOnLine(const Ogre::Vector3& a, const Ogre::Vector3& b, const Ogre::Vector3& c);

/**
 * return - point out of line possible
 */
Ogre::Vector3 NearestPoint(const Ogre::Vector3 &pt1, const Ogre::Vector3 &pt2, const Ogre::Vector3 &testPoint);

Ogre::TexturePtr loadChromaKeyedTexture(const Ogre::String& filename, 
         const Ogre::ColourValue& keyCol = Ogre::ColourValue::Black, 
         const Ogre::String& resGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
         const Ogre::String& prefix = "ck_",
         int numMipmaps = Ogre::MIP_DEFAULT,
         float threshold = 0.003f,
         bool resize = false,
         size_t newSize = 64);

#endif