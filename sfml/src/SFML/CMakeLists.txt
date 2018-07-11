include(${PROJECT_SOURCE_DIR}/sfml/cmake/Config.cmake)

# sfml-system
add_subdirectory(System)

if(PowerslideRemake_OPENAL)
    # sfml-audio
    add_subdirectory(Audio)
endif()

# sfml-network
add_subdirectory(Network) 
