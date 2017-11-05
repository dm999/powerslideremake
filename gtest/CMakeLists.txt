cmake_minimum_required(VERSION 2.8)


project(UT)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

file(GLOB sources_main src/gtest/src/gtest-all.cc src/*.cpp ../ogre/src/tools/Tools.cpp ../ogre/src/tools/OgreTools.cpp  ../ogre/src/tools/KochanekBartelsSpline.cpp ../ogre/src/tools/PSSpline.cpp)
file(GLOB sources_main_h include/gtest/*.h)

list(APPEND sources ${sources_main})
list(APPEND sources ${sources_main_h})

include_directories(include)
include_directories(src/gtest)
include_directories(../ogre/src)

if(ANDROID)
    include_directories( 
            ${OIS_INCLUDE_DIRS}
            ${OGRE_INCLUDE_DIRS}
            ${PowerslideRebuild_OGREHOME}/include/OGRE/RenderSystems/GLES2
            ${PowerslideRebuild_OGREHOME}/include/OGRE/Overlay
    )
            
    link_directories (${PowerslideRebuild_OGREHOME}/lib)
    link_directories (${PowerslideRebuild_OGREHOME}/lib/OGRE)

else()
    include_directories( 
            ${OIS_INCLUDE_DIRS}
            ${OGRE_INCLUDE_DIRS}
            ${PowerslideRebuild_OGREHOME}/include/OGRE/RenderSystems/GL
            ${PowerslideRebuild_OGREHOME}/include/OGRE/Overlay
    )
            
    link_directories (${PowerslideRebuild_OGREHOME}/lib)
    link_directories (${PowerslideRebuild_OGREHOME}/lib/OGRE)

    if(WIN32)
        link_directories (${PowerslideRebuild_OGREHOME}/lib/Debug)
        link_directories (${PowerslideRebuild_OGREHOME}/lib/Debug/opt)
        link_directories (${PowerslideRebuild_OGREHOME}/lib/Release)
        link_directories (${PowerslideRebuild_OGREHOME}/lib/Release/opt)
    endif()
endif()

add_executable(UT ${sources})

if(MSVC)
    add_definitions(/MP)
else()
    add_definitions(-std=c++11)
endif()

if(ANDROID)
    
    target_link_libraries(UT RenderSystem_GLES2Static OgreMainStatic OgreOverlayStatic EGL GLESv2 log android c m dl stdc++)
    target_link_libraries(UT Plugin_ParticleFXStatic)
    target_link_libraries(UT freetype)
    target_link_libraries(UT FreeImage)
    target_link_libraries(UT zlib)
    target_link_libraries(UT zziplib)
else() # Not Android

    if(WIN32)
       
        target_link_libraries(UT debug OgreMainStatic_d)
        target_link_libraries(UT debug OgreOverlayStatic_d)
        target_link_libraries(UT debug RenderSystem_GLStatic_d)
        target_link_libraries(UT debug Plugin_ParticleFXStatic_d)
        target_link_libraries(UT debug freetype_d)
        target_link_libraries(UT debug FreeImage_d)
        target_link_libraries(UT debug zlib_d)
        target_link_libraries(UT debug zziplib_d)
        
        

        target_link_libraries(UT optimized RenderSystem_GLStatic)
        target_link_libraries(UT optimized OgreMainStatic)
        target_link_libraries(UT optimized OgreOverlayStatic)
        target_link_libraries(UT optimized Plugin_ParticleFXStatic)
        target_link_libraries(UT optimized freetype)
        target_link_libraries(UT optimized FreeImage)
        target_link_libraries(UT optimized zlib)
        target_link_libraries(UT optimized zziplib)
        
        target_link_libraries(UT general opengl32)
        target_link_libraries(UT general glu32)
        
       
    else()# Not Windows Not Android (Ubuntu)

        target_link_libraries(UT RenderSystem_GLStatic OgreMainStatic OgreOverlayStatic)
        target_link_libraries(UT GL GLU X11 Xrandr Xt Xaw Xext m dl pthread)
        target_link_libraries(UT Plugin_ParticleFXStatic)
        target_link_libraries(UT freetype)
        target_link_libraries(UT FreeImage)
        target_link_libraries(UT zlib)
        target_link_libraries(UT zziplib)

    endif()
    
endif()


install(TARGETS UT DESTINATION .)