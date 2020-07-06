cmake_minimum_required(VERSION 3.6)

set(QT_PROTOBUF_VERSION 0.4.0)
set(QT_PROTOBUF_PROJECT QtProtobufProject)

project(${QT_PROTOBUF_PROJECT} VERSION ${QT_PROTOBUF_VERSION} LANGUAGES CXX)

# Conan support, optional
if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif()

include(GNUInstallDirs)

set(QT_PROTOBUF_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(QT_PROTOBUF_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(QT_PROTOBUF_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

find_package(Qt5 COMPONENTS Core Network Qml REQUIRED)

if(EXISTS "${QT_PROTOBUF_SOURCE_DIR}/3rdparty/microjson/CMakeLists.txt")
    set(MICROJSON_MAKE_TESTS OFF)
    add_subdirectory("${QT_PROTOBUF_SOURCE_DIR}/3rdparty/microjson" EXCLUDE_FROM_ALL)
    message(STATUS "Using microjson from 3rdparty")
endif()

if(Qt5Core_VERSION VERSION_LESS "5.12.4")
    # grpc target requires QT version not less than 5.12.4
    # earlier versions Http2DirectAttribute is broken: https://doc.qt.io/qt-5/whatsnew511.html
    #                                                  https://bugreports.qt.io/browse/QTBUG-74765
    message(FATAL_ERROR "Required Qt version is 5.12.4+")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/grpc/CMakeLists.txt")
    message(STATUS "Found local gRPC sources directory. Perform all-in-one build")
    set(gRPC_BUILD_TESTS FALSE)
    set(protobuf_BUILD_TESTS FALSE)
    add_subdirectory("3rdparty/grpc" EXCLUDE_FROM_ALL)
    if(TARGET grpc AND TARGET grpc++ AND TARGET grpc_cpp_plugin)
        add_library(gRPC::grpc ALIAS grpc)
        add_library(gRPC::grpc++ ALIAS grpc++)
        add_executable(gRPC::grpc_cpp_plugin ALIAS grpc_cpp_plugin)
        set(gRPC_FOUND TRUE)
    endif()
else()
    include("${QT_PROTOBUF_CMAKE_DIR}/ProtobufLookup.cmake")
    include("${QT_PROTOBUF_CMAKE_DIR}/gRPCLookup.cmake")
endif()

set(gtest_force_shared_crt OFF)
set(GENERATOR_TARGET qtprotobufgen)
set(PROTOBUF_QUICK_PLUGIN_NAME protobufquickplugin)
set(GRPC_QUICK_PLUGIN_NAME grpcquickplugin)

if(UNIX)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        # using Clang
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-pessimizing-move -Wno-mismatched-tags -Wno-unused-private-field -Wno-self-assign-overloaded")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # using GCC
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-error=deprecated-declarations")
    endif()
elseif(WIN32)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
#TODO: not all l4 warnings are fixed for MSVC. Enable options bellow
#      once fixed
#        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX /W4 /MD")
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /W4 /MD")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj /wd4005")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /wd4005")
    else()
        message(WARNING "Only MSVC compilers family are supported for build")
    endif()
endif()

if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif()
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/ProjectConfig.cmake.in" "${QT_PROTOBUF_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")
install(FILES "${QT_PROTOBUF_BINARY_DIR}/${PROJECT_NAME}Config.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    COMPONENT dev)

export(PACKAGE ${PROJECT_NAME})

add_subdirectory("src/protobuf")
add_subdirectory("src/grpc")
add_subdirectory("src/generator")
if(NOT WIN32)#TODO: There are linking issues with windows build of well-known types...
    add_subdirectory("src/wellknowntypes")
endif()
add_subdirectory("src/qttypes")

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
    find_program(DOXYGEN_EXECUTABLE doxygen)
endif()
if(DEFINED DOXYGEN_EXECUTABLE)
    message(STATUS "Using doxygen to generate documenation: ${DOXYGEN_EXECUTABLE}")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/customdoxygen.css ${CMAKE_CURRENT_BINARY_DIR}/customdoxygen.css COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/header.html ${CMAKE_CURRENT_BINARY_DIR}/header.html COPYONLY)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/TitilliumWeb.woff2 ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html/TitilliumWeb.woff2 COPYONLY)

    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif()

set(QT_PROTOBUF_MAKE_TESTS ON CACHE BOOL "Enable QtProtobuf tests build")
set(QT_PROTOBUF_MAKE_EXAMPLES ON CACHE BOOL "Enable QtProtobuf examples build")
set(QT_PROTOBUF_MAKE_COVERAGE OFF CACHE BOOL "Enable QtProtobuf build for profiler (gcov)")
set(BUILD_SHARED_LIBS ON CACHE BOOL "Enable QtProtobuf static library build (disables shared library build)")

if(NOT gRPC_FOUND)
    message(WARNING "gRPC not found: some tests and examples cannot be built.")
    message(STATUS "Force disable test")
    set(QT_PROTOBUF_MAKE_TESTS OFF)
    message(STATUS "Force disable examples")
    set(QT_PROTOBUF_MAKE_EXAMPLES OFF)
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtProtobufCommon.cmake")

if(QT_PROTOBUF_MAKE_TESTS)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/googletest/CMakeLists.txt")
        set(BUILD_GMOCK OFF CACHE BOOL "Disable gmock build functionality" FORCE)
        set(gtest_force_shared_crt ON CACHE BOOL "Enable shared crt" FORCE)
        add_subdirectory("3rdparty/googletest" EXCLUDE_FROM_ALL)
        set(GTest_FOUND TRUE)
    else()
        find_package(GTest)
    endif()
    if(GTest_FOUND)
        enable_testing()
        add_subdirectory("tests")
    else()
        message(STATUS "Force disable test")
    endif()
endif()

if(QT_PROTOBUF_MAKE_EXAMPLES)
    add_subdirectory("examples")
endif()

include("${QT_PROTOBUF_CMAKE_DIR}/packaging.cmake")

