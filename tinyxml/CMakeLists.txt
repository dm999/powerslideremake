cmake_minimum_required(VERSION 2.8)


project(TinyXML)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

file(GLOB sources_main src/*.cpp)
file(GLOB sources_main_h src/*.h)

list(APPEND sources ${sources_main})
list(APPEND sources ${sources_main_h})

#add_definitions(-DTIXML_USE_STL)

add_library(TinyXML STATIC ${sources})


