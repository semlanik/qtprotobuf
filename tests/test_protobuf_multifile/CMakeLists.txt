set(TARGET qtprotobuf_test_multifile)

qt_protobuf_internal_find_dependencies()

file(GLOB SOURCES
    simpletest.cpp
    nestedtest.cpp)

file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/../test_protobuf/proto/*.proto)

qt_protobuf_internal_add_test(TARGET ${TARGET}
    PROTO_FILES ${PROTO_FILES}
    SOURCES ${SOURCES}
    QML
    MULTI
    FIELDENUM)
qt_protobuf_internal_add_target_windeployqt(TARGET ${TARGET}
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ${TARGET} COMMAND ${TARGET})
