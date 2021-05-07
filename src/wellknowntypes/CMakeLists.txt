qt_protobuf_internal_add_library(ProtobufWellKnownTypes
    SOURCES
        dummy.cpp
    INSTALL_INCLUDEDIR
         "${CMAKE_INSTALL_INCLUDEDIR}/${QT_PROTOBUF_NAMESPACE}Protobuf/google/protobuf"
    PUBLIC_INCLUDE_DIRECTORIES
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>"
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::Qml
        ${QT_PROTOBUF_NAMESPACE}::Protobuf
)

function(add_wellknowntype type_name)
    set(lookup_dirs "${QT_PROTOBUF_SOURCE_DIR}/3rdparty/grpc/third_party/protobuf/src"
        ${Protobuf_INCLUDE_DIRS}
    )
    foreach(dir ${lookup_dirs})
        file(GLOB PROTO_FILE "${dir}/google/protobuf/${type_name}.proto")
        if(NOT "${PROTO_FILE}" STREQUAL "")
            message(STATUS "Add well-known type ${PROTO_FILE}")
            qtprotobuf_generate(TARGET ProtobufWellKnownTypes
                GENERATED_TARGET ${type_name}
                OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated"
                PROTO_FILES "${PROTO_FILE}"
                PROTO_INCLUDES "-I\"${dir}\""
                QML
                FOLDER
                ${extra_type_libraries_options}
            )
            target_include_directories(${type_name} PRIVATE
                "$<TARGET_PROPERTY:ProtobufWellKnownTypes,INTERFACE_INCLUDE_DIRECTORIES>"
            )
            break()
        endif()
    endforeach()
    if(NOT TARGET ${type_name})
        message(FATAL_ERROR "Unable to add well-know type ${type_name}")
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
