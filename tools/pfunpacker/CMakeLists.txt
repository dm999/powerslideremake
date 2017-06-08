cmake_minimum_required(VERSION 2.8)


project(PFUnpacker)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(../../ogre/src)
include_directories(src)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

file(GLOB PFUnpacker_sources_main src/Main.cpp)

list(APPEND PFUnpacker_sources ${PFUnpacker_sources_main})

add_executable(PFUnpacker ${PFUnpacker_sources})
