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

file(GLOB PUBLIC_HEADERS
    qtprotobufwellknowntypes_global.qbp.h)

function(add_wellknowntype VARIABLE TYPENAME)
    list(APPEND LOOKUP_DIRS ${QTPROTOBUF_SOURCE_DIR}/3rdparty/grpc/third_party/protobuf/src)
    list(APPEND LOOKUP_DIRS ${Protobuf_INCLUDE_DIRS})
    foreach(INCLUDE_DIR ${LOOKUP_DIRS})
        file(GLOB PROTO_FILE ${INCLUDE_DIR}/google/protobuf/${TYPENAME}.proto)
        if (NOT "${PROTO_FILE}" STREQUAL "")
            message(STATUS "Add well-known type ${PROTO_FILE}")
            generate_qtprotobuf(TARGET ${TYPENAME}
                OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/google/protobuf
                PROTO_FILES ${PROTO_FILE}
                PROTO_INCLUDES -I${QTPROTOBUF_SOURCE_DIR}/3rdparty/grpc/third_party/protobuf/src
                QML TRUE)
            break()
        endif()
    endforeach()
    target_include_directories(${QtProtobuf_GENERATED} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated)
    if(WIN32)
        get_target_property(EXISTING_FLAGS ${TARGET} LINK_FLAGS)
        set_target_properties(${QtProtobuf_GENERATED} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
        set_target_properties(${TARGET} PROPERTIES LINK_DIRECTORIES "$<TARGET_FILE_DIR:${QtProtobuf_GENERATED}>")
        if("${EXISTING_FLAGS}" STREQUAL "EXISTING_FLAGS-NOTFOUND")
            set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/WHOLEARCHIVE:${QtProtobuf_GENERATED}")
        else()
            set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${EXISTING_FLAGS} /WHOLEARCHIVE:${QtProtobuf_GENERATED}")
        endif()
    else()
        set(${VARIABLE} ${${VARIABLE}} -Wl,--whole-archive ${QtProtobuf_GENERATED} -Wl,--no-whole-archive PARENT_SCOPE)
    endif()
    add_dependencies(${TARGET} ${QtProtobuf_GENERATED})
endfunction()

add_library(${TARGET} SHARED ${SOURCES})

add_wellknowntype(GENERATED_TARGETS any)
add_wellknowntype(GENERATED_TARGETS duration)
add_wellknowntype(GENERATED_TARGETS empty)
add_wellknowntype(GENERATED_TARGETS field_mask)
add_wellknowntype(GENERATED_TARGETS source_context)
add_wellknowntype(GENERATED_TARGETS struct)
add_wellknowntype(GENERATED_TARGETS timestamp)
add_wellknowntype(GENERATED_TARGETS wrappers)

add_wellknowntype(GENERATED_TARGETS type)
add_dependencies(type_qtprotobuf_gen any_qtprotobuf_gen source_context_qtprotobuf_gen)
add_wellknowntype(GENERATED_TARGETS api)
add_dependencies(api_qtprotobuf_gen type_qtprotobuf_gen source_context_qtprotobuf_gen)


protobuf_generate_qt_headers(PUBLIC_HEADERS ${PUBLIC_HEADERS} COMPONENT ${TARGET})

target_compile_definitions(${TARGET} PRIVATE QT_BUILD_PROTOBUF_WELLKNOWNTYPES_LIB PUBLIC QTPROTOBUF_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    QTPROTOBUF_VERSION_MINOR=${PROJECT_VERSION_MINOR})

add_library(${QTPROTOBUF_COMMON_NAMESPACE}::${TARGET} ALIAS ${TARGET})
set_target_properties(${TARGET} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "${PUBLIC_HEADERS};${GENERATED_PUBLIC_HEADERS}" OUTPUT_NAME ${TARGET}
    PROTO_INCLUDES -I${QTPROTOBUF_SOURCE_DIR}/3rdparty/grpc/third_party/protobuf/src)
set_target_properties(${TARGET} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
target_include_directories(${TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<BUILD_INTERFACE:${QTPROTOBUF_BINARY_DIR}/include/${TARGET}>
    $<INSTALL_INTERFACE:${TARGET_INCLUDE_DIR}>
    )

target_link_libraries(${TARGET} PUBLIC Qt5::Core Qt5::Qml ${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobuf PRIVATE ${GENERATED_TARGETS})
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

add_coverage_target(TARGET ${TARGET})
