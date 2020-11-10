set(TARGET qtprotobuf_qml_test)

include(${QT_PROTOBUF_CMAKE_DIR}/QtProtobufInternalHelpers.cmake)
qt_protobuf_internal_find_dependencies()
find_package(Qt5 COMPONENTS Quick QuickTest REQUIRED)

set(CMAKE_AUTOMOC OFF)

file(GLOB SOURCES main.cpp)
file(GLOB QML_FILES qml/tst_simple.qml)

qt5_wrap_cpp(MOC_SOURCES test.h)

add_executable(${TARGET} ${MOC_SOURCES} ${SOURCES} ${QML_FILES})
target_link_libraries(${TARGET} PRIVATE Qt5::Core Qt5::Qml Qt5::Network Qt5::Quick Qt5::Test Qt5::QuickTest QtProtobufProject::QtProtobuf)

if(TARGET QtProtobufProject::QtProtobufWellKnownTypes)
    target_link_libraries(${TARGET} PRIVATE QtProtobufProject::QtProtobufWellKnownTypes)
endif()

if(QT_PROTOBUF_STATIC)
    target_link_libraries(${TARGET} PRIVATE ${PROTOBUF_QUICK_PLUGIN_NAME})
endif()

qtprotobuf_link_target(${TARGET} qtprotobuf_test_qtprotobuf_gen)

add_target_qml(TARGET ${TARGET} QML_FILES ${QML_FILES})
add_target_windeployqt(TARGET ${TARGET} QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)

add_test(NAME ${TARGET} COMMAND ${TARGET})

set_tests_properties(${TARGET} PROPERTIES
    ENVIRONMENT QML2_IMPORT_PATH=$<TARGET_FILE_DIR:${PROTOBUF_QUICK_PLUGIN_NAME}>/..)
