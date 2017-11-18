# Readme #

### Goal ###

"Powerslide remake" remakes main features of video game [Powerslide](https://en.wikipedia.org/wiki/Powerslide_(video_game)) - arcade racing in non limited 3D environment, advanced AI based on neural network, multiplayer gaming. It has been implemented using programmable rendering pipeline and crossplatform (windows, linux, android) ready.

### Build ###
To build the project you need:  
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
- [MyGUI](https://github.com/MyGUI/mygui)  
- [Bullet](http://bulletphysics.org/wordpress/)  
- [Lua](https://www.lua.org/)  
- [SimpleINI](https://github.com/brofield/simpleini)  
- [JSON++](https://github.com/hjiang/jsonxx)  
- [Multislider](https://bitbucket.org/alexey_gruzdev/multislider) (only if PowerslideRemake_MULTIPLAYER ON)  
- [Boost::asio](http://www.boost.org/doc/libs/1_60_0/doc/html/boost_asio.html) (only if PowerslideRemake_MULTIPLAYER ON)  

Class diagram of source code presented in docs/ClassDiagram.graphml (use [yED](http://www.yworks.com/products/yed) to open)  

### Run ###
To run builded project you need to support original game assets (packed files - pf). Game assets could be found on original CD or bought at [GOG](https://www.gog.com/game/powerslide). 
You need to have: data.pf, gameshell.pf, store.pf. Place them in the folder with builded application.  
For android version you need to store pf files in <powerslide> folder of External Storage Directory (where DCIM located) of smartphone.  

### License ###
The MIT License (MIT)  
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:  
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.  
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

