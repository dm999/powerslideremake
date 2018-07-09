cmake_minimum_required (VERSION 2.8)

project(MultiSlider)

#--------------------------------
# 3rdParty projects

include_directories (../../sfml/include)
include_directories(../../tools/jsonxx/src)


#--------------------------------

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SCL_SECURE_NO_WARNINGS")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

file(GLOB all_sources ./source/*.h ./source/*.cpp)

add_library(MultiSlider STATIC ${all_sources})

target_link_libraries(MultiSlider PRIVATE Jsonxx)
target_link_libraries(MultiSlider PRIVATE sfml-network)
