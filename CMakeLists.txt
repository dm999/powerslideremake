# set variables
# PowerslideRemake_OGREHOME
# PowerslideRemake_OPENALDIR (if PowerslideRemake_OPENAL set)

cmake_minimum_required(VERSION 2.8)

project(PowerslideRemake)

option(PowerslideRemake_OPENAL "OpenAL build" ON)
set(PowerslideRemake_OGREHOME "" CACHE PATH "")
set(PowerslideRemake_OPENALDIR "" CACHE PATH "")

set(CMAKE_MODULE_PATH "${PowerslideRemake_OGREHOME}/CMake/;${CMAKE_MODULE_PATH}")
set(CMAKE_MODULE_PATH "${PowerslideRemake_OGREHOME}/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT PowerslideRemake_OGREHOME STREQUAL "")
    STRING(REGEX REPLACE "\\\\" "/" PowerslideRemake_OGREHOME ${PowerslideRemake_OGREHOME})
endif()

if(NOT PowerslideRemake_OPENALDIR STREQUAL "")
    STRING(REGEX REPLACE "\\\\" "/" PowerslideRemake_OPENALDIR ${PowerslideRemake_OPENALDIR})
endif()

SET(ENV{OGRE_HOME} ${PowerslideRemake_OGREHOME})
set(OGRE_STATIC true)
find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_definitions(-DSFML_STATIC)

add_subdirectory (lua)
add_subdirectory (simpleini)
add_subdirectory (ogre)

add_subdirectory (sfml/src/SFML)

if(PowerslideRemake_OPENAL)
    add_subdirectory (ogg_vorbis)
endif()

add_subdirectory (tools/jsonxx)
add_subdirectory (multislider/client)

if(MSVC)
    #add_subdirectory (gtest)
    #add_subdirectory (tools/mshtobin)
    #add_subdirectory (tools/pfunpacker)
endif()
