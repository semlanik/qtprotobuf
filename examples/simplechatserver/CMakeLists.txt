set(TARGET simplechatserver)

include(${CMAKE_SOURCE_DIR}/cmake/QtProtobufCommon.cmake)

set(GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/simplechat.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/simplechat.grpc.pb.cc)
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

add_executable(${TARGET} main.cpp ${GENERATED_SOURCES})
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/../simplechat/proto/*.proto)
protobuf_generate_all(TARGET ${TARGET}
    GENERATED_SOURCES ${GENERATED_SOURCES}
    PROTO_FILES ${PROTO_FILES})

target_link_libraries(${TARGET} protobuf::libprotobuf gRPC::grpc++ gRPC::grpc)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cert.pem ${CMAKE_CURRENT_SOURCE_DIR}/key.pem DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
