#ifndef OGRETOOLS_H
#define OGRETOOLS_H

#include "../includes/OgreInclude.h"
#include "../includes/OISInclude.h"

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


void AdjustSuspension(Ogre::Mesh* mesh,
                        const std::vector<std::vector<size_t> >& suspensionIndices,
                        const std::vector<std::vector<Ogre::Vector3> >& suspensionPointOriginalPos,
                        const Ogre::Real frontLSuspHeight,
                        const Ogre::Real frontRSuspHeight,
                        const Ogre::Real backLSuspHeight,
                        const Ogre::Real backRSuspHeight);

/**
 * a, b - should be normalized direction vectors
 */
Ogre::Real GetSignedAngle(const Ogre::Vector3& a, const Ogre::Vector3& b);

/**
 * Get angle in radians between two quaternions
 */
Ogre::Vector3 GetAngleBetweenQuaternions(const Ogre::Quaternion& from, const Ogre::Quaternion& to);

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

/**
 * return - 000000 (black); FFFFFF(white)
 */
std::string OgreColorToString(const Ogre::ColourValue& color);

Ogre::Vector3 RGBToXYZ(Ogre::Vector3 RGB, float gamma = 2.4f);
Ogre::Vector3 XYZToRGB(const Ogre::Vector3& XYZ, float gamma = 2.4f);

size_t getPowerOf2(size_t val);

Ogre::DataStreamPtr getReadableFile(const std::string& dataDir, const std::string& fileName);
Ogre::DataStreamPtr getWritibleFile(const std::string& dataDir, const std::string& fileName);

std::string keyCodeToString(OIS::KeyCode code);

#endif