set(TARGET simplechatserver)

set(GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/simplechat.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/simplechat.grpc.pb.cc)
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

add_executable(${TARGET} main.cpp ${GENERATED_SOURCES})
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # using GCC
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=array-bounds")
    endif()
endif()

file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/../simplechat/proto/*.proto)
qt_protobuf_internal_protobuf_generate_all(TARGET ${TARGET}
    GENERATED_SOURCES ${GENERATED_SOURCES}
    PROTO_FILES ${PROTO_FILES})

target_link_libraries(${TARGET} PRIVATE protobuf::libprotobuf gRPC::grpc++ gRPC::grpc)
if(MSVC)
   target_compile_definitions(${TARGET} PRIVATE _WIN32_WINNT=0x600 _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
endif()
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cert.pem ${CMAKE_CURRENT_SOURCE_DIR}/key.pem DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
set(SERVER_EXEC_PATH ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} PARENT_SCOPE)
