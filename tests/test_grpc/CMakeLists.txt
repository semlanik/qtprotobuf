configure_file(test_driver.sh.in test_driver.sh @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testfile ${CMAKE_CURRENT_BINARY_DIR}/testfile COPYONLY)

set(TARGET qtgrpc_test)
set(GENERATED_HEADERS
    simplestringmessage.h
    testserviceclient.h
    blobmessage.h
#    testserviceserver.h
)

file(GLOB SOURCES main.cpp
#    servertest.cpp
    clienttest.cpp)

add_test_target(TARGET ${TARGET}
    GENERATED_HEADERS ${GENERATED_HEADERS}
    SOURCES ${SOURCES})

add_subdirectory(echoserver)

message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME qtgrpc_echo_test
         COMMAND test_driver.sh ./tests/test_grpc/qtgrpc_test ./tests/test_grpc/echoserver/echoserver
)
