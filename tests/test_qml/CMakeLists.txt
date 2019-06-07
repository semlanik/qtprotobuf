set(TARGET qtprotobuf_qml_test)

find_package(Qt5 COMPONENTS Core Quick Network Test QuickTest REQUIRED)
find_package(QtProtobufProject COMPONENTS QtProtobuf REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/QtProtobufCommon.cmake)

file(GLOB SOURCES main.cpp)
file(GLOB QML_FILES qml/tst_simple.qml)

add_executable(${TARGET} ${SOURCES} ${QML_FILES})
target_link_libraries(${TARGET} Qt5::Core Qt5::Qml Qt5::Network Qt5::Quick Qt5::Test Qt5::QuickTest QtProtobufProject::QtProtobuf qtprotobuf_test_qtprotobuf_gen)

add_target_qml(TARGET ${TARGET} QML_FILES ${QML_FILES})

add_test(NAME ${TARGET} COMMAND ${TARGET})
