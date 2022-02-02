add_executable(qtgrpc_qml_test_grpc_http ${MOC_SOURCES} grpc_http.cpp ${QML_FILES})
add_executable(qtgrpc_qml_test_grpc_socket ${MOC_SOURCES} grpc_socket.cpp ${QML_FILES})
target_link_libraries(qtgrpc_qml_test_grpc_http
    PRIVATE
        ${QT_VERSIONED_PREFIX}::Qml
        ${QT_VERSIONED_PREFIX}::Quick
        ${QT_VERSIONED_PREFIX}::Test
        ${QT_VERSIONED_PREFIX}::QuickTest
        ${QT_PROTOBUF_NAMESPACE}::Grpc
)
target_link_libraries(qtgrpc_qml_test_grpc_socket
    PRIVATE
        ${QT_VERSIONED_PREFIX}::Qml
        ${QT_VERSIONED_PREFIX}::Quick
        ${QT_VERSIONED_PREFIX}::Test
        ${QT_VERSIONED_PREFIX}::QuickTest
        ${QT_PROTOBUF_NAMESPACE}::Grpc
)
_qt_internal_link_protobuf_objects(qtgrpc_qml_test_grpc_http qtgrpc_test_qtprotobuf_gen)
_qt_internal_link_protobuf_objects(qtgrpc_qml_test_grpc_socket qtgrpc_test_qtprotobuf_gen)
if(QT_PROTOBUF_STATIC)
    target_link_libraries(qtgrpc_qml_test_grpc_http PRIVATE ${PROTOBUF_QUICK_PLUGIN_NAME} ${GRPC_QUICK_PLUGIN_NAME})
    target_link_libraries(qtgrpc_qml_test_grpc_socket PRIVATE ${PROTOBUF_QUICK_PLUGIN_NAME} ${GRPC_QUICK_PLUGIN_NAME})
endif()
add_test(NAME qtgrpc_qml_test_grpc_http
    COMMAND ${TEST_DRIVER_NAME} $<TARGET_FILE:qtgrpc_qml_test_grpc_http> $<TARGET_FILE:echoserver> $<TARGET_FILE_NAME:qtgrpc_qml_test_grpc_http> $<TARGET_FILE_NAME:echoserver>
)
add_test(NAME qtgrpc_qml_test_grpc_socket
        COMMAND ${TEST_DRIVER_NAME} $<TARGET_FILE:qtgrpc_qml_test_grpc_socket> $<TARGET_FILE:echoserver> $<TARGET_FILE_NAME:qtgrpc_qml_test_grpc_socket> $<TARGET_FILE_NAME:echoserver>
)
qt_protobuf_internal_add_target_qml(TARGET qtgrpc_qml_test_grpc_http QML_FILES ${QML_FILES})
qt_protobuf_internal_add_target_windeployqt(TARGET qtgrpc_qml_test_grpc_http QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)
qt_protobuf_internal_add_target_qml(TARGET qtgrpc_qml_test_grpc_socket QML_FILES ${QML_FILES})
qt_protobuf_internal_add_target_windeployqt(TARGET qtgrpc_qml_test_grpc_socket QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)
if(WIN32)
    set_tests_properties(qtgrpc_qml_test_grpc_http PROPERTIES
        ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..\;$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
    set_tests_properties(qtgrpc_qml_test_grpc_socket PROPERTIES
        ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..\;$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
else()
    set_tests_properties(qtgrpc_qml_test_grpc_http PROPERTIES
        ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..:$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
    set_tests_properties(qtgrpc_qml_test_grpc_socket PROPERTIES
        ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..:$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
endif()
