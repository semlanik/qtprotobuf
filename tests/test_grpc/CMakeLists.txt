include(${QTPROTOBUF_CMAKE_DIR}/QtProtobufTest.cmake)

# clients
add_test_target(TARGET qtgrpc_test
    SOURCES clienttest.cpp)
add_target_windeployqt(TARGET qtgrpc_test
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_test_target(TARGET qtgrpc_secure_test
    SOURCES sslclienttest.cpp)
add_target_windeployqt(TARGET qtgrpc_secure_test
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# servers
add_subdirectory(echoserver)
add_subdirectory(secureechoserver)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testfile ${CMAKE_CURRENT_BINARY_DIR}/testfile COPYONLY)

if(UNIX)
    set(TEST_DRIVER_NAME "test_driver.sh")
elseif(WIN32)
    set(TEST_DRIVER_NAME "test_driver.bat")
endif()

configure_file(${TEST_DRIVER_NAME}.in ${TEST_DRIVER_NAME} @ONLY)
add_test(NAME qtgrpc_echo_test
         COMMAND ${TEST_DRIVER_NAME} $<TARGET_FILE:qtgrpc_test> $<TARGET_FILE:echoserver> $<TARGET_FILE_NAME:qtgrpc_test> $<TARGET_FILE_NAME:echoserver>
)

add_test(NAME qtgrpc_secure_test
         COMMAND ${TEST_DRIVER_NAME} $<TARGET_FILE:qtgrpc_secure_test> $<TARGET_FILE:secureechoserver> $<TARGET_FILE_NAME:qtgrpc_secure_test> $<TARGET_FILE_NAME:secureechoserver>
)
