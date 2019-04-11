find_package(Protobuf)

file(GLOB HEADERS ${TESTS_OUT_DIR}/*.h)
file(GLOB SOURCES main.cpp)
file(GLOB GENERATED_SOURCES addressbook.pb.cc addressbook.grpc.pb.cc)

set(ADDRESSBOOK_SERVER "address_servers")

add_executable(${ADDRESSBOOK_SERVER} ${SOURCES} ${GENERATED_SOURCES})
target_link_libraries(${ADDRESSBOOK_SERVER} protobuf grpc++)
