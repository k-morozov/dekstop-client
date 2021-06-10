cmake_minimum_required(VERSION 3.16)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(appchat-client VERSION 0.1.$ENV{TRAVIS_BUILD_NUMBER})
else ()
    project(appchat-client VERSION 0.1.1)
endif ()

set(CONAN_DISABLE_CHECK_COMPILER TRUE)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-module.cmake)
include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-generate.cmake)

add_subdirectory(src)

if (NOT WIN32)
    set(CPACK_GENERATOR DEB)
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
    set(CPACK_PACKAGE_CONTACT morozov-kst@yandex.ru)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    include(CPack)
endif()
