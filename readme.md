# Readme #

### Goal ###

"Powerslide remake" project remakes the main features of the video game [Powerslide](https://en.wikipedia.org/wiki/Powerslide_(video_game)) - arcade racing in an open 3D environment, with advanced AI based on a neural network and multiplayer gaming. It has been implemented using a programmable rendering pipeline and is cross-platform (Windows, Linux, Android).

### Build ###
To build the project you need:  
- [CMake 2.8 or higher](https://cmake.org/)  
- C++03 compiler  
- [Ogre 1.9.0](http://www.ogre3d.org/)  
- [OpenAL](https://www.openal.org) (only if PowerslideRemake_OPENAL ON)  

The initial step before building the project is preparation of dependencies: you need to build Ogre (static, OpenGL render system), OpenAL (OpenAL Soft).  

To build the project you need to:  
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
- [Cinepak codec](https://multimedia.cx/mirror/cinepak.txt) explanation of Cinepak decoding by Dr. Tim Ferguson  
- [HKLUT](https://github.com/jasonli0707/hklut) HKLUT: Hundred-Kilobyte Lookup Tables  

The class diagram of the source code is presented in docs/ClassDiagram.graphml (use [yED](http://www.yworks.com/products/yed) to open)  

### Run ###
To run the built project you need to supply the original game assets (packed files - pf). Game assets can be taken from the original CD or bought at [GOG](https://www.gog.com/game/powerslide).  
You need to have: data.pf, gameshell.pf, store.pf (patch.pf is optional). Put them in the folder with the built application.  
**Important**: integrated graphics are not supported, please use a discrete video card on PC.  
For the Android version you need to store the .pf files in the <powerslide> folder of the smartphone's External Storage Directory (where DCIM is located).  
Prebuilt archives for Windows / Linux / Android (obsolete) are available in the files section ([ModDB](https://www.moddb.com/mods/powerslideremake/downloads)). To run on Linux make sure OpenAL is installed.  

### Music ###
Sounds/music support is available in the project. To enable sounds/music the project should be built with PowerslideRemake_OPENAL.
Put the original CD tracks compressed as .ogg (vorbis) files into the same directory as the .pf files. Naming convention for files:  
track_0.ogg - main theme  
track_2–8.ogg - other tracks {dam track, sand blaster, speedway, devil's elbow, mineshafted, freezer, urban brawl}  
track_1.ogg is not used, nor is track_9.ogg (extra track on the original CD).  

### Multiplayer ###
"Powerslide remake" supports multiplayer gaming through a web server. 
To run a multiplayer game, a web server should be deployed on the most powerful PC of a local network or on a global virtual/physical server with a public IP address.
A prebuilt web server is available in the files section ([ModDB](https://www.moddb.com/mods/powerslideremake/downloads/server1)).  
To start the web server the following command should be executed: (Windows: multislider-0.8/bin/multislider.bat 192.168.1.1, Linux: ./multislider-0.8/bin/multislider 192.168.1.1) - the IP should be adjusted.  
The following ports are used by the multiplayer client: TCP:8800 (outbound) - lobby, UDP:8800 (outbound) - list of rooms, 8700 (outbound) - race data.
Both TCP & UDP inbound ports are randomly set.  
Before starting a multiplayer game, make sure there are no firewall restrictions preventing the executable (PowerslideRemake.exe) from creating network connections and that no ports are blacklisted in the router/proxy.  
In case of issues there are several actions that could be performed:  
1. Ping the server and make sure the response is correct.  
2. Run the client and try to create a room. Make sure the Lobby UI has appeared. After the UI has been drawn, open a command line and list the open ports for the application (Windows: run cmd as administrator and execute: netstat -bna | find "PowerslideRemake") - there should be 1 connection listed (TCP).  
3. Invite your friend into the created room and start a race. After the race has been started, list the open ports again - there should be 2 open connections (TCP, UDP).  

### License ###
The MIT License (MIT)  
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:  
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.  
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

