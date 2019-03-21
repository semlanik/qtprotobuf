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

add_executable(${PROJECT_NAME}
    src/generator/main.cpp
    src/generator/generator.cpp
    src/generator/classgeneratorbase.cpp
    src/generator/servergenerator.cpp
    src/generator/protobufclassgenerator.cpp
    src/generator/globalenumsgenerator.cpp
    src/generator/servicegeneratorbase.cpp
    src/generator/templates.cpp
)

if(WIN32)
    #Needs to set path to protobuf libraries
    set(PROTOBUF_INSTALATION_PATH "")
    include_directories(${PROTOBUF_INSTALATION_PATH}/include)

    set(PROTOBUF_LIBRARIES_PATH ${PROTOBUF_INSTALATION_PATH}/lib)
    target_link_libraries(${PROJECT_NAME} "${PROTOBUF_LIBRARIES_PATH}/libprotobuf.lib")
    target_link_libraries(${PROJECT_NAME} "${PROTOBUF_LIBRARIES_PATH}/libprotoc.lib")
elseif(UNIX)
    if(NOT Protobuf_FOUND)
        target_link_libraries(${PROJECT_NAME} libprotoc)
    else()
        target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Protobuf_PROTOC_LIBRARY})
    endif()
endif()

add_subdirectory("src/lib")

if(MAKE_TESTS)
    set(testgeneration "test_generation")
    set(TESTS_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/tests)
    file(MAKE_DIRECTORY ${TESTS_OUT_DIR})
    add_custom_target(${testgeneration})

    file(GLOB PROTO_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto/*.proto)

    if(WIN32)
        include_directories("src/lib")
        add_custom_command(TARGET ${testgeneration} POST_BUILD
            COMMAND ${PROTOBUF_INSTALATION_PATH}/bin/protoc.exe --plugin=protoc-gen-${PROJECT_NAME}=${CMAKE_BINARY_DIR}/qtprotobuf.exe --qtprotobuf_out=${CMAKE_CURRENT_BINARY_DIR}/tests
            ${PROTO_FILES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto/
            COMMENT "Generating test headers"
            )
    elseif(UNIX)
        add_custom_command(TARGET ${testgeneration} POST_BUILD
            COMMAND ${Protobuf_PROTOC_EXECUTABLE} --plugin=protoc-gen-${PROJECT_NAME}=${CMAKE_BINARY_DIR}/qtprotobuf --qtprotobuf_out=${CMAKE_CURRENT_BINARY_DIR}/tests
            ${PROTO_FILES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto/
            COMMENT "Generating test headers"
            )
    endif()

    add_dependencies(${testgeneration} ${PROJECT_NAME})
    add_subdirectory("tests")
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin)
