qt_internal_add_module(ProtobufWellKnownTypes
    SOURCES
        dummy.cpp
    # TODO Qt6: Need to specify the special install include directory for the wellknowntypes module
    # INSTALL_INCLUDEDIR
    #  "${CMAKE_INSTALL_INCLUDEDIR}/QtProtobuf/google/protobuf"
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Protobuf
)

qt_internal_extend_target(ProtobufWellKnownTypes
    CONDITION TARGET Qt::Qml
    LIBRARIES
        Qt::Qml
)

function(qt_internal_add_protobuf_wellknowntype type_name)
    # TODO Qt6: use WrapProtobuf's INTERFACE_INCLUDE_DIRECTORIES
    set(lookup_dirs ${Protobuf_INCLUDE_DIRS})

    set(qml_enabled "")
    if(TARGET Qt::Qml)
        set(qml_enabled QML)
    endif()
    foreach(dir ${lookup_dirs})
        file(GLOB PROTO_FILE "${dir}/google/protobuf/${type_name}.proto")
        if(NOT "${PROTO_FILE}" STREQUAL "")
            message(STATUS "Add well-known type ${PROTO_FILE}")
            qt6_protobuf_generate(TARGET ProtobufWellKnownTypes
                GENERATED_TARGET ${type_name}
                OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/proto_generated"
                PROTO_FILES "${PROTO_FILE}"
                PROTO_INCLUDES "-I\"${dir}\""
                ${qml_enabled}
                FOLDER
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

# TODO Qt6: The generated header files are not installed properly.
qt_internal_add_protobuf_wellknowntype(any)
qt_internal_add_protobuf_wellknowntype(duration)
qt_internal_add_protobuf_wellknowntype(empty)
qt_internal_add_protobuf_wellknowntype(field_mask)
qt_internal_add_protobuf_wellknowntype(source_context)
qt_internal_add_protobuf_wellknowntype(struct)
qt_internal_add_protobuf_wellknowntype(timestamp)
qt_internal_add_protobuf_wellknowntype(wrappers)
qt_internal_add_protobuf_wellknowntype(type)
add_dependencies(type any source_context)
qt_internal_add_protobuf_wellknowntype(api)
add_dependencies(api type source_context)

set_target_properties(ProtobufWellKnownTypes PROPERTIES
    QT_PROTO_INCLUDES "-I\"${Protobuf_INCLUDE_DIRS}\"")
set_property(TARGET ProtobufWellKnownTypes APPEND PROPERTY EXPORT_PROPERTIES QT_PROTO_INCLUDES)
