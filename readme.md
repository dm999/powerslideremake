# Readme #

### Goal ###

"Powerslide remake" project remakes main features of video game [Powerslide](https://en.wikipedia.org/wiki/Powerslide_(video_game)) - arcade racing in non-limited 3D environment, with advanced AI based on neural network and multiplayer gaming. It has been implemented using programmable rendering pipeline and is cross-platform (windows, linux, android).

### Build ###
To build the project you need:  
- [CMake 2.8 or higher](https://cmake.org/)  
- C++03 compiler  
- [Ogre 1.9.0](http://www.ogre3d.org/)  
- [OpenAL](https://www.openal.org) (only if PowerslideRemake_OPENAL ON)  

Initial steps before building project is preparation of dependencies: you need to build Ogre (static, OpenGL render system), OpenAL (OpenAL Soft).  

To build project you need:  
- Set CMake variable PowerslideRemake_OGREHOME - path to Ogre includes & binaries (for example C:\Program Files\OGRE\1_9_0)  
- Set CMake variable PowerslideRemake_OPENALDIR (only if PowerslideRemake_OPENAL ON) - path to OpenAL includes & binaries (for example C:\Program Files\OpenAL)  

### Sources ###
"Powerslide remake" also depends and/or includes sources of such open sourced projects as:  
- [SFML](https://www.sfml-dev.org/)  
- [Lua](https://www.lua.org/)  
- [SimpleINI](https://github.com/brofield/simpleini)  
- [JSON++](https://github.com/hjiang/jsonxx)  
- [Yato](https://github.com/agruzdev/Yato/)  
- [Multislider](https://bitbucket.org/alexey_gruzdev/multislider)  
- [ogg/vorbis](https://www.xiph.org/) (only if PowerslideRemake_OPENAL ON)  
- [ogg/vorbis cmake](https://github.com/Iunusov/OGG-Vorbis-CMAKE) (only if PowerslideRemake_OPENAL ON)  
- [OpenCV](https://opencv.org/) AVIReadContainer class source code used to parse AVI header and extract encoded frame  
- [Cinepack codec](https://multimedia.cx/mirror/cinepak.txt) explanation of Cinepack decoding by Dr. Tim Ferguson  
- [HKLUT](https://github.com/jasonli0707/hklut) HKLUT: Hundred-Kilobyte Lookup Tables  

Class diagram of source code presented in docs/ClassDiagram.graphml (use [yED](http://www.yworks.com/products/yed) to open)  

### Run ###
To run the built project you need to support original game assets (packed files - pf). Game assets can be taken from original CD or bought at [GOG](https://www.gog.com/game/powerslide).  
You need to have: data.pf, gameshell.pf, store.pf (patch.pf is optional). Put them in the folder with the built application.  
**Important**: integrated graphics are not supported, please use discrete video card on PC.  
For android version you need to store pf files in <powerslide> folder of External Storage Directory (where DCIM located) of smartphone.  
Prebuilt archives for windows / linux / android available in /Downloads section ([windows](https://bitbucket.org/dm_999/powerslideremake/downloads/win.7z), [linux](https://bitbucket.org/dm_999/powerslideremake/downloads/linux_64.7z), [android](https://bitbucket.org/dm_999/powerslideremake/downloads/PowerslideRemake.apk)). To run on linux make sure openal installed.  

### Music ###
There is sounds/music support available in project. To enable sounds/music the project should be built with PowerslideRemake_OPENAL.
Put original CD tracks compressed as .ogg (vorbis) files into the same directory as .pf files. Name convention for files:  
track_0.ogg - main theme  
track_2-8.ogg - other tracks {dam track, sand blaster, speedway, devil`s elbow, mineshafted, freezer, urban brawl}  
File track_1.ogg is not used as well as track_9.ogg (extra track on original CD).  

### Multiplayer ###
"Powerslide remake" supports multiplayer game through web server. 
To run multiplayer game web server should be deployed on most powerful PC of a local network or on a global virtual/physical server with public IP address.
Prebuilt web server available in /Downloads section ([multislider-08.zip](https://bitbucket.org/dm_999/powerslideremake/downloads/multislider-0.8.zip)).  
To start web server command should be executed: (windows: multislider-0.8/bin/multislider.bat 192.168.1.1, linux: ./multislider-0.8/bin/multislider 192.168.1.1) - IP should be adjusted.  
The following ports are used by multiplayer client: TCP:8800(outbound) - lobby, UDP:8800 (outbound) - list of rooms, 8700 (outbound) - race data.
Both TCP & UDP inbound ports are randomly set.  
Before starting a multiplayer game make sure there are no limitations in firewall for executable files (PowerslideRemake.exe) to create network connections and there are no ports being blacklisted in router/proxy.  
In case of issues there are several actions that could be performed:  
1. Ping server and make sure respond is correct.  
2. Run client and try to create room. Make sure Lobby UI has appeared. After UI has been drawn, open command line and list opened ports for application (windows: run cmd as administrator and execute: netstat -bna | find "PowerslideRemake") - there should be 1 connection listed (TCP).  
3. Invite your friend into created room and start race. After race has been started list opened ports again - there should be 2 connections opened (TCP, UDP).  

### License ###
The MIT License (MIT)  
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:  
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.  
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

