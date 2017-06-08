cmake_minimum_required(VERSION 2.8)


project(MshToBin)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(../../ogre/src)

if(ANDROID)

    add_definitions(-std=c++11)
    
    include_directories( 
            ${OGRE_INCLUDE_DIRS}
            ${PowerslideRebuild_OGREHOME}/include/OGRE/RenderSystems/GLES2
            ${PowerslideRebuild_OGREHOME}/include/OGRE/Overlay
    )
else()
    include_directories( 
            ${OGRE_INCLUDE_DIRS}
            ${PowerslideRebuild_OGREHOME}/include/OGRE/RenderSystems/GL
            ${PowerslideRebuild_OGREHOME}/include/OGRE/Overlay
    )
endif()

file(GLOB MshToBin_sources_main src/Main.cpp)

list(APPEND MshToBin_sources ${MshToBin_sources_main})

add_executable(MshToBin ${MshToBin_sources})
