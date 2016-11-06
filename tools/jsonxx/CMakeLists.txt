
project(Jsonxx)

file(GLOB jsonxx_source ./src/*.cc ./src/*.h)
add_library(Jsonxx STATIC ${jsonxx_source})


