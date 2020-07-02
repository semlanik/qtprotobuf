set(TARGET qttypes_test)

include(${QT_PROTOBUF_CMAKE_DIR}/QtProtobufTest.cmake)
find_package(QtProtobufProject CONFIG COMPONENTS QtProtobuf QtGrpc QtProtobufQtTypes REQUIRED)

file(GLOB SOURCES
    simpletest.cpp)

add_test_target(TARGET ${TARGET}
    SOURCES ${SOURCES}
    PROTO_INCLUDES $<TARGET_PROPERTY:${QT_PROTOBUF_PROJECT}::QtProtobufQtTypes,PROTO_INCLUDES>
    QML)
add_target_windeployqt(TARGET ${TARGET}
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${TARGET} PRIVATE ${QT_PROTOBUF_PROJECT}::QtProtobufQtTypes)
add_test(NAME ${TARGET} COMMAND ${TARGET})
