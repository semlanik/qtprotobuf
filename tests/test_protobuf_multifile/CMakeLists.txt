set(TARGET qtprotobuf_test_multifile)

include(${QTPROTOBUF_CMAKE_DIR}/QtProtobufCommon.cmake)

file(GLOB SOURCES
    simpletest.cpp)

file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/../test_protobuf/proto/*.proto)

add_test_target(TARGET ${TARGET}
    PROTO_FILES ${PROTO_FILES}
    EXCLUDE_HEADERS nestedsimpleintmessage.h
    SOURCES ${SOURCES}
    MULTI TRUE
    NOQML TRUE)
add_target_windeployqt(TARGET ${TARGET}
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ${TARGET} COMMAND ${TARGET})
