#ifndef OGREINCLUDE_H
#define OGREINCLUDE_H

#include "Ogre.h"

#if defined(__ANDROID__)
    #include "OgreGLES2Plugin.h"
#else
    #include "OgreGLPlugin.h"
#endif
#include "Plugins/ParticleFX/OgreParticleFXPlugin.h"
#include "Plugins/OctreeSceneManager/OgreOctreePlugin.h"

#endif
