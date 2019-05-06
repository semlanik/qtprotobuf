# This script clones gRPC project and builds it with its dependencies
# In order to use it
# - copy DownloadProject.cmake, DownloadProject.CMakeLists.cmake.in and current file
#   into a separate folder outside the project.
# - create there a simple CMakeLists.txt with content:
# -------
#   cmake_minimum_required(VERSION 3.1)
#   project(gRPCTargetsBuilder)
#   include(gRPCBuild.cmake)
# -------
#
# -  from a build directory run "cmake <folder with created CMakeLists.txt>"
#
# In order to use the built gRPC project with qtprotobuf, please, specify CMAKE_PREFIX_PATH and other vars. E.g.
# PATH="<build-grpc>/grpc/bin:$PATH" cmake -DCMAKE_PREFIX_PATH="<build-grpc>/protobuf/lib/cmake;<build-grpc>/grpc/lib/cmake" -Dprotobuf_MODULE_COMPATIBLE=ON <qtprotobuf_CMakeLists_dir>

include(cmake/DownloadProject.cmake)
download_project(PROJ   gRPCDownload
                        GIT_REPOSITORY      https://github.com/grpc/grpc.git
                        GIT_TAG             v1.20.x
)
set(gRPCDownload_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})

include(ExternalProject)
# Builds c-ares project from the git submodule.
# Note: For all external projects, instead of using checked-out code, one could
# specify GIT_REPOSITORY and GIT_TAG to have cmake download the dependency directly,
# without needing to add a submodule to your project.
ExternalProject_Add(c-ares
  PREFIX c-ares
  SOURCE_DIR "${gRPCDownload_SOURCE_DIR}/third_party/cares/cares"
  CMAKE_CACHE_ARGS
        -DCARES_SHARED:BOOL=OFF
        -DCARES_STATIC:BOOL=ON
        -DCARES_STATIC_PIC:BOOL=ON
        -DCMAKE_INSTALL_PREFIX:PATH=${gRPCDownload_INSTALL_DIR}/c-ares
)

# Builds protobuf project from the git submodule.
ExternalProject_Add(protobuf
  PREFIX protobuf
  SOURCE_DIR "${gRPCDownload_SOURCE_DIR}/third_party/protobuf/cmake"
  CMAKE_CACHE_ARGS
        -Dprotobuf_BUILD_TESTS:BOOL=OFF
        -Dprotobuf_WITH_ZLIB:BOOL=OFF
        -Dprotobuf_MSVC_STATIC_RUNTIME:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${gRPCDownload_INSTALL_DIR}/protobuf
)
# Setup libprotobuf as a target
set(LIBPROTOC_PREFIX_PATH "${gRPCDownload_INSTALL_DIR}/protobuf")
set(LIBPROTOC_BINARY_PATH "${LIBPROTOC_PREFIX_PATH}/lib")
set(LIBPROTOC_LIB_INCLUDE "${LIBPROTOC_PREFIX_PATH}/include")

add_library(libprotoc IMPORTED STATIC GLOBAL)
add_library(libprotobuf IMPORTED STATIC GLOBAL)
add_executable(protoc IMPORTED GLOBAL)
add_dependencies(libprotoc protobuf)
add_dependencies(libprotobuf protobuf)
add_dependencies(protoc protobuf)

set_target_properties(protoc PROPERTIES
        IMPORTED_LOCATION "${LIBPROTOC_PREFIX_PATH}/bin/protoc"
)

set_target_properties(libprotoc PROPERTIES
        IMPORTED_LOCATION "${LIBPROTOC_BINARY_PATH}/libprotoc.a"
        INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${LIBPROTOC_LIB_INCLUDE}>
)

set_target_properties(libprotobuf PROPERTIES
        IMPORTED_LOCATION "${LIBPROTOC_BINARY_PATH}/libprotobuf.a"
        INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${LIBPROTOC_LIB_INCLUDE}>
)

# Builds zlib project from the git submodule.
ExternalProject_Add(zlib
  PREFIX zlib
  SOURCE_DIR "${gRPCDownload_SOURCE_DIR}/third_party/zlib"
  CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${gRPCDownload_INSTALL_DIR}/zlib
)

# the location where protobuf-config.cmake will be installed varies by platform
if (WIN32)
  set(_FINDPACKAGE_PROTOBUF_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf/cmake")
else()
  set(_FINDPACKAGE_PROTOBUF_CONFIG_DIR "${gRPCDownload_INSTALL_DIR}/protobuf/lib/cmake/protobuf")
endif()

# if OPENSSL_ROOT_DIR is set, propagate that hint path to the external projects with OpenSSL dependency.
set(_CMAKE_ARGS_OPENSSL_ROOT_DIR "")
if (OPENSSL_ROOT_DIR)
  set(_CMAKE_ARGS_OPENSSL_ROOT_DIR "-DOPENSSL_ROOT_DIR:PATH=${OPENSSL_ROOT_DIR}")
endif()

# Builds gRPC based on locally checked-out sources and set arguments so that all the dependencies
# are correctly located.
ExternalProject_Add(gRPC
  PREFIX grpc
  SOURCE_DIR "${gRPCDownload_SOURCE_DIR}"
  CMAKE_CACHE_ARGS
        -DgRPC_INSTALL:BOOL=ON
        -DgRPC_BUILD_TESTS:BOOL=OFF
        -DgRPC_PROTOBUF_PROVIDER:STRING=package
        -DgRPC_PROTOBUF_PACKAGE_TYPE:STRING=CONFIG
        -DProtobuf_DIR:PATH=${_FINDPACKAGE_PROTOBUF_CONFIG_DIR}
        -DgRPC_ZLIB_PROVIDER:STRING=package
        -DZLIB_ROOT:STRING=${gRPCDownload_INSTALL_DIR}/zlib
        -DgRPC_CARES_PROVIDER:STRING=package
        -Dc-ares_DIR:PATH=${gRPCDownload_INSTALL_DIR}/c-ares/lib/cmake/c-ares
        -DgRPC_SSL_PROVIDER:STRING=package
        ${_CMAKE_ARGS_OPENSSL_ROOT_DIR}
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/grpc
  DEPENDS c-ares protobuf zlib
)
set(LIBGRPC_PREFIX_PATH "${gRPCDownload_INSTALL_DIR}/grpc")

add_executable(grpc_cpp_plugin IMPORTED GLOBAL)
set_target_properties(grpc_cpp_plugin PROPERTIES
        "IMPORTED_LOCATION" "${LIBGRPC_PREFIX_PATH}/bin/grpc_cpp_plugin"
)

add_library(grpc++ IMPORTED STATIC GLOBAL)
set_target_properties(grpc++ PROPERTIES
        IMPORTED_LOCATION "${LIBGRPC_PREFIX_PATH}/lib/libgrpc++.a"
        INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${LIBGRPC_PREFIX_PATH}/include>
        IMPORTED_LINK_INTERFACE_LIBRARIES "${LIBGRPC_PREFIX_PATH}/lib/libgrpc.a;${LIBGRPC_PREFIX_PATH}/lib/libgpr.a;${LIBGRPC_PREFIX_PATH}/lib/libaddress_sorting.a;${gRPCDownload_INSTALL_DIR}/c-ares/lib/libcares.a;${gRPCDownload_INSTALL_DIR}/zlib/lib/libz.a;ssl;crypto;pthread"
)

add_library(grpc IMPORTED STATIC GLOBAL)
set_target_properties(grpc PROPERTIES
        IMPORTED_LOCATION "${LIBGRPC_PREFIX_PATH}/lib/libgrpc.a"
)
add_dependencies(grpc_cpp_plugin gRPC)
add_dependencies(grpc++ gRPC)
add_dependencies(grpc gRPC)
