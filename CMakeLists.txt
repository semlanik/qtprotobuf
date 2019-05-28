cmake_minimum_required(VERSION 3.1)

set(QTPROTOBUF_PROJECT_VERSION 0.1.0)
project(QtProtobufProject VERSION ${QTPROTOBUF_PROJECT_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)

find_package(Qt5 COMPONENTS Core Network Qml REQUIRED)
if (Qt5Core_VERSION VERSION_LESS "5.11.0")
    # grpc target requires QT version not less than 5.11
    # earlier versions do not provide required Http2DirectAttribute: https://doc.qt.io/qt-5/whatsnew511.html
    message(FATAL_ERROR "Required Qt version is 5.11+")
endif()

if (Qt5Core_VERSION VERSION_LESS "5.11.4" OR (Qt5Core_VERSION VERSION_GREATER "5.12.0" AND Qt5Core_VERSION VERSION_LESS "5.12.3"))
    message(WARNING "Qt version<5.11.4 and (v5.12.0>=version<v5.12.3 has a bug in http2 connection handling. Tests using http2 feature and examples will not work!")
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/3rdparty/grpc/CMakeLists.txt")
    message(STATUS "Found local gRPC sources directory. Perform all-in-one build")
    set(gRPC_BUILD_TESTS FALSE)
    set(protobuf_BUILD_TESTS FALSE)
    add_subdirectory("3rdparty/grpc")
    if(TARGET grpc AND TARGET grpc++ AND TARGET grpc_cpp_plugin)
        add_library(gRPC::grpc ALIAS grpc)
        add_library(gRPC::grpc++ ALIAS grpc++)
        add_executable(gRPC::grpc_cpp_plugin ALIAS grpc_cpp_plugin)
        set(gRPC_FOUND TRUE)
    endif()
else()
    include("cmake/ProtobufLookup.cmake")
    include("cmake/gRPCLookup.cmake")
endif()

set(QTPROTOBUF_COMMON_NAMESPACE QtProtobufProject)
set(GENERATOR_TARGET qtprotobufgen)
set(QTPROTOBUF_EXECUTABLE_INSTALL ${CMAKE_INSTALL_FULL_BINDIR}/${GENERATOR_TARGET})
set(QTPROTOBUF_EXECUTABLE $<TARGET_FILE:${GENERATOR_TARGET}>)

if(UNIX)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      # using Clang
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-pessimizing-move -Wno-mismatched-tags -Wno-unused-private-field")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
      # using GCC
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-error=deprecated-declarations")
    endif()
elseif(WIN32)
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/WX /W4")
    else()
        message(WARNING "Only MSVC compilers family are supported for build")
    endif()
endif()

if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif()
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("src/protobuf")
add_subdirectory("src/grpc")
add_subdirectory("src/generator")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/ProjectConfig.cmake.in" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

export(PACKAGE ${PROJECT_NAME})

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)

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

if(NOT gRPC_FOUND)
    message(WARNING "gRPC not found: some tests and examples cannot be built.")
    message(STATUS "Force disable test")
    set(MAKE_TESTS OFF)
    message(STATUS "Force disable examples")
    set(MAKE_EXAMPLES OFF)
endif()

if(MAKE_TESTS)
    find_package(GTest)
    if (UNIX AND NOT GTest_FOUND)
        include(cmake/DownloadProject.cmake)
        download_project(PROJ                GTest
            GIT_REPOSITORY      https://github.com/google/googletest.git
            GIT_TAG             v1.8.x
            )
        # uncomment if gmock is going to be needed
        set(BUILD_GMOCK OFF CACHE BOOL "Disable gmock build functionality" FORCE)
        add_subdirectory(${GTest_SOURCE_DIR} ${GTest_BINARY_DIR})
    endif()

    enable_testing()
    add_subdirectory("tests")
endif()

if(MAKE_EXAMPLES)
    add_subdirectory("examples")
endif()
