set(generated_sources
    ${CMAKE_CURRENT_BINARY_DIR}/simpletest.grpc.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/simpletest.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/testservice.grpc.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/testservice.pb.cc)
set_source_files_properties(${generated_sources} PROPERTIES GENERATED TRUE)

qt_internal_add_executable(echoserver
    SOURCES
        main.cpp
    LIBRARIES
       gRPC::grpc++
       protobuf::libprotobuf
    INCLUDE_DIRECTORIES
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_sources(echoserver PRIVATE ${generated_sources})
if(MSVC)
   target_compile_definitions(echoserver PRIVATE
       _WIN32_WINNT=0x600
       _SCL_SECURE_NO_WARNINGS
       _CRT_SECURE_NO_WARNINGS
       _WINSOCK_DEPRECATED_NO_WARNINGS
   )
endif()

file(GLOB proto_files ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/../proto/*.proto)
qt_internal_protobuf_reference_generate(TARGET echoserver
    GENERATED_SOURCES ${generated_sources}
    PROTO_FILES ${proto_files}
)
