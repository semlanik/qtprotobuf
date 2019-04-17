cmake_minimum_required(VERSION 3.1)

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
    set(Protobuf_PROTOC_LIBRARY libprotoc)
    set(Protobuf_LIBRARIES "")
    add_subdirectory("${Protobuf_SOURCE_DIR}/cmake" ${Protobuf_BINARY_DIR})
endif()

# grpc target requires QT version not less than 5.11
# earlier versions do not provide required Http2DirectAttribute: https://doc.qt.io/qt-5/whatsnew511.html
find_package(Qt5 COMPONENTS Core Network Qml REQUIRED)
if (Qt5Core_VERSION VERSION_LESS "5.11.0")
    message(FATAL_ERROR "Required Qt version is 5.11+")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

add_subdirectory("src/protobuf")
add_subdirectory("src/grpc")
add_subdirectory("src/generator")

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

    add_subdirectory("tests")
endif()

if(MAKE_EXAMPLES)
    add_subdirectory("examples/addressbook")
endif()
