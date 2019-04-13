file(GLOB HEADERS ${TESTS_OUT_DIR}/*.h)
file(GLOB SOURCES main.cpp)
file(GLOB GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/*.cpp)

set(${ECHO_SERVER} "echoserver")

add_executable(${ECHO_SERVER} ${SOURCES} ${GENERATED_SOURCES})
add_custom_command(TARGET ${ECHO_SERVER} PRE_BUILD
    COMMAND ${Protobuf_PROTOC_EXECUTABLE} --plugin=protoc-gen-${PROJECT_NAME}=${CMAKE_BINARY_DIR}/qtprotobuf --qtprotobuf_out=${CMAKE_CURRENT_BINARY_DIR}/tests
    ${PROTO_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto/
    DEPENDS ${PROJECT_NAME} ${PROTO_FILES}
    COMMENT "Generating test headers")
