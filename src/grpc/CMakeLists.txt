project(QtGrpc VERSION ${QTPROTOBUF_PROJECT_VERSION} LANGUAGES CXX)

set(TARGET ${PROJECT_NAME})
set(TARGET_STATIC ${TARGET}Static)
set(TARGET_EXPORT ${TARGET}Targets)
set(TARGET_CONFIG ${TARGET}Config)

set(TARGET_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
set(TARGET_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
set(TARGET_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(GLOB SOURCES asyncreply.cpp
    abstractchannel.cpp
    http2channel.cpp
    abstractclient.cpp
    abstractcredentials.cpp
    sslcredentials.cpp
    insecurecredentials.cpp)

file(GLOB HEADERS asyncreply.h
    abstractchannel.h
    http2channel.h
    abstractclient.h
    abstractcredentials.h
    sslcredentials.h
    insecurecredentials.h)

set(INCLUDE_DIRS PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:${TARGET_INCLUDE_DIR}>)

add_library(${TARGET} SHARED ${SOURCES})
add_library(${QTPROTOBUF_COMMON_NAMESPACE}::${TARGET} ALIAS ${TARGET})
set_target_properties(${TARGET} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "${HEADERS}" OUTPUT_NAME ${TARGET})
target_include_directories(${TARGET} ${INCLUDE_DIRS})
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
    LIBRARY DESTINATION ${TARGET_LIB_DIR})

install(EXPORT ${TARGET_EXPORT} NAMESPACE ${QTPROTOBUF_COMMON_NAMESPACE}:: FILE ${TARGET_EXPORT}.cmake DESTINATION ${TARGET_CMAKE_DIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${TARGET_CONFIG}.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_CMAKE_DIR}/${TARGET_CONFIG}.cmake"
    INSTALL_DESTINATION "${TARGET_CMAKE_DIR}")
configure_package_config_file(
    "${TARGET_CONFIG}Tree.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_CONFIG}.cmake"
    INSTALL_DESTINATION "${TARGET_CMAKE_DIR}")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_CMAKE_DIR}/${TARGET_CONFIG}.cmake" DESTINATION "${TARGET_CMAKE_DIR}")

export(TARGETS ${TARGET} NAMESPACE ${QTPROTOBUF_COMMON_NAMESPACE}:: FILE ${TARGET_EXPORT}.cmake)
export(PACKAGE ${TARGET})
