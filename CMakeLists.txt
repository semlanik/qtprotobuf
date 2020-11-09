cmake_minimum_required(VERSION 3.6)

set(QT_PROTOBUF_VERSION 0.5.0)
set(QT_PROTOBUF_PROJECT QtProtobufProject)

project(${QT_PROTOBUF_PROJECT} VERSION ${QT_PROTOBUF_VERSION} LANGUAGES CXX)

# Conan support, optional
if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(QT_PROTOBUF_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(QT_PROTOBUF_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(QT_PROTOBUF_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

include("${QT_PROTOBUF_CMAKE_DIR}/dependencies.cmake")
include("${QT_PROTOBUF_CMAKE_DIR}/Coverage.cmake" EXCLUDE_FROM_ALL)

set(GENERATOR_TARGET qtprotobufgen)
set(PROTOBUF_QUICK_PLUGIN_NAME protobufquickplugin)
set(GRPC_QUICK_PLUGIN_NAME grpcquickplugin)

set(QT_PROTOBUF_MAKE_TESTS ON CACHE BOOL "Enable QtProtobuf tests build")
set(QT_PROTOBUF_MAKE_EXAMPLES ON CACHE BOOL "Enable QtProtobuf examples build")
set(QT_PROTOBUF_MAKE_COVERAGE OFF CACHE BOOL "Enable QtProtobuf build for profiler (gcov)")
set(QT_PROTOBUF_FIELD_ENUM OFF CACHE BOOL "Enable generation of enumeration with fields numbers for well-known and Qt types libraries")
set(BUILD_SHARED_LIBS ON CACHE BOOL "Enable QtProtobuf shared library build (disables static library build)")

if(CMAKE_CROSSCOMPILING)
    set(QT_PROTOBUF_MAKE_TESTS OFF)
    set(QT_PROTOBUF_MAKE_EXAMPLES OFF)
endif()

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

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/ProjectConfig.cmake.in" "${QT_PROTOBUF_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")
install(FILES "${QT_PROTOBUF_BINARY_DIR}/${PROJECT_NAME}Config.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    COMPONENT dev)
export(PACKAGE ${PROJECT_NAME})
add_subdirectory("src/generator")

add_subdirectory("src/protobuf")
add_subdirectory("src/grpc")

set(EXTRA_TYPE_LIBRARIES_OPTIONS "") #Prevent to set from outside
if(QT_PROTOBUF_FIELD_ENUM)
    set(EXTRA_TYPE_LIBRARIES_OPTIONS "FIELDENUM ${EXTRA_TYPE_LIBRARIES_OPTIONS}")
endif()

if(NOT WIN32)#TODO: There are linking issues with windows build of well-known types...
    add_subdirectory("src/wellknowntypes")
endif()
add_subdirectory("src/qttypes")

if(QT_PROTOBUF_MAKE_TESTS)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/googletest/CMakeLists.txt")
        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        add_subdirectory("3rdparty/googletest" EXCLUDE_FROM_ALL)
        set(GTest_FOUND TRUE)
    else()
        find_package(GTest)
    endif()
    if(GTest_FOUND)
        enable_testing()
        add_subdirectory("tests")
    else()
        message(STATUS "googletest not found. Force disable tests.")
    endif()
endif()

if(QT_PROTOBUF_MAKE_EXAMPLES)
    add_subdirectory("examples")
endif()

if(QT_PROTOBUF_INSTALL)
    include("${QT_PROTOBUF_CMAKE_DIR}/packaging.cmake")
endif()

include("${QT_PROTOBUF_CMAKE_DIR}/doxygen.cmake")
