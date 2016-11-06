
#ifndef XMLPARSING_H
#define XMLPARSING_H

#include "OgreInclude.h"
#include "CommonIncludes.h"

class TiXmlDocument;
class TiXmlElement;

class XMLParsing
{
public:
protected:
    TiXmlElement* parseDotScene(CommonIncludes::shared_ptr<TiXmlDocument>& xml, const Ogre::String &SceneName, const Ogre::String &groupName = Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    Ogre::String getAttrib(TiXmlElement *XMLNode, const Ogre::String &parameter, const Ogre::String &defaultValue = "");
    Ogre::Real getAttribReal(TiXmlElement *XMLNode, const Ogre::String &parameter, Ogre::Real defaultValue = 0);
    int getAttribInt(TiXmlElement *XMLNode, const Ogre::String &parameter, int defaultValue = 0);
    bool getAttribBool(TiXmlElement *XMLNode, const Ogre::String &parameter, bool defaultValue = false);
    Ogre::Matrix4 parseMatrix43(TiXmlElement *XMLNode);
    Ogre::Vector3 parseVector3(TiXmlElement *XMLNode);
    Ogre::ColourValue parseColour(TiXmlElement *XMLNode);
};

#endif
