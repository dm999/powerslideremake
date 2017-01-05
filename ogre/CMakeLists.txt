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

project(PowerslideHD_Engine)

option(POWERSLIDE_OPENAL "OpenAL build" ON)

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

if(POWERSLIDE_OPENAL)
    message(STATUS "OpenAL enabled")
    file(GLOB sources_openal_main src/OpenAL/*.cpp)
    file(GLOB sources_openal_main_h src/OpenAL/*.h)
else()
    add_definitions(-DNO_OPENAL)
endif()

file(GLOB sources_multiplayer_main src/multiplayer/*.cpp)
file(GLOB sources_multiplayer_main_h src/multiplayer/*.h)

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
if(POWERSLIDE_OPENAL)
    list(APPEND sources ${sources_openal_main})
endif()
list(APPEND sources ${sources_multiplayer_main})
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
if(POWERSLIDE_OPENAL)
    list(APPEND sources ${sources_openal_main_h})
endif()
list(APPEND sources ${sources_multiplayer_main_h})
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

if(POWERSLIDE_OPENAL)
    include_directories($ENV{OPENAL}/include)
    link_directories ($ENV{OPENAL}/lib)
endif()

link_directories (${PROJECT_SOURCE_DIR}/../lib/multislider)
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
    

    add_executable( PowerslideHD_Engine WIN32 ${sources})
else()

    if(NOT ANDROID)
        add_executable( PowerslideHD_Engine ${sources})
    endif()
endif()



if(ANDROID)
    
    
    add_library( PowerslideHD_Engine SHARED ${sources})
    
    target_link_libraries(PowerslideHD_Engine MultiSlider)
    
    target_link_libraries(PowerslideHD_Engine MyGUI.OgrePlatform)
    target_link_libraries(PowerslideHD_Engine MyGUIEngine)
    
    target_link_libraries(PowerslideHD_Engine Jsonxx)
    
    if(POWERSLIDE_OPENAL)
        target_link_libraries(PowerslideHD_Engine openal)
    endif()
    
    target_link_libraries(PowerslideHD_Engine RenderSystem_GLES2Static OgreMainStatic OgreOverlayStatic EGL GLESv2 log android c m dl)
    target_link_libraries(PowerslideHD_Engine Plugin_ParticleFXStatic)
    target_link_libraries(PowerslideHD_Engine Plugin_OctreeSceneManagerStatic)
    target_link_libraries(PowerslideHD_Engine freetype)
    target_link_libraries(PowerslideHD_Engine FreeImage)
    target_link_libraries(PowerslideHD_Engine zlib)
    target_link_libraries(PowerslideHD_Engine zziplib)
    
    target_link_libraries(PowerslideHD_Engine Lua ${OIS_LIBRARIES})

    target_link_libraries(PowerslideHD_Engine OgreBullet BulletDynamics BulletCollision LinearMath ConvexDecomposition)
else()
    target_link_libraries(PowerslideHD_Engine debug MultiSlider_d)
    target_link_libraries(PowerslideHD_Engine optimized MultiSlider)
    
    target_link_libraries(PowerslideHD_Engine debug MyGUI.OgrePlatform_d)
    target_link_libraries(PowerslideHD_Engine debug MyGUIEngine_d)
    target_link_libraries(PowerslideHD_Engine optimized MyGUI.OgrePlatform)
    target_link_libraries(PowerslideHD_Engine optimized MyGUIEngine)
    
    target_link_libraries(PowerslideHD_Engine Jsonxx)
    
    if(POWERSLIDE_OPENAL)
        target_link_libraries(PowerslideHD_Engine openal32)
    endif()
    
    target_link_libraries(PowerslideHD_Engine general opengl32)
    target_link_libraries(PowerslideHD_Engine general glu32)
    
    target_link_libraries(PowerslideHD_Engine debug OgreMainStatic_d)
    target_link_libraries(PowerslideHD_Engine debug OgreOverlayStatic_d)
    target_link_libraries(PowerslideHD_Engine debug RenderSystem_GLStatic_d)
    target_link_libraries(PowerslideHD_Engine debug Plugin_ParticleFXStatic_d)
    target_link_libraries(PowerslideHD_Engine debug Plugin_OctreeSceneManagerStatic_d)
    target_link_libraries(PowerslideHD_Engine debug freetype2311_d)
    target_link_libraries(PowerslideHD_Engine debug FreeImaged)
    target_link_libraries(PowerslideHD_Engine debug zlibd)
    target_link_libraries(PowerslideHD_Engine debug zziplibd)
    
    

    target_link_libraries(PowerslideHD_Engine optimized OgreMainStatic)
    target_link_libraries(PowerslideHD_Engine optimized OgreOverlayStatic)
    target_link_libraries(PowerslideHD_Engine optimized RenderSystem_GLStatic)
    target_link_libraries(PowerslideHD_Engine optimized Plugin_ParticleFXStatic)
    target_link_libraries(PowerslideHD_Engine optimized Plugin_OctreeSceneManagerStatic)
    target_link_libraries(PowerslideHD_Engine optimized freetype2311)
    target_link_libraries(PowerslideHD_Engine optimized FreeImage)
    target_link_libraries(PowerslideHD_Engine optimized zlib)
    target_link_libraries(PowerslideHD_Engine optimized zziplib)
    
    target_link_libraries(PowerslideHD_Engine Lua ${OIS_LIBRARIES})

    target_link_libraries(PowerslideHD_Engine LinearMath)
    target_link_libraries(PowerslideHD_Engine BulletCollision)
    target_link_libraries(PowerslideHD_Engine BulletDynamics)
    target_link_libraries(PowerslideHD_Engine ConvexDecomposition)

    target_link_libraries(PowerslideHD_Engine OgreBullet)
endif()
