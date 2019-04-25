if(NOT DEFINED gRPC_CPP_PLUGIN_EXECUTABLE)
    message(FATAL_ERROR "gRPC plugin is not found set full path to grpc_cpp_plugin using -DgRPC_CPP_PLUGIN_EXECUTABLE=</path/to/grpc_cpp_plugin>")
endif()

file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/../proto/*.proto)
add_custom_target(echoserver_src_generation)
add_custom_command(
    TARGET echoserver_src_generation
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I="${CMAKE_CURRENT_SOURCE_DIR}/../proto/"
    --plugin=protoc-gen-grpc="${gRPC_CPP_PLUGIN_EXECUTABLE}"
    ${PROTO_FILES}
    DEPENDS ${PROTO_FILES})
set(GENERATED_SOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/simpletest.grpc.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/simpletest.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/testservice.grpc.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/testservice.pb.cc")
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

add_executable(echoserver main.cpp ${GENERATED_SOURCES})
target_link_libraries(echoserver grpc++ ${Protobuf_LIBRARIES})
target_include_directories(echoserver PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_dependencies(echoserver echoserver_src_generation)
