set(TARGET QtProtobufWellKnownTypes)
set(TARGET_EXPORT ${TARGET}Targets)
set(TARGET_CONFIG ${TARGET}Config)

set(TARGET_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/QtProtobuf/google/protobuf)
set(TARGET_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
set(TARGET_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(TARGET_BINDIR ${CMAKE_INSTALL_BINDIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Qml REQUIRED)
find_package(Protobuf QUIET)
find_package(QtProtobufProject COMPONENTS Generator CONFIG REQUIRED)

set(CMAKE_POSITION_INDEPENDENT_CODE ${Qt5_POSITION_INDEPENDENT_CODE})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${QT_PROTOBUF_CMAKE_DIR}/Coverage.cmake)
include(${QT_PROTOBUF_CMAKE_DIR}/GenerateQtHeaders.cmake)

file(GLOB SOURCES
    dummy.cpp)

function(add_wellknowntype TYPENAME)
    list(APPEND LOOKUP_DIRS ${QT_PROTOBUF_SOURCE_DIR}/3rdparty/grpc/third_party/protobuf/src)
    list(APPEND LOOKUP_DIRS ${Protobuf_INCLUDE_DIRS})
    foreach(INCLUDE_DIR ${LOOKUP_DIRS})
        file(GLOB PROTO_FILE ${INCLUDE_DIR}/google/protobuf/${TYPENAME}.proto)
        if(NOT "${PROTO_FILE}" STREQUAL "")
            message(STATUS "Add well-known type ${PROTO_FILE}")
            qtprotobuf_generate(TARGET ${TARGET} GENERATED_TARGET ${TYPENAME}
                OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated
                PROTO_FILES ${PROTO_FILE}
                PROTO_INCLUDES "-I\"${INCLUDE_DIR}\""
                QML FOLDER)
            target_include_directories(${TYPENAME} PRIVATE
                $<TARGET_PROPERTY:${QT_PROTOBUF_PROJECT}::QtProtobufWellKnownTypes,INTERFACE_INCLUDE_DIRECTORIES>)
            get_target_property(GENERATED_PUBLIC_HEADER_PRIVATE ${TYPENAME} PUBLIC_HEADER)
            set(GENERATED_PUBLIC_HEADER "${GENERATED_PUBLIC_HEADER};${GENERATED_PUBLIC_HEADER_PRIVATE}" PARENT_SCOPE)
            break()
        endif()
    endforeach()
    if(NOT TARGET ${TYPENAME})
        message(FATAL_ERROR "Unable to add well-know type ${TYPENAME}")
    endif()
endfunction()

add_library(${TARGET} ${SOURCES})

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


target_compile_definitions(${TARGET} PRIVATE QT_BUILD_PROTOBUF_WELLKNOWNTYPES_LIB PUBLIC QT_PROTOBUF_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    QT_PROTOBUF_VERSION_MINOR=${PROJECT_VERSION_MINOR})

set_target_properties(${TARGET} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "${GENERATED_PUBLIC_HEADER}" OUTPUT_NAME ${TARGET}
    PROTO_INCLUDES "-I\"${QT_PROTOBUF_SOURCE_DIR}/3rdparty/grpc/third_party/protobuf/src\"")

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
    $<BUILD_INTERFACE:${QT_PROTOBUF_BINARY_DIR}/include/${TARGET}>
    $<INSTALL_INTERFACE:${TARGET_INCLUDE_DIR}>
    )

add_library(${QT_PROTOBUF_PROJECT}::${TARGET} ALIAS ${TARGET})

target_link_libraries(${TARGET} PUBLIC Qt5::Core Qt5::Qml ${QT_PROTOBUF_PROJECT}::QtProtobuf)

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

add_coverage_target(TARGET ${TARGET})
