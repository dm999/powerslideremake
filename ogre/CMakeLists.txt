# set env variables
# OGRE_HOME
# OPENAL
# MYGUI_HOME
#
# android
# OGRE_ANDROID_HOME
# OPENAL
# MYGUI_ANDROID_HOME
#
# make sure debug & release versions of libs available (/MDd, /MD compilled for MSVC)
# sample of naming
# Ogre: OgreMainStatic_d / OgreMainStatic

cmake_minimum_required(VERSION 2.8)

project(PowerslideRebuild)

option(PowerslideRebuild_OPENAL "OpenAL build" ON)
option(PowerslideRebuild_MULTIPLAYER "Multiplayer build" ON)

if(ANDROID)
    set(CMAKE_MODULE_PATH "$ENV{OGRE_ANDROID_HOME}/CMake/;${CMAKE_MODULE_PATH}")
else()
    set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
endif()

set(OGRE_STATIC true)

find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

file(GLOB sources_main src/*.cpp)
file(GLOB sources_main_h src/*.h)

file(GLOB sources_lua_main src/lua/*.cpp)
file(GLOB sources_lua_main_h src/lua/*.h)

if(PowerslideRebuild_OPENAL)
    message(STATUS "OpenAL enabled")
    file(GLOB sources_openal_main src/OpenAL/*.cpp)
    file(GLOB sources_openal_main_h src/OpenAL/*.h)
else()
    add_definitions(-DNO_OPENAL)
endif()

if(PowerslideRebuild_MULTIPLAYER)
    message(STATUS "Multiplayer enabled")
    file(GLOB sources_multiplayer_main src/multiplayer/*.cpp)
    file(GLOB sources_multiplayer_main_h src/multiplayer/*.h)
else()
    add_definitions(-DNO_MULTIPLAYER)
endif()

file(GLOB sources_loaders_main src/loaders/*.cpp)
file(GLOB sources_loaders_main_h src/loaders/*.h)

file(GLOB sources_logic_main src/gamelogic/*.cpp)
file(GLOB sources_logic_main_h src/gamelogic/*.h)

file(GLOB sources_customs_main src/customs/*.cpp)
file(GLOB sources_customs_main_h src/customs/*.h)

file(GLOB sources_pscar_main src/pscar/*.cpp)
file(GLOB sources_pscar_main_h src/pscar/*.h)

file(GLOB sources_listeners_main src/listeners/*.cpp)
file(GLOB sources_listeners_main_h src/listeners/*.h)

file(GLOB sources_tools_main src/tools/*.cpp)
file(GLOB sources_tools_main_h src/tools/*.h)

file(GLOB sources_mesh_main src/mesh/*.cpp)
file(GLOB sources_mesh_main_h src/mesh/*.h)

file(GLOB sources_modes_main src/gamemodes/*.cpp)
file(GLOB sources_modes_main_h src/gamemodes/*.h)

file(GLOB sources_ui_main src/ui/*.cpp)
file(GLOB sources_ui_main_h src/ui/*.h)

file(GLOB sources_includes_main_h src/includes/*.h)

if(WIN32)
    foreach( src_file ${sources_main} )
        message(STATUS ${src_file})
        set_source_files_properties(
            ${src_file}
            PROPERTIES
            COMPILE_FLAGS "/Yupcheader.h"
            )
    endforeach( src_file ${sources_main} )
    
    set_source_files_properties(src/pcheader.cpp
        PROPERTIES
        COMPILE_FLAGS "/Ycpcheader.h"
        )
    
    add_definitions(-Zm114)
endif()

list(APPEND sources ${sources_main})
list(APPEND sources ${sources_lua_main})
if(PowerslideRebuild_OPENAL)
    list(APPEND sources ${sources_openal_main})
endif()
if(PowerslideRebuild_MULTIPLAYER)
    list(APPEND sources ${sources_multiplayer_main})
endif()
list(APPEND sources ${sources_loaders_main})
list(APPEND sources ${sources_logic_main})
list(APPEND sources ${sources_customs_main})
list(APPEND sources ${sources_pscar_main})
list(APPEND sources ${sources_listeners_main})
list(APPEND sources ${sources_tools_main})
list(APPEND sources ${sources_mesh_main})
list(APPEND sources ${sources_modes_main})
list(APPEND sources ${sources_ui_main})
list(APPEND sources ${sources_main_h})
list(APPEND sources ${sources_lua_main_h})
if(PowerslideRebuild_OPENAL)
    list(APPEND sources ${sources_openal_main_h})
endif()
if(PowerslideRebuild_MULTIPLAYER)
    list(APPEND sources ${sources_multiplayer_main_h})
endif()
list(APPEND sources ${sources_loaders_main_h})
list(APPEND sources ${sources_logic_main_h})
list(APPEND sources ${sources_customs_main_h})
list(APPEND sources ${sources_pscar_main_h})
list(APPEND sources ${sources_listeners_main_h})
list(APPEND sources ${sources_tools_main_h})
list(APPEND sources ${sources_mesh_main_h})
list(APPEND sources ${sources_modes_main_h})
list(APPEND sources ${sources_ui_main_h})
list(APPEND sources ${sources_includes_main_h})


if(ANDROID)
    add_definitions(-std=c++11)
    
    include_directories( 
            ${OIS_INCLUDE_DIRS}
            ${OGRE_INCLUDE_DIRS}
            $ENV{OGRE_ANDROID_HOME}/include/OGRE/RenderSystems/GLES2
            $ENV{OGRE_HOME}/include/OGRE/Overlay
            $ENV{MYGUI_ANDROID_HOME}/include/MYGUI
            #${OGRE_RenderSystem_GL_INCLUDE_DIR}
            #${OGRE_SAMPLES_INCLUDEPATH}
            #${OGRE_Overlay_INCLUDE_DIRS}
    )
else()
    include_directories( 
            ${OIS_INCLUDE_DIRS}
            ${OGRE_INCLUDE_DIRS}
            $ENV{OGRE_HOME}/include/OGRE/RenderSystems/GL
            $ENV{OGRE_HOME}/include/OGRE/Overlay
            $ENV{MYGUI_HOME}/include/MYGUI
            #${OGRE_RenderSystem_GL_INCLUDE_DIR}
            #${OGRE_SAMPLES_INCLUDEPATH}
            #${OGRE_Overlay_INCLUDE_DIRS}
    )
endif()

include_directories ("../lua/src")
include_directories ("../simpleini/src")

include_directories(../bullet ../bulletextras)
include_directories(../ogrebullet/Collisions/include)
include_directories(../ogrebullet/Dynamics/include)

if(PowerslideRebuild_OPENAL)
    include_directories($ENV{OPENAL}/include)
    link_directories ($ENV{OPENAL}/lib)
endif()

if(PowerslideRebuild_MULTIPLAYER)
    link_directories (${PROJECT_SOURCE_DIR}/../lib/multislider)
endif()
include_directories(../include)
include_directories(../include/raknet)
include_directories(../tools/jsonxx/src)

if(ANDROID)
    link_directories ($ENV{OGRE_ANDROID_HOME}/lib)
    link_directories ($ENV{OGRE_ANDROID_HOME}/lib/OGRE)
    link_directories ($ENV{MYGUI_ANDROID_HOME}/lib)
else()
    link_directories ($ENV{OGRE_HOME}/lib/Debug)
    link_directories ($ENV{OGRE_HOME}/lib/Debug/opt)
    link_directories ($ENV{OGRE_HOME}/lib/Release)
    link_directories ($ENV{OGRE_HOME}/lib/Release/opt)
    link_directories ($ENV{MYGUI_HOME}/lib/Debug)
    link_directories ($ENV{MYGUI_HOME}/lib/Release)
endif()

if(WIN32)

    foreach(f ${sources})
        # Get the path of the file relative to ${CMAKE_HOME_DIRECTORY},
        # then alter it (not compulsory)
        file(RELATIVE_PATH SRCGR ${CMAKE_HOME_DIRECTORY} ${f})
        set(SRCGR "${SRCGR}")

        # Extract the folder, ie remove the filename part
        string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

        # Source_group expects \\ (double antislash), not / (slash)
        string(REPLACE / \\ SRCGR ${SRCGR})
        source_group("${SRCGR}" FILES ${f})
    endforeach()
    
    list(APPEND sources src/res/PowerslideHD_Engine.ico)
    list(APPEND sources src/res/PowerslideHD_Engine.rc2)
    list(APPEND sources src/res/PowerslideHD_Engine.rc)
    
    source_group("Resource Files" FILES src/res/PowerslideHD_Engine.ico)
    source_group("Resource Files" FILES src/res/PowerslideHD_Engine.rc2)
    source_group("Resource Files" FILES src/res/PowerslideHD_Engine.rc)
    

    add_executable( PowerslideRebuild WIN32 ${sources})
else()

    if(NOT ANDROID)
        add_executable( PowerslideRebuild ${sources})
    endif()
endif()



if(ANDROID)
    
    
    add_library( PowerslideRebuild SHARED ${sources})
    
    if(PowerslideRebuild_MULTIPLAYER)
        target_link_libraries(PowerslideRebuild MultiSlider)
    endif()
    
    target_link_libraries(PowerslideRebuild MyGUI.OgrePlatform)
    target_link_libraries(PowerslideRebuild MyGUIEngine)
    
    target_link_libraries(PowerslideRebuild Jsonxx)
    
    if(PowerslideRebuild_OPENAL)
        target_link_libraries(PowerslideRebuild openal)
    endif()
    
    target_link_libraries(PowerslideRebuild RenderSystem_GLES2Static OgreMainStatic OgreOverlayStatic EGL GLESv2 log android c m dl)
    target_link_libraries(PowerslideRebuild Plugin_ParticleFXStatic)
    target_link_libraries(PowerslideRebuild Plugin_OctreeSceneManagerStatic)
    target_link_libraries(PowerslideRebuild freetype)
    target_link_libraries(PowerslideRebuild FreeImage)
    target_link_libraries(PowerslideRebuild zlib)
    target_link_libraries(PowerslideRebuild zziplib)
    
    target_link_libraries(PowerslideRebuild Lua ${OIS_LIBRARIES})

    target_link_libraries(PowerslideRebuild OgreBullet BulletDynamics BulletCollision LinearMath ConvexDecomposition)
else()
    if(PowerslideRebuild_MULTIPLAYER)
        target_link_libraries(PowerslideRebuild debug MultiSlider_d)
        target_link_libraries(PowerslideRebuild optimized MultiSlider)
    endif()
    
    target_link_libraries(PowerslideRebuild debug MyGUI.OgrePlatform_d)
    target_link_libraries(PowerslideRebuild debug MyGUIEngine_d)
    target_link_libraries(PowerslideRebuild optimized MyGUI.OgrePlatform)
    target_link_libraries(PowerslideRebuild optimized MyGUIEngine)
    
    target_link_libraries(PowerslideRebuild Jsonxx)
    
    if(PowerslideRebuild_OPENAL)
        target_link_libraries(PowerslideRebuild openal32)
    endif()
    
    target_link_libraries(PowerslideRebuild general opengl32)
    target_link_libraries(PowerslideRebuild general glu32)
    
    target_link_libraries(PowerslideRebuild debug OgreMainStatic_d)
    target_link_libraries(PowerslideRebuild debug OgreOverlayStatic_d)
    target_link_libraries(PowerslideRebuild debug RenderSystem_GLStatic_d)
    target_link_libraries(PowerslideRebuild debug Plugin_ParticleFXStatic_d)
    target_link_libraries(PowerslideRebuild debug Plugin_OctreeSceneManagerStatic_d)
    target_link_libraries(PowerslideRebuild debug freetype2311_d)
    target_link_libraries(PowerslideRebuild debug FreeImaged)
    target_link_libraries(PowerslideRebuild debug zlibd)
    target_link_libraries(PowerslideRebuild debug zziplibd)
    
    

    target_link_libraries(PowerslideRebuild optimized OgreMainStatic)
    target_link_libraries(PowerslideRebuild optimized OgreOverlayStatic)
    target_link_libraries(PowerslideRebuild optimized RenderSystem_GLStatic)
    target_link_libraries(PowerslideRebuild optimized Plugin_ParticleFXStatic)
    target_link_libraries(PowerslideRebuild optimized Plugin_OctreeSceneManagerStatic)
    target_link_libraries(PowerslideRebuild optimized freetype2311)
    target_link_libraries(PowerslideRebuild optimized FreeImage)
    target_link_libraries(PowerslideRebuild optimized zlib)
    target_link_libraries(PowerslideRebuild optimized zziplib)
    
    target_link_libraries(PowerslideRebuild Lua ${OIS_LIBRARIES})

    target_link_libraries(PowerslideRebuild LinearMath)
    target_link_libraries(PowerslideRebuild BulletCollision)
    target_link_libraries(PowerslideRebuild BulletDynamics)
    target_link_libraries(PowerslideRebuild ConvexDecomposition)

    target_link_libraries(PowerslideRebuild OgreBullet)
endif()
