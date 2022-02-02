qt_protobuf_internal_find_dependencies()

set(CMAKE_AUTOMOC OFF)

file(GLOB QML_FILES qml/tst_grpc.qml)

qt5_wrap_cpp(MOC_SOURCES test.h)

add_executable(qtgrpc_qml_test_http2 ${MOC_SOURCES} http2.cpp ${QML_FILES})
target_link_libraries(qtgrpc_qml_test_http2
    PRIVATE
        ${QT_VERSIONED_PREFIX}::Qml
        ${QT_VERSIONED_PREFIX}::Quick
        ${QT_VERSIONED_PREFIX}::Test
        ${QT_VERSIONED_PREFIX}::QuickTest
        ${QT_PROTOBUF_NAMESPACE}::Grpc
)
_qt_internal_link_protobuf_objects(qtgrpc_qml_test_http2 qtgrpc_test_qtprotobuf_gen)

if(QT_PROTOBUF_STATIC)
    target_link_libraries(qtgrpc_qml_test_http2 PRIVATE
        ${QT_PROTOBUF_NAMESPACE}::${PROTOBUF_QUICK_PLUGIN_NAME}
        ${QT_PROTOBUF_NAMESPACE}::${GRPC_QUICK_PLUGIN_NAME}
    )
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../test_grpc/${TEST_DRIVER_NAME}.in ${TEST_DRIVER_NAME} @ONLY)
add_test(NAME qtgrpc_qml_test_http2
    COMMAND ${TEST_DRIVER_NAME}
        $<TARGET_FILE:qtgrpc_qml_test_http2>
        $<TARGET_FILE:echoserver>
        $<TARGET_FILE_NAME:qtgrpc_qml_test_http2>
        $<TARGET_FILE_NAME:echoserver>
)

qt_protobuf_internal_add_target_qml(TARGET qtgrpc_qml_test_http2 QML_FILES ${QML_FILES})
qt_protobuf_internal_add_target_windeployqt(TARGET qtgrpc_qml_test_http2 QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)

if(NOT QT_PROTOBUF_STATIC AND NOT QT_PROTOBUF_STANDALONE_TESTS)
    set(path_separator ":")
    if(WIN32)
        set(path_separator "$<SEMICOLON>")
    endif()

    set_tests_properties(qtgrpc_qml_test_http2 PROPERTIES
        ENVIRONMENT "QML2_IMPORT_PATH=$ENV{QML2_IMPORT_PATH}${path_separator}\
$<TARGET_FILE_DIR:${QT_PROTOBUF_NAMESPACE}::${PROTOBUF_QUICK_PLUGIN_NAME}>/..${path_separator}\
$<TARGET_FILE_DIR:${QT_PROTOBUF_NAMESPACE}::${GRPC_QUICK_PLUGIN_NAME}>/.."
    )
endif()

if(QT_PROTOBUF_NATIVE_GRPC_CHANNEL)
    include("${CMAKE_CURRENT_LIST_DIR}/native_grpc_channel_tests.cmake")
endif()
