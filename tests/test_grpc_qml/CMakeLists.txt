set(TARGET qtgrpc_qml_test)

find_package(Qt5 COMPONENTS Core Quick Network Test QuickTest REQUIRED)
find_package(QtProtobufProject COMPONENTS QtGrpc REQUIRED)

include(${QT_PROTOBUF_CMAKE_DIR}/QtProtobufTest.cmake)

set(CMAKE_AUTOMOC OFF)

file(GLOB SOURCES main.cpp)
file(GLOB QML_FILES qml/tst_grpc.qml)

qt5_wrap_cpp(MOC_SOURCES test.h)

add_executable(${TARGET} ${MOC_SOURCES} ${SOURCES} ${QML_FILES})
target_link_libraries(${TARGET} PRIVATE Qt5::Qml Qt5::Quick Qt5::Test Qt5::QuickTest QtProtobufProject::QtGrpc)

if(QT_PROTOBUF_STATIC)
    target_link_libraries(${TARGET} PRIVATE ${PROTOBUF_QUICK_PLUGIN_NAME} ${GRPC_QUICK_PLUGIN_NAME})
endif()

qtprotobuf_link_target(${TARGET} qtgrpc_test_qtprotobuf_gen)

if(UNIX)
    set(TEST_DRIVER_NAME "test_driver.sh")
elseif(WIN32)
    set(TEST_DRIVER_NAME "test_driver.bat")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../test_grpc/${TEST_DRIVER_NAME}.in ${TEST_DRIVER_NAME} @ONLY)
add_test(NAME ${TARGET}
         COMMAND ${TEST_DRIVER_NAME} $<TARGET_FILE:${TARGET}> $<TARGET_FILE:echoserver> $<TARGET_FILE_NAME:${TARGET}> $<TARGET_FILE_NAME:echoserver>
)

add_target_qml(TARGET ${TARGET} QML_FILES ${QML_FILES})
add_target_windeployqt(TARGET ${TARGET} QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)

if(WIN32)
    set_tests_properties(${TARGET} PROPERTIES
        ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..\;$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
else()
    set_tests_properties(${TARGET} PROPERTIES
        ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..:$<TARGET_FILE_DIR:${GRPC_QUICK_PLUGIN_NAME}>/..)
endif()
