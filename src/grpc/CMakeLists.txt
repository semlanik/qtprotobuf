set(TARGET QtGrpc)
set(TARGET_STATIC ${TARGET}Static)
set(TARGET_EXPORT ${TARGET}Targets)
set(TARGET_CONFIG ${TARGET}Config)

set(TARGET_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET})
set(TARGET_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
set(TARGET_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(TARGET_BIN_DIR ${CMAKE_INSTALL_BINDIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${QTPROTOBUF_CMAKE_DIR}/Coverage.cmake)
include(${QTPROTOBUF_CMAKE_DIR}/GenerateQtHeaders.cmake)

file(GLOB SOURCES qgrpcasyncoperationbase.cpp
    qgrpcasyncreply.cpp
    qgrpcsubscription.cpp
    qgrpcstatus.cpp
    qabstractgrpcchannel.cpp
    qgrpchttp2channel.cpp
    qabstractgrpcclient.cpp
    qgrpccredentials.cpp
    qgrpcsslcredentials.cpp
    qgrpcinsecurecredentials.cpp
    qgrpcuserpasswordcredentials.cpp)

file(GLOB HEADERS qgrpcasyncoperationbase_p.h
    qgrpcasyncreply.h
    qgrpcsubscription.h
    qgrpcstatus.h
    qabstractgrpcchannel.h
    qgrpchttp2channel.h
    qabstractgrpcclient.h
    qabstractgrpccredentials.h
    qgrpccredentials.h
    qgrpcsslcredentials.h
    qgrpcinsecurecredentials.h
    qgrpcuserpasswordcredentials.h
    qtgrpcglobal.h)

protobuf_generate_qt_headers(PUBLIC_HEADERS ${HEADERS} COMPONENT ${TARGET})

add_library(${TARGET} SHARED ${SOURCES})
target_compile_definitions(${TARGET} PRIVATE QT_BUILD_GRPC_LIB)

add_library(${QTPROTOBUF_COMMON_NAMESPACE}::${TARGET} ALIAS ${TARGET})
set_target_properties(${TARGET} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "${HEADERS};${GENERATED_PUBLIC_HEADERS}" OUTPUT_NAME ${TARGET})
target_include_directories(${TARGET} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${QTPROTOBUF_BINARY_DIR}/include/${TARGET}>
    $<INSTALL_INTERFACE:${TARGET_INCLUDE_DIR}>
)
target_link_libraries(${TARGET} PUBLIC ${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobuf Qt5::Core Qt5::Network)
target_compile_features(${TARGET} PUBLIC cxx_std_14
                                         cxx_auto_type
                                         cxx_decltype
                                         cxx_final
                                         cxx_override
                                         cxx_nullptr
                                         cxx_lambdas
                                         cxx_func_identifier)
install(TARGETS ${TARGET}
    EXPORT ${TARGET_EXPORT}
    ARCHIVE DESTINATION ${TARGET_LIB_DIR}
    PUBLIC_HEADER DESTINATION ${TARGET_INCLUDE_DIR}
    LIBRARY DESTINATION ${TARGET_LIB_DIR}
    RUNTIME DESTINATION ${TARGET_BIN_DIR})

install(EXPORT ${TARGET_EXPORT} NAMESPACE ${QTPROTOBUF_COMMON_NAMESPACE}:: FILE ${TARGET_EXPORT}.cmake DESTINATION ${TARGET_CMAKE_DIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${TARGET_CONFIG}.cmake.in" "${QTPROTOBUF_BINARY_DIR}/${TARGET_CONFIG}.cmake"
    INSTALL_DESTINATION "${TARGET_CMAKE_DIR}")

install(FILES "${QTPROTOBUF_BINARY_DIR}/${TARGET_CONFIG}.cmake" DESTINATION "${TARGET_CMAKE_DIR}")

export(TARGETS ${TARGET} NAMESPACE ${QTPROTOBUF_COMMON_NAMESPACE}:: FILE ${TARGET_EXPORT}.cmake)

configure_file("${QTPROTOBUF_CMAKE_DIR}/gRPCLookup.cmake" "${QTPROTOBUF_BINARY_DIR}/gRPCLookup.cmake" COPYONLY)
install(FILES "${QTPROTOBUF_BINARY_DIR}/gRPCLookup.cmake" DESTINATION "${TARGET_CMAKE_DIR}")

add_coverage_target(TARGET ${TARGET})
