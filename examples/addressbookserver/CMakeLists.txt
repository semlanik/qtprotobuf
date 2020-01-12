set(TARGET addressbookserver)

include(${QTPROTOBUF_CMAKE_DIR}/QtProtobufCommon.cmake)

set(GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/addressbook.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/addressbook.grpc.pb.cc)
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

add_executable(${TARGET} main.cpp ${GENERATED_SOURCES})
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/../addressbook/proto/*.proto)
protobuf_generate_all(TARGET ${TARGET}
    GENERATED_SOURCES ${GENERATED_SOURCES}
    PROTO_FILES ${PROTO_FILES})

target_link_libraries(${TARGET} PRIVATE protobuf::libprotobuf gRPC::grpc++ gRPC::grpc)
if(MSVC)
   target_compile_definitions(${TARGET} PRIVATE _WIN32_WINNT=0x600 _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
endif()
add_custom_command(TARGET ${TARGET}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cert.pem ${CMAKE_CURRENT_SOURCE_DIR}/key.pem $<TARGET_FILE_DIR:${TARGET}>)
set(SERVER_EXEC_PATH ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} PARENT_SCOPE)
