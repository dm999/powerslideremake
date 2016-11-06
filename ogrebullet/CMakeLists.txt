cmake_minimum_required(VERSION 2.8)

project(OgreBullet)

if(ANDROID)
    set(CMAKE_MODULE_PATH "$ENV{OGRE_ANDROID_HOME}/CMake/;${CMAKE_MODULE_PATH}")
else()
    set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
endif()

set(OGRE_STATIC true)
find_package(OGRE REQUIRED)

set(INCLUDE_DIRS
        ../bullet
        ../bulletextras
        ${CMAKE_CURRENT_SOURCE_DIR}/Collisions/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/include)
        
if(ANDROID)
    include_directories( 
            ${OIS_INCLUDE_DIRS}
            ${OGRE_INCLUDE_DIRS}
            $ENV{OGRE_ANDROID_HOME}/include/OGRE/RenderSystems/GL
    )
            
    link_directories ($ENV{OGRE_ANDROID_HOME}/lib)
    link_directories ($ENV{OGRE_ANDROID_HOME}/lib/OGRE)

else()
    include_directories( 
            ${OIS_INCLUDE_DIRS}
            ${OGRE_INCLUDE_DIRS}
            $ENV{OGRE_HOME}/include/OGRE/RenderSystems/GL
    )
            
    link_directories ($ENV{OGRE_HOME}/lib/Debug)
    link_directories ($ENV{OGRE_HOME}/lib/Debug/opt)
    link_directories ($ENV{OGRE_HOME}/lib/Release)
    link_directories ($ENV{OGRE_HOME}/lib/Release/opt)
endif()

include_directories(${INCLUDE_DIRS})

link_directories(${OGRE_LIB_DIR})

file(GLOB sources_col Collisions/src/*.cpp)
file(GLOB sources_col_h Collisions/include/*.h)
file(GLOB sources_dyn Dynamics/src/*.cpp)
file(GLOB sources_dyn_h Dynamics/include/*.h)

if(WIN32)
    foreach( src_file ${sources_col} )
        set_source_files_properties(${src_file} PROPERTIES COMPILE_FLAGS "/YuOgreBulletCollisions.h" )
    endforeach( src_file ${sources_col} )
    
    set_source_files_properties(Collisions/src/OgreBulletCollisionsPrecompiled.cpp PROPERTIES COMPILE_FLAGS "/YcOgreBulletCollisions.h" )
        
    foreach( src_file ${sources_dyn} )
        set_source_files_properties( ${src_file} PROPERTIES COMPILE_FLAGS "/YuOgreBulletDynamics.h" )
    endforeach( src_file ${sources_dyn} )
    
    set_source_files_properties(Dynamics/src/OgreBulletDynamicsPrecompiled.cpp PROPERTIES COMPILE_FLAGS "/YcOgreBulletDynamics.h" )
    
    add_definitions(-Zm125)
endif()

list(APPEND sources ${sources_col})
list(APPEND sources ${sources_col_h})
list(APPEND sources ${sources_dyn})
list(APPEND sources ${sources_dyn_h})

if(MSVC)
    file(GLOB_RECURSE all_sources ./*.cpp ./*.h)
    
    foreach(f ${all_sources})
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
endif()

add_library(OgreBullet ${sources})


