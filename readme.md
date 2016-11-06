# Readme #

### Goal ###

"Powerslide rebuild" is the project which tries to rebuild main features of video game [Powerslide](https://en.wikipedia.org/wiki/Powerslide_(video_game)) such as arcade racing in non limited 3D environment.

### Build ###
To build the project you need:  
- C++ compiler  
- [Ogre 1.9.0](http://www.ogre3d.org/)  
- [OpenAL](https://www.openal.org)  
- [Enet](http://enet.bespin.org/)  
- [Raknet](http://www.jenkinssoftware.com/)  
- [Multislider](https://bitbucket.org/alexey_gruzdev/multislider)  

Initial steps before building project is preparation of dependencies: you need to build Ogre (static, OpenGL render system), OpenAL, Multislider.  
There are prebuild versions of OpenAL & Multislider (MSVC 2008) are included into project.

To build project you need set environment variables:  
- OGRE_HOME - path to Ogre includes & binaries  
- OPENAL - path to OpenAL includes & binaries  

### Sources ###
"Powerslide rebuild" also depends and includes sources of such open sourced projects as:  
- [Bullet](http://bulletphysics.org/wordpress/)  
- [Ogre Bullet](http://www.ogre3d.org/tikiwiki/OgreBullet)  
- TinyXML  
- [Lua](https://www.lua.org/)  
- [SimpleINI](https://github.com/brofield/simpleini)  

Class diagram of source code presented in docs/ClassDiagram.graphml (use [yED](http://www.yworks.com/products/yed) to open)  

### Run ###
To run builded project you need to support original game assets (packed files *.pf). Game assets could be found on original CD or bought at [GOG](https://www.gog.com/game/powerslide). 
You need to have: data.pf, gameshell.pf, store.pf.  
Make sure script variables in file assets/scripts/Main.lua points to correct location:  
Main.OriginalPathData = "F:/Data/";  
Main.OriginalPathCommon = "F:/Common/";  
Main.OriginalPathData - should point at directory with data.pf & gameshell.pf  
Main.OriginalPathCommon - should point at directory with store.pf  

### License ###
The MIT License (MIT) Copyright (c) 2016 Dmitry Polubotko  
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:  
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.  
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

