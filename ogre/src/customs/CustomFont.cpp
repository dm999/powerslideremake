#include "../pcheader.h"

#include "CustomFont.h"

namespace
{
#if defined(__ANDROID__)
    const char vert[] = "               \n\
    #version 100                        \n\
    uniform mat4 modelviewproj;         \n\
    uniform mat4 texturematrix;         \n\
    attribute vec4 position;            \n\
    attribute vec2 uv0;                 \n\
    attribute vec4 colour;              \n\
    varying vec2 T;                     \n\
    varying vec4 C;                     \n\
    void main()                         \n\
    {                                                   \n\
        T = (texturematrix * vec4(uv0, 0.0, 1.0)).xy;   \n\
        C = colour;                                     \n\
        gl_Position = modelviewproj * position;         \n\
    }                                                   \n\
    ";

    const char  frag[] = "                              \n\
    #version 100                                        \n\
    precision highp float;                              \n\
    uniform sampler2D diffuseMap;                       \n\
    varying vec2 T;                                     \n\
    varying vec4 C;                                     \n\
    void main()                                         \n\
    {                                                   \n\
        gl_FragColor = vec4(texture2D(diffuseMap, T) * C);  \n\
    }                                                   \n\
    ";

    const char lang[] = "glsles";
#else
    const char vert[] = "               \n\
    #version 120                        \n\
    uniform mat4 modelviewproj;         \n\
    uniform mat4 texturematrix;         \n\
    attribute vec4 position;            \n\
    attribute vec2 uv0;                 \n\
    attribute vec4 colour;              \n\
    varying vec2 T;                     \n\
    varying vec4 C;                     \n\
    void main()                         \n\
    {                                                   \n\
        T = (texturematrix * vec4(uv0, 0.0, 1.0)).xy;   \n\
        C = colour;                                     \n\
        gl_Position = modelviewproj * position;         \n\
    }                                                   \n\
    ";

    const char  frag[] = "                              \n\
    #version 120                                        \n\
    precision highp float;                              \n\
    uniform sampler2D diffuseMap;                       \n\
    varying vec2 T;                                     \n\
    varying vec4 C;                                     \n\
    void main()                                         \n\
    {                                                   \n\
        gl_FragColor = vec4(texture2D(diffuseMap, T) * C);  \n\
    }                                                   \n\
    ";

    const char lang[] = "glsl";
#endif
}

CustomFont::CustomFont(Ogre::ResourceManager* creator, const Ogre::String& name, Ogre::ResourceHandle handle,
                       const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader) :
    Font(creator, name, handle, group, isManual, loader)
{
}

void CustomFont::loadImpl()
{
    Font::loadImpl();


    if (!mMaterial.isNull())
    {
        Ogre::HighLevelGpuProgramPtr vertex = Ogre::HighLevelGpuProgramManager::getSingletonPtr()->createProgram( "font_basicVS", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, lang, Ogre::GPT_VERTEX_PROGRAM);
        vertex->setSource(vert);

        Ogre::HighLevelGpuProgramPtr fragment = Ogre::HighLevelGpuProgramManager::getSingletonPtr()->createProgram( "font_basicFS", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, lang, Ogre::GPT_FRAGMENT_PROGRAM);
        fragment->setSource(frag);

        Ogre::Pass *pass = mMaterial->getTechnique(0)->getPass(0);

        pass->setVertexProgram( vertex->getName() );
        pass->setFragmentProgram( fragment->getName() );

        Ogre::GpuProgramParametersSharedPtr params_v = pass->getVertexProgramParameters();
        params_v->setNamedAutoConstant("modelviewproj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
        params_v->setNamedAutoConstant("texturematrix", Ogre::GpuProgramParameters::ACT_TEXTURE_MATRIX);

        Ogre::GpuProgramParametersSharedPtr params_f = pass->getFragmentProgramParameters();
        params_f->setNamedConstant("diffuseMap", 0);
    }
}