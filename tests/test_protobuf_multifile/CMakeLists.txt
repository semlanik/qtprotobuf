set(TARGET qtprotobuf_test_multifile)

include(${QT_PROTOBUF_CMAKE_DIR}/QtProtobufTest.cmake)

file(GLOB SOURCES
    simpletest.cpp
    nestedtest.cpp)

file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/../test_protobuf/proto/*.proto)

add_test_target(TARGET ${TARGET}
    PROTO_FILES ${PROTO_FILES}
    SOURCES ${SOURCES}
    QML
    MULTI
    FIELDENUM)
add_target_windeployqt(TARGET ${TARGET}
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ${TARGET} COMMAND ${TARGET})
