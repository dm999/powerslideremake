cmake_minimum_required(VERSION 3.5)


project(Lua)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

file(GLOB sources_main src/*.c)
file(GLOB sources_main_h src/*.h)

list(APPEND sources ${sources_main})
list(APPEND sources ${sources_main_h})


add_library(Lua STATIC ${sources})


