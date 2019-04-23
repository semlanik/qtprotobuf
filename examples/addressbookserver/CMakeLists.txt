find_package(Protobuf)

file(GLOB HEADERS ${TESTS_OUT_DIR}/*.h)
file(GLOB SOURCES main.cpp)
file(GLOB GENERATED_SOURCES addressbook.pb.cc addressbook.grpc.pb.cc)

set(ADDRESSBOOK_SERVER "address_servers")

add_executable(${ADDRESSBOOK_SERVER} ${SOURCES} ${GENERATED_SOURCES})

if(WIN32)
    include_directories(${GRPC_INCLUDE_PATHS} "/")
    set(GRPC_LIBRARIES "")
    target_link_libraries(${ADDRESSBOOK_SERVER} "${GRPC_LIBRARIES}/grpc++.lib")

    #Needs to set path to protobuf libraries
    set(PROTOBUF_INSTALATION_PATH "")
    set(PROTOBUF_LIBRARIES_PATH ${PROTOBUF_INSTALATION_PATH}/lib)
    target_link_libraries(${ADDRESSBOOK_SERVER} "${PROTOBUF_LIBRARIES_PATH}/libprotobuf.lib")
elseif(UNIX)
    target_link_libraries(${ADDRESSBOOK_SERVER} protobuf grpc grpc++)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cert.pem ${CMAKE_CURRENT_SOURCE_DIR}/key.pem DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
