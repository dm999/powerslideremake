cmake_minimum_required(VERSION 2.8)


project(PowerslideRebuild)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_subdirectory (lua)
add_subdirectory (tinyxml)
add_subdirectory (simpleini)
add_subdirectory (tools/jsonxx)
add_subdirectory (ogre)
add_subdirectory (ogrebullet)
add_subdirectory (bullet)
add_subdirectory (bulletextras)
if(NOT ANDROID)
    add_subdirectory (gtest)
    add_subdirectory (tools/mshtobin)
endif()
