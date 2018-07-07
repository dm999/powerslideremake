# Readme #

### Goal ###

"Powerslide remake" project remakes main features of video game [Powerslide](https://en.wikipedia.org/wiki/Powerslide_(video_game)) - arcade racing in non limited 3D environment, advanced AI based on neural network, multiplayer gaming. It has been implemented using programmable rendering pipeline and crossplatform (windows, linux, android) ready.

### Build ###
To build the project you need:  
- [CMake 2.8 or higher](https://cmake.org/)  
- C++03 compiler  
- [Ogre 1.9.0](http://www.ogre3d.org/)  
- [OpenAL](https://www.openal.org) (only if PowerslideRemake_OPENAL ON)  
- [Boost](https://www.boost.org) (only if PowerslideRemake_MULTIPLAYER ON)  

Initial steps before building project is preparation of dependencies: you need to build Ogre (static, OpenGL render system), OpenAL, Boost::asio.  

To build project you need:  
- Set CMake variable PowerslideRemake_OGREHOME - path to Ogre includes & binaries (for example C:\Program Files\OGRE\1_9_0)  
- Set CMake variable PowerslideRemake_OPENALDIR (only if PowerslideRemake_OPENAL ON) - path to OpenAL includes & binaries (for example C:\Program Files\OpenAL)  
- Set CMake variable PowerslideRemake_BOOSTHOME (only if PowerslideRemake_MULTIPLAYER ON) - path to Boost includes & binaries for libs [system, date_time, regex] builded statically (for example C:\Boost)  

### Sources ###
"Powerslide remake" also depends and/or includes sources of such open sourced projects as:  
- [Lua](https://www.lua.org/)  
- [SimpleINI](https://github.com/brofield/simpleini)  
- [JSON++](https://github.com/hjiang/jsonxx)  
- [Multislider](https://bitbucket.org/alexey_gruzdev/multislider) (only if PowerslideRemake_MULTIPLAYER ON)  
- [Boost::asio](http://www.boost.org/doc/libs/1_60_0/doc/html/boost_asio.html) (only if PowerslideRemake_MULTIPLAYER ON)  
- [ogg/vorbis](https://www.xiph.org/) (only if PowerslideRemake_OPENAL ON)  
- [ogg/vorbis cmake](https://github.com/Iunusov/OGG-Vorbis-CMAKE) (only if PowerslideRemake_OPENAL ON)  
- [SFML](https://www.sfml-dev.org/) (only if PowerslideRemake_OPENAL ON)  

Class diagram of source code presented in docs/ClassDiagram.graphml (use [yED](http://www.yworks.com/products/yed) to open)  

### Run ###
To run builded project you need to support original game assets (packed files - pf). Game assets can be taken from original CD or bought at [GOG](https://www.gog.com/game/powerslide). 
You need to have: data.pf, gameshell.pf, store.pf (patch.pf is optional). Put them in the folder with builded application.  
For android version you need to store pf files in <powerslide> folder of External Storage Directory (where DCIM located) of smartphone.  

### Music ###
It is possible to listen to in game music. To enable sound/music project should be built with PowerslideRemake_OPENAL. Put original CD tracks compressed as .ogg files into the same directory as .pf files. Name convention for files:  
track_0.ogg - main theme  
track_2-8.ogg - other tracks {dam track, sand blaster, speedway, devil`s elbow, mineshafted, freezer, urban brawl}  

### Multiplayer ###
"Powerslide remake" supports multiplayer game through web server. To run multiplayer game web server should be deployed on most power PC of local network or global node. Prebuilt web server available in /Downloads section ([multislider-08.zip](https://bitbucket.org/dm_999/powerslideremake/downloads/multislider-0.8.zip)). To start web server command should be executed: (windows: multislider-0.8/bin/multislider.bat 192.168.1.1, linux: ./multislider-0.8/bin/multislider 192.168.1.1) - IP should be adjusted. Ports being used by multiplayer: TCP:8800, UDP:8800  

### License ###
The MIT License (MIT)  
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:  
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.  
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

