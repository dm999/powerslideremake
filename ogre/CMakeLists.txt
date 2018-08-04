# make sure debug & release versions of libs available (/MDd, /MD compilled for MSVC)
# sample of naming
# Ogre: OgreMainStatic_d / OgreMainStatic

cmake_minimum_required(VERSION 2.8)

#https://github.com/larsch/cmake-precompiled-header
include(cmake/PrecompiledHeader.cmake)

project(PowerslideRemake)

#version
set (PowerslideRemake_VERSION_MAJOR 1)
set (PowerslideRemake_VERSION_MINOR 18)
set (PowerslideRemake_VERSION_BUILD 0)
find_package(Git)
if(GIT_FOUND)
    execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE DESCRIBE_BUILD
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(SUBSTRING ${DESCRIBE_BUILD} 0 6 PowerslideRemake_VERSION_BUILD_SHA)
    message(STATUS "Git found, build " ${PowerslideRemake_VERSION_BUILD_SHA})
else()
    set (PowerslideRemake_VERSION_BUILD_SHA "N/A")
endif()
configure_file ("${PROJECT_SOURCE_DIR}/src/PowerslideRemakeConfig.h.in" "${PROJECT_BINARY_DIR}/PowerslideRemakeConfig.h")
configure_file ("${PROJECT_SOURCE_DIR}/src/res/PowerslideHD_Engine.rc.in" "${PROJECT_BINARY_DIR}/PowerslideHD_Engine.rc")
include_directories("${PROJECT_BINARY_DIR}")

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

file(GLOB PowerslideRemake_sources_main src/*.cpp)
file(GLOB PowerslideRemake_sources_main_h src/*.h)

file(GLOB PowerslideRemake_sources_lua_main src/lua/*.cpp)
file(GLOB PowerslideRemake_sources_lua_main_h src/lua/*.h)

if(PowerslideRemake_OPENAL)
    message(STATUS "OpenAL enabled")
    file(GLOB PowerslideRemake_sources_sound_main src/sound/*.cpp)
    file(GLOB PowerslideRemake_sources_sound_main_h src/sound/*.h)
else()
    add_definitions(-DNO_OPENAL)
endif()

file(GLOB PowerslideRemake_sources_video_main src/video/*.cpp)
file(GLOB PowerslideRemake_sources_video_main_h src/video/*.h)

file(GLOB PowerslideRemake_sources_multiplayer_main src/multiplayer/*.cpp)
file(GLOB PowerslideRemake_sources_multiplayer_main_h src/multiplayer/*.h)

file(GLOB PowerslideRemake_sources_loaders_main src/loaders/*.cpp)
file(GLOB PowerslideRemake_sources_loaders_main_h src/loaders/*.h)

file(GLOB PowerslideRemake_sources_logic_main src/gamelogic/*.cpp)
file(GLOB PowerslideRemake_sources_logic_main_h src/gamelogic/*.h)

file(GLOB PowerslideRemake_sources_customs_main src/customs/*.cpp)
file(GLOB PowerslideRemake_sources_customs_main_h src/customs/*.h)

file(GLOB PowerslideRemake_sources_pscar_main src/pscar/*.cpp)
file(GLOB PowerslideRemake_sources_pscar_main_h src/pscar/*.h)

file(GLOB PowerslideRemake_sources_physics_main src/physics/*.cpp)
file(GLOB PowerslideRemake_sources_physics_main_h src/physics/*.h)

file(GLOB PowerslideRemake_sources_listeners_main src/listeners/*.cpp)
file(GLOB PowerslideRemake_sources_listeners_main_h src/listeners/*.h)

file(GLOB PowerslideRemake_sources_tools_main src/tools/*.cpp)
file(GLOB PowerslideRemake_sources_tools_main_h src/tools/*.h)

file(GLOB PowerslideRemake_sources_mesh_main src/mesh/*.cpp)
file(GLOB PowerslideRemake_sources_mesh_main_h src/mesh/*.h)

file(GLOB PowerslideRemake_sources_modes_main src/gamemodes/*.cpp)
file(GLOB PowerslideRemake_sources_modes_main_h src/gamemodes/*.h)

file(GLOB PowerslideRemake_sources_cheats_main src/cheats/*.cpp)
file(GLOB PowerslideRemake_sources_cheats_main_h src/cheats/*.h)

file(GLOB PowerslideRemake_sources_ui_main src/ui/*.cpp)
file(GLOB PowerslideRemake_sources_ui_main_h src/ui/*.h)

file(GLOB PowerslideRemake_sources_ui_elements src/ui/elements/*.cpp)
file(GLOB PowerslideRemake_sources_ui_elements_h src/ui/elements/*.h)

file(GLOB PowerslideRemake_sources_includes_main_h src/includes/*.h)

list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_lua_main})
if(PowerslideRemake_OPENAL)
    list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_sound_main})
endif()
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_video_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_multiplayer_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_loaders_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_logic_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_customs_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_pscar_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_physics_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_listeners_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_tools_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_mesh_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_modes_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_cheats_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_ui_main})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_ui_elements})


if(MSVC)
    add_definitions(-Zm114)
    add_definitions(/MP)
    
    foreach( src_file ${PowerslideRemake_sources} )
        set_source_files_properties(
            ${src_file}
            PROPERTIES
            COMPILE_FLAGS "/Yupcheader.h /FIpcheader.h"
            )
    endforeach( src_file ${sources_main} )
    
    set_source_files_properties(src/pcheader.cpp
        PROPERTIES
        COMPILE_FLAGS "/Ycpcheader.h"
        )
else()
    add_definitions(-std=c++11)
endif()



list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_lua_main_h})
if(PowerslideRemake_OPENAL)
    list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_sound_main_h})
endif()
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_video_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_multiplayer_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_loaders_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_logic_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_customs_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_pscar_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_physics_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_listeners_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_tools_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_mesh_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_modes_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_cheats_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_ui_main_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_ui_elements_h})
list(APPEND PowerslideRemake_sources ${PowerslideRemake_sources_includes_main_h})


if(ANDROID)
    include_directories( 
            ${OIS_INCLUDE_DIRS}
            ${OGRE_INCLUDE_DIRS}
            ${PowerslideRemake_OGREHOME}/include/OGRE/RenderSystems/GLES2
            ${PowerslideRemake_OGREHOME}/include/OGRE/Overlay
    )
else()
    include_directories( 
            ${OIS_INCLUDE_DIRS}
            ${OGRE_INCLUDE_DIRS}
            ${PowerslideRemake_OGREHOME}/include/OGRE/RenderSystems/GL
            ${PowerslideRemake_OGREHOME}/include/OGRE/Overlay
    )
endif()

include_directories ("../lua/src")
include_directories ("../simpleini/src")
include_directories(../multislider/client/source)
include_directories(../tools/jsonxx/src)

include_directories (${PROJECT_SOURCE_DIR}/../sfml/include)

if(PowerslideRemake_OPENAL)
    include_directories(${PowerslideRemake_OPENALDIR}/include)
    link_directories (${PowerslideRemake_OPENALDIR}/lib)
endif()

link_directories (${PowerslideRemake_OGREHOME}/lib)
link_directories (${PowerslideRemake_OGREHOME}/lib/OGRE)

if(WIN32)
    link_directories (${PowerslideRemake_OGREHOME}/lib/Debug)
    link_directories (${PowerslideRemake_OGREHOME}/lib/Debug/opt)
    link_directories (${PowerslideRemake_OGREHOME}/lib/Release)
    link_directories (${PowerslideRemake_OGREHOME}/lib/Release/opt)
endif()

if(MSVC)

    foreach(f ${PowerslideRemake_sources})
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
    
    list(APPEND PowerslideRemake_sources src/res/PowerslideHD_Engine.ico)
    list(APPEND PowerslideRemake_sources src/res/PowerslideHD_Engine.rc2)
    list(APPEND PowerslideRemake_sources ${PROJECT_BINARY_DIR}/PowerslideHD_Engine.rc)
    
    source_group("Resource Files" FILES src/res/PowerslideHD_Engine.ico)
    source_group("Resource Files" FILES src/res/PowerslideHD_Engine.rc2)
    source_group("Resource Files" FILES src/res/PowerslideHD_Engine.rc)
    

    add_executable( PowerslideRemake WIN32 ${PowerslideRemake_sources})
else()

    if(MINGW)
    
        list(APPEND PowerslideRemake_sources src/res/PowerslideHD_Engine.ico)
        list(APPEND PowerslideRemake_sources src/res/PowerslideHD_Engine.rc2)
        list(APPEND PowerslideRemake_sources ${PROJECT_BINARY_DIR}/PowerslideHD_Engine.rc)
    
        set(CMAKE_RC_COMPILER_INIT windres)
        ENABLE_LANGUAGE(RC)
        set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -i <SOURCE> -o <OBJECT>")
    endif()

    if(NOT ANDROID)
        if(WIN32)
            add_executable( PowerslideRemake WIN32 ${PowerslideRemake_sources})
            add_precompiled_header(PowerslideRemake src/pcheader.h SOURCE_CXX src/pcheader.cpp FORCEINCLUDE)
        else()
            add_executable( PowerslideRemake ${PowerslideRemake_sources})
            add_precompiled_header(PowerslideRemake src/pcheader.h SOURCE_CXX src/pcheader.cpp FORCEINCLUDE)
        endif()
    endif()
endif()



if(ANDROID)
    
    
    add_library( PowerslideRemake SHARED ${PowerslideRemake_sources})
    add_precompiled_header(PowerslideRemake src/pcheader.h SOURCE_CXX src/pcheader.cpp FORCEINCLUDE)
    
    target_link_libraries(PowerslideRemake MultiSlider)
    target_link_libraries(PowerslideRemake Jsonxx)
    
    
    if(PowerslideRemake_OPENAL)
        target_link_libraries(PowerslideRemake openal)
    endif()
    
    target_link_libraries(PowerslideRemake RenderSystem_GLES2Static OgreMainStatic OgreOverlayStatic EGL GLESv2 log android c m dl)
    target_link_libraries(PowerslideRemake Plugin_ParticleFXStatic)
    target_link_libraries(PowerslideRemake freetype)
    target_link_libraries(PowerslideRemake FreeImage)
    target_link_libraries(PowerslideRemake zlib)
    target_link_libraries(PowerslideRemake zziplib)
    
    target_link_libraries(PowerslideRemake Lua OIS)

else() # Not Android

    if(WIN32)
        target_link_libraries(PowerslideRemake MultiSlider)
        target_link_libraries(PowerslideRemake Jsonxx)
        target_link_libraries(PowerslideRemake ws2_32)
        
        if(PowerslideRemake_OPENAL)
            target_link_libraries(PowerslideRemake openal32)
        endif()
        
       
        target_link_libraries(PowerslideRemake debug OgreMainStatic_d)
        target_link_libraries(PowerslideRemake debug OgreOverlayStatic_d)
        target_link_libraries(PowerslideRemake debug RenderSystem_GLStatic_d)
        target_link_libraries(PowerslideRemake debug Plugin_ParticleFXStatic_d)
        target_link_libraries(PowerslideRemake debug freetype_d)
        target_link_libraries(PowerslideRemake debug FreeImage_d)
        target_link_libraries(PowerslideRemake debug zlib_d)
        target_link_libraries(PowerslideRemake debug zziplib_d)
        
        

        target_link_libraries(PowerslideRemake optimized RenderSystem_GLStatic)
        target_link_libraries(PowerslideRemake optimized OgreMainStatic)
        target_link_libraries(PowerslideRemake optimized OgreOverlayStatic)
        target_link_libraries(PowerslideRemake optimized Plugin_ParticleFXStatic)
        target_link_libraries(PowerslideRemake optimized freetype)
        target_link_libraries(PowerslideRemake optimized FreeImage)
        target_link_libraries(PowerslideRemake optimized zlib)
        target_link_libraries(PowerslideRemake optimized zziplib)
        
        target_link_libraries(PowerslideRemake general opengl32)
        target_link_libraries(PowerslideRemake general glu32)
        
        target_link_libraries(PowerslideRemake Lua)
        
        target_link_libraries(PowerslideRemake debug OIS_d)
        target_link_libraries(PowerslideRemake optimized OIS)

    else()# Not Windows Not Android (Ubuntu)
    
        target_link_libraries(PowerslideRemake MultiSlider)
        target_link_libraries(PowerslideRemake Jsonxx)
        
        if(PowerslideRemake_OPENAL)
            target_link_libraries(PowerslideRemake openal32)
        endif()


        target_link_libraries(PowerslideRemake RenderSystem_GLStatic OgreMainStatic OgreOverlayStatic)
        target_link_libraries(PowerslideRemake GL GLU X11 Xrandr Xt Xaw Xext m dl pthread)
        target_link_libraries(PowerslideRemake Plugin_ParticleFXStatic)
        target_link_libraries(PowerslideRemake freetype)
        target_link_libraries(PowerslideRemake FreeImage)
        target_link_libraries(PowerslideRemake zlib)
        target_link_libraries(PowerslideRemake zziplib)


        
        target_link_libraries(PowerslideRemake Lua ${OIS_LIBRARIES})

    endif()
endif()

if(PowerslideRemake_OPENAL)
    target_link_libraries(PowerslideRemake sfml-audio)
endif()

target_link_libraries(PowerslideRemake sfml-network)

if(WIN32)
    install(FILES "${PowerslideRemake_OGREHOME}/lib/Debug/OIS_d.dll" CONFIGURATIONS Debug DESTINATION .)
    install(FILES "${PowerslideRemake_OGREHOME}/lib/Release/OIS.dll" CONFIGURATIONS Release DESTINATION .)
    install(FILES "${PowerslideRemake_OGREHOME}/bin/release/OIS.dll" CONFIGURATIONS Release DESTINATION .)
    install(TARGETS PowerslideRemake DESTINATION .)
else()
    if(ANDROID)
        install(TARGETS PowerslideRemake DESTINATION ./android/app/jniLibs/armeabi-v7a)
    else()
        install(TARGETS PowerslideRemake DESTINATION .)
    endif()
endif()


