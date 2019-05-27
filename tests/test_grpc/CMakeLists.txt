configure_file(test_driver.sh.in test_driver.sh @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testfile ${CMAKE_CURRENT_BINARY_DIR}/testfile COPYONLY)

include(${CMAKE_SOURCE_DIR}/cmake/QtProtobufCommon.cmake)

set(GENERATED_HEADERS
    simplestringmessage.h
    testserviceclient.h
    blobmessage.h
#   testserviceserver.h
)

# clients
add_test_target(TARGET qtgrpc_test
    GENERATED_HEADERS ${GENERATED_HEADERS}
    SOURCES clienttest.cpp)
    
add_test_target(TARGET qtgrpc_secure_test
    GENERATED_HEADERS ${GENERATED_HEADERS}
    SOURCES sslclienttest.cpp)

# servers
add_subdirectory(echoserver)
add_subdirectory(secureechoserver)

add_test(NAME qtgrpc_echo_test
         COMMAND test_driver.sh ./tests/test_grpc/qtgrpc_test ./tests/test_grpc/echoserver/echoserver
)
