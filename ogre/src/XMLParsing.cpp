#include "pcheader.h"

#include "XMLParsing.h"

#include "tinyxml.h"

TiXmlElement* XMLParsing::parseDotScene(CommonIncludes::shared_ptr<TiXmlDocument>& xml, const Ogre::String &SceneName, const Ogre::String &groupName)
{
    try{
        Ogre::String basename, path;
        Ogre::StringUtil::splitFilename(SceneName, basename, path);
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource( basename, groupName );
        Ogre::String data = pStream->getAsString();
        xml.reset(new TiXmlDocument());
        xml->Parse( data.c_str() );

        pStream->close();
        pStream.setNull();
 
        if( xml->Error() )
        {
            //We'll just log, and continue on gracefully
            Ogre::LogManager::getSingleton().logMessage("parseDotScene The TiXmlDocument reported an error");
            xml.reset();
            return NULL;
        }

    }catch(...){
        Ogre::LogManager::getSingleton().logMessage("parseDotScene: Error creating TiXmlDocument");
    }

    // Validate the File
    TiXmlElement*XMLRoot = xml->RootElement();
    if( Ogre::String( XMLRoot->Value()) != "scene"  ) {
        Ogre::LogManager::getSingleton().logMessage( "parseDotScene Error: Invalid .scene File. Missing <scene>" );
        xml.reset();
        return NULL;
    }

    return XMLRoot;

}

Ogre::String XMLParsing::getAttrib(TiXmlElement *XMLNode, const Ogre::String &attrib, const Ogre::String &defaultValue)
{
    if(XMLNode->Attribute(attrib.c_str()))
        return XMLNode->Attribute(attrib.c_str());
    else
        return defaultValue;
}

Ogre::Real XMLParsing::getAttribReal(TiXmlElement *XMLNode, const Ogre::String &attrib, Ogre::Real defaultValue)
{
    if(XMLNode->Attribute(attrib.c_str()))
        return Ogre::StringConverter::parseReal(XMLNode->Attribute(attrib.c_str()), defaultValue);
    else
        return defaultValue;
}

int XMLParsing::getAttribInt(TiXmlElement *XMLNode, const Ogre::String &attrib, int defaultValue)
{
    if(XMLNode->Attribute(attrib.c_str()))
        return Ogre::StringConverter::parseInt(XMLNode->Attribute(attrib.c_str()), defaultValue);
    else
        return defaultValue;
}

bool XMLParsing::getAttribBool(TiXmlElement *XMLNode, const Ogre::String &attrib, bool defaultValue)
{
    if(!XMLNode->Attribute(attrib.c_str()))
        return defaultValue;
 
    if(Ogre::String(XMLNode->Attribute(attrib.c_str())) == "true" || Ogre::String(XMLNode->Attribute(attrib.c_str())) == "1")
        return true;
 
    return false;
}

Ogre::Matrix4 XMLParsing::parseMatrix43(TiXmlElement *XMLNode)
{
    return Ogre::Matrix4(
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m00")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m10")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m20")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m30")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m01")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m11")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m21")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m31")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m02")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m12")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m22")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("m32")),
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );
}

Ogre::Vector3 XMLParsing::parseVector3(TiXmlElement *XMLNode)
{
    return Ogre::Vector3(
        Ogre::StringConverter::parseReal(XMLNode->Attribute("x")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("y")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("z"))
    );
}

Ogre::ColourValue XMLParsing::parseColour(TiXmlElement *XMLNode)
{
    return Ogre::ColourValue(
        Ogre::StringConverter::parseReal(XMLNode->Attribute("r")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("g")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("b")),
        XMLNode->Attribute("a") != NULL ? Ogre::StringConverter::parseReal(XMLNode->Attribute("a")) : 1
    );
}