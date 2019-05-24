set(TARGET echoserver)

set(GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/simpletest.grpc.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/simpletest.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/testservice.grpc.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/testservice.pb.cc)
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

add_executable(${TARGET} main.cpp ${GENERATED_SOURCES})
target_link_libraries(${TARGET} gRPC::grpc++ protobuf::libprotobuf)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
if(MSVC)
   target_compile_definitions(${TARGET} PRIVATE _WIN32_WINNT=0x600 _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
endif()
file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/../proto/*.proto)
protobuf_generate_all(TARGET ${TARGET}
    GENERATED_SOURCES ${GENERATED_SOURCES}
    PROTO_FILES ${PROTO_FILES})
