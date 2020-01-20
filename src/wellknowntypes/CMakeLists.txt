set(TARGET QtProtobufWellKnownTypes)
set(TARGET_STATIC ${TARGET}Static)
set(TARGET_EXPORT ${TARGET}Targets)
set(TARGET_CONFIG ${TARGET}Config)

set(TARGET_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/QtProtobuf/google/protobuf)
set(TARGET_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
set(TARGET_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(TARGET_BIN_DIR ${CMAKE_INSTALL_BINDIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Qml REQUIRED)
find_package(Protobuf QUIET)

set(PROTO_PARSER ${QTPROTOBUF_BINARY_DIR}/parsemessages.go)

include(${QTPROTOBUF_CMAKE_DIR}/Coverage.cmake)
include(${QTPROTOBUF_CMAKE_DIR}/GenerateQtHeaders.cmake)
include(${QTPROTOBUF_CMAKE_DIR}/QtProtobufGen.cmake)

file(GLOB SOURCES
    timestamp.qpb.cpp)

file(GLOB HEADERS
    qtprotobufwellknowntypes_global.qbp.h)

file(GLOB PUBLIC_HEADER
    qtprotobufwellknowntypes_global.qbp.h)

function(add_wellknowntype TYPENAME)
    list(APPEND LOOKUP_DIRS ${QTPROTOBUF_SOURCE_DIR}/3rdparty/grpc/third_party/protobuf/src)
    list(APPEND LOOKUP_DIRS ${Protobuf_INCLUDE_DIRS})
    foreach(INCLUDE_DIR ${LOOKUP_DIRS})
        file(GLOB PROTO_FILE ${INCLUDE_DIR}/google/protobuf/${TYPENAME}.proto)
        if (NOT "${PROTO_FILE}" STREQUAL "")
            message(STATUS "Add well-known type ${PROTO_FILE}")
            qtprotobuf_generate(TARGET ${TARGET} GENERATED_TARGET ${TYPENAME}
                OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/google/protobuf
                PROTO_FILES ${PROTO_FILE}
                PROTO_INCLUDES -I${INCLUDE_DIR}
                QML)
            target_include_directories(${TYPENAME} PRIVATE
                $<TARGET_PROPERTY:${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobufWellKnownTypes,INTERFACE_INCLUDE_DIRECTORIES>)
            get_target_property(GENERATED_PUBLIC_HEADER_PRIVATE ${TYPENAME} PUBLIC_HEADER)
            set(GENERATED_PUBLIC_HEADER "${GENERATED_PUBLIC_HEADER};${GENERATED_PUBLIC_HEADER_PRIVATE}" PARENT_SCOPE)
            break()
        endif()
    endforeach()
endfunction()

if(QT_PROTOBUF_STATIC AND NOT WIN32)
    add_library(${TARGET} STATIC ${SOURCES})
else()
    add_library(${TARGET} SHARED ${SOURCES})
endif()

add_wellknowntype(any)
add_wellknowntype(duration)
add_wellknowntype(empty)
add_wellknowntype(field_mask)
add_wellknowntype(source_context)
add_wellknowntype(struct)
add_wellknowntype(timestamp)
add_wellknowntype(wrappers)

add_wellknowntype(type)
add_dependencies(type any source_context)
add_wellknowntype(api)
add_dependencies(api type source_context)


protobuf_generate_qt_headers(PUBLIC_HEADER ${PUBLIC_HEADER} COMPONENT ${TARGET})

target_compile_definitions(${TARGET} PRIVATE QT_BUILD_PROTOBUF_WELLKNOWNTYPES_LIB PUBLIC QTPROTOBUF_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    QTPROTOBUF_VERSION_MINOR=${PROJECT_VERSION_MINOR})

set_target_properties(${TARGET} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "${PUBLIC_HEADER};${GENERATED_PUBLIC_HEADER}" OUTPUT_NAME ${TARGET}
    PROTO_INCLUDES -I${QTPROTOBUF_SOURCE_DIR}/3rdparty/grpc/third_party/protobuf/src)

target_compile_features(${TARGET} PUBLIC cxx_std_14
    cxx_auto_type
    cxx_decltype
    cxx_final
    cxx_override
    cxx_nullptr
    cxx_lambdas
    cxx_func_identifier)

target_include_directories(${TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<BUILD_INTERFACE:${QTPROTOBUF_BINARY_DIR}/include/${TARGET}>
    $<INSTALL_INTERFACE:${TARGET_INCLUDE_DIR}>
    )

add_library(${QTPROTOBUF_COMMON_NAMESPACE}::${TARGET} ALIAS ${TARGET})

target_link_libraries(${TARGET} PUBLIC Qt5::Core Qt5::Qml ${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobuf)

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

add_coverage_target(TARGET ${TARGET})
