# set env variables
# OPENAL

cmake_minimum_required(VERSION 2.8)


project(engsound)


set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

file(GLOB sources_main src/*.cpp )
file(GLOB sources_main_h src/*.h )

list(APPEND sources ${sources_main} ${sources_main_h})

include_directories($ENV{OPENAL}/include)


link_directories ($ENV{OPENAL}/lib)

add_executable(engsound ${sources})

target_link_libraries(engsound openal32)



