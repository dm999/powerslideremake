# set variables
# PowerslideRebuild_OGREHOME
# PowerslideRebuild_OPENALDIR (if PowerslideRebuild_OPENAL set)

cmake_minimum_required(VERSION 2.8)

project(PowerslideRebuild)

option(PowerslideRebuild_OPENAL "OpenAL build" ON)
option(PowerslideRebuild_MULTIPLAYER "Multiplayer build" ON)
set(PowerslideRebuild_OGREHOME "" CACHE STRING "")
set(PowerslideRebuild_OPENALDIR "" CACHE STRING "")

set(CMAKE_MODULE_PATH "${PowerslideRebuild_OGREHOME}/CMake/;${CMAKE_MODULE_PATH}")
set(CMAKE_MODULE_PATH "${PowerslideRebuild_OGREHOME}/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")

set(OGRE_STATIC true)
find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})


add_subdirectory (lua)
add_subdirectory (simpleini)
add_subdirectory (tools/jsonxx)
add_subdirectory (ogre)
add_subdirectory (ogrebullet)
add_subdirectory (bullet)
add_subdirectory (bulletextras)
if(PowerslideRebuild_MULTIPLAYER)
    add_subdirectory (multislider/client)
endif()
if(NOT ANDROID)
    add_subdirectory (gtest)
    add_subdirectory (tools/mshtobin)
    add_subdirectory (tools/pfunpacker)
endif()
