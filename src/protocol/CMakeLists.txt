cmake_minimum_required(VERSION 3.5)

# GENERATE PROTOBUF FILES
set(PROTOBUF_FILES
  include/protocol/proto/messages.proto
  include/protocol/proto/request.proto
  include/protocol/proto/response.proto
  include/protocol/proto/datetime.proto
)

find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOBUF_FILES})
protobuf_generate_python(PROTO_PY ${PROTOBUF_FILES})
add_custom_target(protobuf_python ALL DEPENDS ${PROTO_PY})

add_custom_command(TARGET protobuf_python POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${PROTO_PY} ${CMAKE_SOURCE_DIR}/src/py_tests
)


set(PROTOCOL_LIB_NAME protocol_lib)
add_library(${PROTOCOL_LIB_NAME} STATIC)

target_sources(${PROTOCOL_LIB_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/protocol/protocol.h
    PRIVATE
        ${PROTO_HDRS}
        ${PROTO_SRCS}
        "include/protocol/command_table.h"
        "include/protocol/datetime.h" include/protocol/datetime.cpp
        "include/protocol/msgfactory.h" include/protocol/msgfactory.cpp
)

set_target_properties(${PROTOCOL_LIB_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_compile_options(${PROTOCOL_LIB_NAME} PRIVATE "-fPIC")
target_link_libraries(${PROTOCOL_LIB_NAME}
    PUBLIC
        protobuf
    PRIVATE
        pthread
        project_sanitizers
)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

target_include_directories(${PROTOCOL_LIB_NAME}
    PUBLIC
        ./include
        ${CMAKE_BINARY_DIR}/src/protocol
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS ${PROTOCOL_LIB_NAME} DESTINATION /usr/local/lib)


