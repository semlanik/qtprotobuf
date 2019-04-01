cmake_minimum_required(VERSION 3.0)

project(qtprotobuf)

find_package(Protobuf)
if (NOT Protobuf_FOUND OR Protobuf_VERSION VERSION_LESS "3.0.0")
    include(cmake/DownloadProject.cmake)
    download_project(PROJ                Protobuf
                     GIT_REPOSITORY      https://github.com/protocolbuffers/protobuf.git
                     GIT_TAG             3.7.x
    )
    set(protobuf_BUILD_TESTS OFF CACHE BOOL "Disable protobuf tests build" FORCE)
    set(Protobuf_PROTOC_EXECUTABLE protoc)
    add_subdirectory("${Protobuf_SOURCE_DIR}/cmake" ${Protobuf_BINARY_DIR})
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
elseif(WIN32)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall" "-Werror")
endif()

if(DEFINED $ENV{MAKE_TESTS})
    set(MAKE_TESTS ${MAKE_TESTS})
elseif(NOT DEFINED MAKE_TESTS)
    set(MAKE_TESTS ON)
endif()

add_subdirectory("src/protobuf")
add_subdirectory("src/grpc")
add_subdirectory("src/generator")

message(STATUS "QTPROTOBUF_GENERATOR_EXEC_PATH: ${QTPROTOBUF_GENERATOR_EXEC_PATH}")

if(MAKE_TESTS)
    set(testgeneration "test_generation")
    set(TESTS_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/tests)
    file(MAKE_DIRECTORY ${TESTS_OUT_DIR})
    add_custom_target(${testgeneration})

    if(WIN32)
        file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto/*.proto)
        add_custom_command(TARGET ${testgeneration} POST_BUILD
            COMMAND ${PROTOBUF_INSTALATION_PATH}/bin/protoc.exe --plugin=protoc-gen-${PROJECT_NAME}=${QTPROTOBUF_GENERATOR_EXEC_PATH} --qtprotobuf_out=${CMAKE_CURRENT_BINARY_DIR}/tests -I=${CMAKE_CURRENT_SOURCE_DIR}/tests/proto/
            ${PROTO_FILES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto/
            DEPENDS ${PROJECT_NAME} ${PROTO_FILES}
            COMMENT "Generating test headers"
            )
    elseif(UNIX)
        file(GLOB PROTO_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto/*.proto)
        add_custom_command(TARGET ${testgeneration}
            COMMAND ${Protobuf_PROTOC_EXECUTABLE} --plugin=protoc-gen-${PROJECT_NAME}=${QTPROTOBUF_GENERATOR_EXEC_PATH} --qtprotobuf_out=${CMAKE_CURRENT_BINARY_DIR}/tests
            ${PROTO_FILES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto/
            DEPENDS ${PROJECT_NAME} ${PROTO_FILES}
            COMMENT "Generating test headers"
            )
    endif()

    add_dependencies(${testgeneration} ${PROJECT_NAME})
    add_subdirectory("tests")
endif()
