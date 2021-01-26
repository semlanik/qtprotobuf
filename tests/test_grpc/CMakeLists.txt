qt_protobuf_internal_find_dependencies()

# clients
qt_protobuf_internal_add_test(TARGET qtgrpc_test
    SOURCES clienttest.cpp QML)
qt_protobuf_internal_add_target_windeployqt(TARGET qtgrpc_test
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

qt_protobuf_internal_add_test(TARGET qtgrpc_secure_test
    SOURCES sslclienttest.cpp)
qt_protobuf_internal_add_target_windeployqt(TARGET qtgrpc_secure_test
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# servers
add_subdirectory(echoserver)
add_subdirectory(secureechoserver)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testfile ${CMAKE_CURRENT_BINARY_DIR}/testfile COPYONLY)

configure_file(${TEST_DRIVER_NAME}.in ${TEST_DRIVER_NAME} @ONLY)
add_test(NAME qtgrpc_echo_test
         COMMAND ${TEST_DRIVER_NAME} $<TARGET_FILE:qtgrpc_test> $<TARGET_FILE:echoserver> $<TARGET_FILE_NAME:qtgrpc_test> $<TARGET_FILE_NAME:echoserver>
)

add_test(NAME qtgrpc_secure_test
         COMMAND ${TEST_DRIVER_NAME} $<TARGET_FILE:qtgrpc_secure_test> $<TARGET_FILE:secureechoserver> $<TARGET_FILE_NAME:qtgrpc_secure_test> $<TARGET_FILE_NAME:secureechoserver>
)
