qt_find_package(gRPC CONFIG)

# gRPC tests require reference gRPC and protobuf implementation
if(NOT TARGET gRPC::grpc_cpp_plugin
    OR NOT TARGET gRPC::grpc++
    OR NOT TARGET protobuf::libprotobuf
    OR NOT TARGET protobuf::protoc)
endif()

function(qt_internal_protobuf_reference_generate)
    set(options)
    set(oneValueArgs OUTPUT_DIRECTORY TARGET)
    set(multiValueArgs GENERATED_SOURCES PROTO_FILES)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(generated_target "${arg_TARGET}_generate")

    foreach(proto_file IN LISTS arg_PROTO_FILES)
        get_filename_component(dir "${proto_file}" DIRECTORY)
        list(APPEND proto_includes "-I${dir}")
    endforeach()

    if(NOT DEFINED arg_OUTPUT_DIRECTORY)
        set(output_directory "${CMAKE_CURRENT_BINARY_DIR}")
    else()
        set(output_directory "${arg_OUTPUT_DIRECTORY}")
    endif()

    file(MAKE_DIRECTORY "${OUTPUT_DIRECTORY}")

    if(NOT TARGET gRPC::grpc_cpp_plugin)
        message(FATAL_ERROR "gRPC plugin is not found")
    endif()

    add_custom_command(
        OUTPUT ${arg_GENERATED_SOURCES}
        COMMAND protobuf::protoc
        ARGS --grpc_out="${output_directory}"
            --cpp_out="${output_directory}"
            ${proto_includes}
            "--plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
            ${arg_PROTO_FILES}
        DEPENDS
            ${arg_PROTO_FILES}
            gRPC::grpc_cpp_plugin
    )

    add_custom_target(${generated_target} DEPENDS ${arg_PROTO_FILES} ${arg_GENERATED_SOURCES})
    add_dependencies(${arg_TARGET} ${generated_target})
endfunction()

qt_protobuf_generate(GENERATED_TARGET grpc_tests_common_gen
    PROTO_FILES
        proto/simpletest.proto
        proto/testservice.proto
    # TODO Qt6: Restore QML support
    # QML
)

add_subdirectory(echoserver)
qt_internal_add_test(grpc_client_basic
    SOURCES
        clienttest.cpp
    LIBRARIES
        Qt::Grpc
        Qt::Protobuf
    DEFINES
        QT_GRPC_SERVER_EXECUTABLE=\"$<TARGET_FILE:echoserver>\"
)
_qt_internal_link_protobuf_objects(grpc_client_basic grpc_tests_common_gen)

# TODO Qt6: Enable secure connection test later
#add_subdirectory(secureechoserver)
#qt_internal_add_test(grpc_client_secure
#    SOURCES
#        sslclienttest.cpp
#    LIBRARIES
#        Qt::Grpc
#        Qt::Protobuf
#    DEFINES
#        QT_GRPC_SERVER_EXECUTABLE=\"$<TARGET_FILE:secureechoserver>\"
#)
#_qt_internal_link_protobuf_objects(grpc_client_secure grpc_tests_common_gen)
