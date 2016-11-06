cmake_minimum_required(VERSION 2.8)


project(MshToBin)

if(ANDROID)
    set(CMAKE_MODULE_PATH "$ENV{OGRE_ANDROID_HOME}/CMake/;${CMAKE_MODULE_PATH}")
else()
    set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
endif()
set(OGRE_STATIC true)

find_package(OGRE REQUIRED)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(../../ogre/src)

if(ANDROID)

    add_definitions(-std=c++11)
    
    include_directories( 
            ${OGRE_INCLUDE_DIRS}
            $ENV{OGRE_ANDROID_HOME}/include/OGRE/RenderSystems/GLES2
            $ENV{OGRE_HOME}/include/OGRE/Overlay
    )
else()
    include_directories( 
            ${OGRE_INCLUDE_DIRS}
            $ENV{OGRE_HOME}/include/OGRE/RenderSystems/GL
            $ENV{OGRE_HOME}/include/OGRE/Overlay
    )
endif()

file(GLOB sources_main src/Main.cpp)

list(APPEND sources ${sources_main})

add_executable(MshToBin ${sources})
