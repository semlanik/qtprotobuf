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

if(WIN32)
    include_directories(${GRPC_INCLUDE_PATHS} "/")
    set(GRPC_LIBRARIES "")
    #Needs to set path to protobuf libraries
    set(PROTOBUF_LIBRARIES_PATH ${PROTOBUF_INSTALATION_PATH}/lib)
    link_directories(${PROTOBUF_LIBRARIES_PATH} ${GRPC_LIBRARIES})
endif()

target_link_libraries(${TARGET} ${Protobuf_LIBRARIES} grpc++ grpc)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cert.pem ${CMAKE_CURRENT_SOURCE_DIR}/key.pem DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
