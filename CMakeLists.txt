cmake_minimum_required(VERSION 3.16)

project(goodok-dekstop)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-module.cmake)
include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-generate.cmake)

add_subdirectory(src)
