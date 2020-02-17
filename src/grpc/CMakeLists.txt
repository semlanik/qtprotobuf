set(TARGET QtGrpc)
set(TARGET_STATIC ${TARGET}Static)
set(TARGET_EXPORT ${TARGET}Targets)
set(TARGET_CONFIG ${TARGET}Config)

set(TARGET_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET})
set(TARGET_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
set(TARGET_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(TARGET_BINDIR ${CMAKE_INSTALL_BINDIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(${QT_PROTOBUF_CMAKE_DIR}/Coverage.cmake)
include(${QT_PROTOBUF_CMAKE_DIR}/GenerateQtHeaders.cmake)
include(${QT_PROTOBUF_CMAKE_DIR}/QtProtobufCommon.cmake)

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

protobuf_generate_qt_headers(PUBLIC_HEADER ${HEADERS} COMPONENT ${TARGET})

if(QT_PROTOBUF_STATIC AND NOT WIN32)
    add_library(${TARGET} STATIC ${SOURCES})
else()
    add_library(${TARGET} SHARED ${SOURCES})
endif()

extract_qt_variable(QT_HOST_DATA)

target_compile_definitions(${TARGET} PRIVATE QT_BUILD_GRPC_LIB)

set_target_properties(${TARGET} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "${HEADERS};${GENERATED_PUBLIC_HEADER}" OUTPUT_NAME ${TARGET})
target_include_directories(${TARGET} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${QT_PROTOBUF_BINARY_DIR}/include/${TARGET}>
    $<INSTALL_INTERFACE:${TARGET_INCLUDE_DIR}>
)
target_link_libraries(${TARGET} PUBLIC ${QT_PROTOBUF_PROJECT}::QtProtobuf Qt5::Core Qt5::Network)
target_compile_features(${TARGET} PUBLIC cxx_std_14
                                         cxx_auto_type
                                         cxx_decltype
                                         cxx_final
                                         cxx_override
                                         cxx_nullptr
                                         cxx_lambdas
                                         cxx_func_identifier)

add_library(${QT_PROTOBUF_PROJECT}::${TARGET} ALIAS ${TARGET})

install(TARGETS ${TARGET}
    EXPORT ${TARGET_EXPORT} COMPONENT dev
    ARCHIVE DESTINATION ${TARGET_LIB_DIR} COMPONENT lib
    PUBLIC_HEADER DESTINATION ${TARGET_INCLUDE_DIR} COMPONENT dev
    LIBRARY DESTINATION ${TARGET_LIB_DIR} COMPONENT lib
    RUNTIME DESTINATION ${TARGET_BINDIR} COMPONENT lib)

install(EXPORT ${TARGET_EXPORT} NAMESPACE ${QT_PROTOBUF_PROJECT}:: FILE ${TARGET_EXPORT}.cmake DESTINATION ${TARGET_CMAKE_DIR} COMPONENT dev)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${TARGET_CONFIG}.cmake.in" "${QT_PROTOBUF_BINARY_DIR}/${TARGET_CONFIG}.cmake"
    INSTALL_DESTINATION "${TARGET_CMAKE_DIR}")

install(FILES "${QT_PROTOBUF_BINARY_DIR}/${TARGET_CONFIG}.cmake" DESTINATION "${TARGET_CMAKE_DIR}" COMPONENT dev)

export(TARGETS ${TARGET} NAMESPACE ${QT_PROTOBUF_PROJECT}:: FILE ${TARGET_EXPORT}.cmake)

configure_file("${QT_PROTOBUF_CMAKE_DIR}/gRPCLookup.cmake" "${QT_PROTOBUF_BINARY_DIR}/gRPCLookup.cmake" COPYONLY)
install(FILES "${QT_PROTOBUF_BINARY_DIR}/gRPCLookup.cmake" DESTINATION "${TARGET_CMAKE_DIR}" COMPONENT dev)

if(QT_PROTOBUF_STATIC) #extra config for .pri file in case if static build enabled
    set(QT_PROTOBUF_EXTRA_CONFIG "staticlib")
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/qt_lib_grpc.pri.in" "${QT_PROTOBUF_BINARY_DIR}/qt_lib_grpc.pri" @ONLY)
install(FILES "${QT_PROTOBUF_BINARY_DIR}/qt_lib_grpc.pri" DESTINATION "${QT_HOST_DATA}/mkspecs/modules" COMPONENT dev)

add_coverage_target(TARGET ${TARGET})
