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

# Hack to make it work, otherwise INTERFACE_INCLUDE_DIRECTORIES will not be propagated
file(MAKE_DIRECTORY ${LIBPROTOC_LIB_INCLUDE})

add_library(libprotoc IMPORTED STATIC GLOBAL)
add_library(libprotobuf IMPORTED STATIC GLOBAL)
add_executable(protoc IMPORTED GLOBAL)
add_dependencies(libprotoc protobuf)

set_target_properties(protoc PROPERTIES
        "IMPORTED_LOCATION" "${LIBPROTOC_PREFIX_PATH}/bin/protoc"
)

set_target_properties(libprotoc PROPERTIES
        "IMPORTED_LOCATION" "${LIBPROTOC_BINARY_PATH}/libprotoc.a"
        "INTERFACE_INCLUDE_DIRECTORIES" "${LIBPROTOC_LIB_INCLUDE}"
)

set_target_properties(libprotobuf PROPERTIES
        "IMPORTED_LOCATION" "${LIBPROTOC_BINARY_PATH}/libprotobuf.a"
        "INTERFACE_INCLUDE_DIRECTORIES" "${LIBPROTOC_LIB_INCLUDE}"
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
ExternalProject_Add(grpc
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
        IMPORTED_LOCATION "${LIBGRPC_PREFIX_PATH}/lib/libgrpc++_unsecure.a"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBGRPC_PREFIX_PATH}/include"
        IMPORTED_LINK_INTERFACE_LIBRARIES "${LIBGRPC_PREFIX_PATH}/lib/libgrpc.a;${LIBGRPC_PREFIX_PATH}/lib/libgpr.a;${LIBGRPC_PREFIX_PATH}/lib/libaddress_sorting.a;${gRPCDownload_INSTALL_DIR}/c-ares/lib/libcares.a;${gRPCDownload_INSTALL_DIR}/zlib/lib/libz.a;pthread"
                                          
)
