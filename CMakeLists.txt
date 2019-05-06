cmake_minimum_required(VERSION 3.1)

set(QTPROTOBUF_PROJECT_VERSION 0.1.0)
project(QtProtobufProject VERSION ${QTPROTOBUF_PROJECT_VERSION} LANGUAGES CXX)
set(QTPROTOBUF_COMMON_NAMESPACE QtProtobufProject)

include(GNUInstallDirs)

find_package(Protobuf CONFIG)
if(NOT Protobuf_FOUND)
    find_package(Protobuf MODULE)
endif()

# FIXME: what is the reason to set these variables in root CMake?

set(GENERATOR_TARGET qtprotobufgen)
set(QTPROTOBUF_EXECUTABLE_INSTALL ${CMAKE_INSTALL_FULL_BINDIR}/${GENERATOR_TARGET})

find_program(gRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
if (gRPC_CPP_PLUGIN_EXECUTABLE STREQUAL gRPC_CPP_PLUGIN_EXECUTABLE-NOTFOUND)
    # if no approprite Protobuf installed, there is no appropriate version of gRPC neither
    # the next include provides both from sources
    include(cmake/gRPCBuild.cmake)
    
    # names of the variables are equal to those assigned by find_package(Protobuf)
    set(Protobuf_PROTOC_EXECUTABLE protoc)
    set(Protobuf_PROTOC_LIBRARY libprotoc)
    set(Protobuf_LIBRARIES libprotobuf)

    set(gRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
endif()

# grpc target requires QT version not less than 5.11
# earlier versions do not provide required Http2DirectAttribute: https://doc.qt.io/qt-5/whatsnew511.html
find_package(Qt5 COMPONENTS Core Network Qml REQUIRED)

if (Qt5Core_VERSION VERSION_LESS "5.11.0")
    message(FATAL_ERROR "Required Qt version is 5.11+")
endif()

if(UNIX)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      # using Clang
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-pessimizing-move -Wno-mismatched-tags -Wno-unused-private-field")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
      # using GCC
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
    endif()
elseif(WIN32)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall" "-Werror")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("src/protobuf")
add_subdirectory("src/grpc")
add_subdirectory("src/generator")

set(QTPROTOBUF_EXECUTABLE $<TARGET_FILE:${GENERATOR_TARGET}>)

if(DEFINED $ENV{MAKE_TESTS})
    set(MAKE_TESTS $ENV{MAKE_TESTS})
elseif(NOT DEFINED MAKE_TESTS)
    set(MAKE_TESTS ON)
endif()

if(DEFINED $ENV{MAKE_EXAMPLES})
    set(MAKE_EXAMPLES $ENV{MAKE_EXAMPLES})
elseif(NOT DEFINED MAKE_EXAMPLES)
    set(MAKE_EXAMPLES ON)
endif()

if(DEFINED $ENV{MAKE_COVERAGE})
    set(MAKE_COVERAGE $ENV{MAKE_COVERAGE})
elseif(NOT DEFINED MAKE_COVERAGE)
    set(MAKE_COVERAGE OFF)
endif()

if(MAKE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

if(MAKE_TESTS)
    find_package(GTest)
    if (UNIX AND NOT GTest_FOUND)
        if (NOT Protobuf_FOUND)
            # if prebuilt found protobuf then functionality to download GTEST is needed
            include(cmake/DownloadProject.cmake)
        endif()
        download_project(PROJ                GTest
            GIT_REPOSITORY      https://github.com/google/googletest.git
            GIT_TAG             v1.8.x
            )
        # uncomment if gmock is going to be needed
        set(BUILD_GMOCK OFF CACHE BOOL "Disable gmock build functionality" FORCE)
        add_subdirectory(${GTest_SOURCE_DIR} ${GTest_BINARY_DIR})
    endif()


    if(WIN32)
        #Needs to set path to protobuf libraries
        set(PROTOBUF_INSTALATION_PATH "")
        set(Protobuf_PROTOC_EXECUTABLE ${PROTOBUF_INSTALATION_PATH}/bin/protoc.exe)
        #Set  path to GTest build libraries
        set(GTEST_BOTH_LIBRARIES "")

        #Set path to GTest include directory
        include_directories(${GTEST_INCLUDE_PATHS} "/")
        link_directories(${GTEST_BOTH_LIBRARIES})
    endif()

    enable_testing()
    add_subdirectory("tests")
endif()

if(MAKE_EXAMPLES)
    add_subdirectory("examples/addressbook")
    add_subdirectory("examples/addressbookserver")
    add_subdirectory("examples/simplechat")
    add_subdirectory("examples/simplechatserver")
endif()
