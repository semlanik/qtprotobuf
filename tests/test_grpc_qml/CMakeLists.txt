find_package(Qt5 COMPONENTS Core Quick Network Test QuickTest REQUIRED)
find_package(QtProtobufProject COMPONENTS QtGrpc REQUIRED)

include(${QT_PROTOBUF_CMAKE_DIR}/QtProtobufTest.cmake)

set(CMAKE_AUTOMOC OFF)

file(GLOB QML_FILES qml/tst_grpc.qml)

qt5_wrap_cpp(MOC_SOURCES test.h)

add_executable(qtgrpc_qml_test_http2 ${MOC_SOURCES} http2.cpp ${QML_FILES})
if (QT_PROTOBUF_NATIVE_GRPC_CHANNEL)
    add_executable(qtgrpc_qml_test_grpc_http ${MOC_SOURCES} grpc_http.cpp ${QML_FILES})
    add_executable(qtgrpc_qml_test_grpc_socket ${MOC_SOURCES} grpc_socket.cpp ${QML_FILES})
endif()

target_link_libraries(qtgrpc_qml_test_http2 PRIVATE Qt5::Qml Qt5::Quick Qt5::Test Qt5::QuickTest QtProtobufProject::QtGrpc)
if (QT_PROTOBUF_NATIVE_GRPC_CHANNEL)
    target_link_libraries(qtgrpc_qml_test_grpc_http PRIVATE Qt5::Qml Qt5::Quick Qt5::Test Qt5::QuickTest QtProtobufProject::QtGrpc)
    target_link_libraries(qtgrpc_qml_test_grpc_socket PRIVATE Qt5::Qml Qt5::Quick Qt5::Test Qt5::QuickTest QtProtobufProject::QtGrpc)
endif()

if(QT_PROTOBUF_STATIC)
    target_link_libraries(qtgrpc_qml_test_http2 PRIVATE ${PROTOBUF_QUICK_PLUGIN_NAME} ${GRPC_QUICK_PLUGIN_NAME})
    if (QT_PROTOBUF_NATIVE_GRPC_CHANNEL)
        target_link_libraries(qtgrpc_qml_test_grpc_http PRIVATE ${PROTOBUF_QUICK_PLUGIN_NAME} ${GRPC_QUICK_PLUGIN_NAME})
        target_link_libraries(qtgrpc_qml_test_grpc_socket PRIVATE ${PROTOBUF_QUICK_PLUGIN_NAME} ${GRPC_QUICK_PLUGIN_NAME})
    endif()
endif()

qtprotobuf_link_target(qtgrpc_qml_test_http2 qtgrpc_test_qtprotobuf_gen)
if (QT_PROTOBUF_NATIVE_GRPC_CHANNEL)
    qtprotobuf_link_target(qtgrpc_qml_test_grpc_http qtgrpc_test_qtprotobuf_gen)
    qtprotobuf_link_target(qtgrpc_qml_test_grpc_socket qtgrpc_test_qtprotobuf_gen)
endif()

if(UNIX)
    set(TEST_DRIVER_NAME "test_driver.sh")
elseif(WIN32)
    set(TEST_DRIVER_NAME "test_driver.bat")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../test_grpc/${TEST_DRIVER_NAME}.in ${TEST_DRIVER_NAME} @ONLY)
add_test(NAME qtgrpc_qml_test_http2
         COMMAND ${TEST_DRIVER_NAME} $<TARGET_FILE:qtgrpc_qml_test_http2> $<TARGET_FILE:echoserver> $<TARGET_FILE_NAME:qtgrpc_qml_test_http2> $<TARGET_FILE_NAME:echoserver>
)
if (QT_PROTOBUF_NATIVE_GRPC_CHANNEL)
    add_test(NAME qtgrpc_qml_test_grpc_http
            COMMAND ${TEST_DRIVER_NAME} $<TARGET_FILE:qtgrpc_qml_test_grpc_http> $<TARGET_FILE:echoserver> $<TARGET_FILE_NAME:qtgrpc_qml_test_grpc_http> $<TARGET_FILE_NAME:echoserver>
    )
    add_test(NAME qtgrpc_qml_test_grpc_socket
            COMMAND ${TEST_DRIVER_NAME} $<TARGET_FILE:qtgrpc_qml_test_grpc_socket> $<TARGET_FILE:echoserver> $<TARGET_FILE_NAME:qtgrpc_qml_test_grpc_socket> $<TARGET_FILE_NAME:echoserver>
    )
endif()

add_target_qml(TARGET qtgrpc_qml_test_http2 QML_FILES ${QML_FILES})
add_target_windeployqt(TARGET qtgrpc_qml_test_http2 QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)
if (QT_PROTOBUF_NATIVE_GRPC_CHANNEL)
    add_target_qml(TARGET qtgrpc_qml_test_grpc_http QML_FILES ${QML_FILES})
    add_target_windeployqt(TARGET qtgrpc_qml_test_grpc_http QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)
    add_target_qml(TARGET qtgrpc_qml_test_grpc_socket QML_FILES ${QML_FILES})
    add_target_windeployqt(TARGET qtgrpc_qml_test_grpc_socket QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)
endif()


if(WIN32)
    set_tests_properties(qtgrpc_qml_test_http2 PROPERTIES
        ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..\;$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
    if (QT_PROTOBUF_NATIVE_GRPC_CHANNEL)
        set_tests_properties(qtgrpc_qml_test_grpc_http PROPERTIES
            ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..\;$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
        set_tests_properties(qtgrpc_qml_test_grpc_socket PROPERTIES
            ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..\;$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
    endif()
else()
    set_tests_properties(qtgrpc_qml_test_http2 PROPERTIES
        ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..:$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
    if (QT_PROTOBUF_NATIVE_GRPC_CHANNEL)
        set_tests_properties(qtgrpc_qml_test_grpc_http PROPERTIES
            ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..:$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
        set_tests_properties(qtgrpc_qml_test_grpc_socket PROPERTIES
            ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..:$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
    endif()
endif()
