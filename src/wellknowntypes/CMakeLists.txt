qt_protobuf_internal_add_library(ProtobufWellKnownTypes
    SOURCES
        dummy.cpp
    PUBLIC_INCLUDE_DIRECTORIES
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/QtProtobuf/google/protobuf>"
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::Qml
        ${QT_PROTOBUF_NAMESPACE}::Protobuf
)

function(add_wellknowntype TYPENAME)
    list(APPEND LOOKUP_DIRS ${QT_PROTOBUF_SOURCE_DIR}/3rdparty/grpc/third_party/protobuf/src)
    list(APPEND LOOKUP_DIRS ${Protobuf_INCLUDE_DIRS})
    foreach(INCLUDE_DIR ${LOOKUP_DIRS})
        file(GLOB PROTO_FILE ${INCLUDE_DIR}/google/protobuf/${TYPENAME}.proto)
        if(NOT "${PROTO_FILE}" STREQUAL "")
            message(STATUS "Add well-known type ${PROTO_FILE}")
            qtprotobuf_generate(TARGET ProtobufWellKnownTypes GENERATED_TARGET ${TYPENAME}
                OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated
                PROTO_FILES ${PROTO_FILE}
                PROTO_INCLUDES "-I\"${INCLUDE_DIR}\""
                QML FOLDER ${extra_type_libraries_options})
            target_include_directories(${TYPENAME} PRIVATE
                $<TARGET_PROPERTY:${QT_PROTOBUF_NAMESPACE}::ProtobufWellKnownTypes,INTERFACE_INCLUDE_DIRECTORIES>)
            get_target_property(GENERATED_PUBLIC_HEADER_PRIVATE ${TYPENAME} PUBLIC_HEADER)
            set(GENERATED_PUBLIC_HEADER "${GENERATED_PUBLIC_HEADER};${GENERATED_PUBLIC_HEADER_PRIVATE}" PARENT_SCOPE)
            break()
        endif()
    endforeach()
    if(NOT TARGET ${TYPENAME})
        message(FATAL_ERROR "Unable to add well-know type ${TYPENAME}")
    endif()
endfunction()

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

# TODO: Check if 3rdparty protobuf module is initialized
if(EXISTS "${QT_PROTOBUF_SOURCE_DIR}/3rdparty/grpc/third_party/protobuf/src")
    set_target_properties(ProtobufWellKnownTypes PROPERTIES
        PROTO_INCLUDES "-I\"${QT_PROTOBUF_SOURCE_DIR}/3rdparty/grpc/third_party/protobuf/src\"")
elseif(Protobuf_INCLUDE_DIRS)
    set_target_properties(ProtobufWellKnownTypes PROPERTIES
        PROTO_INCLUDES "-I\"${Protobuf_INCLUDE_DIRS}\"")
endif()
