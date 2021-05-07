set(TARGET qttypes_test)

qt_protobuf_internal_find_dependencies()

file(GLOB SOURCES
    qtcoretest.cpp
    qtguitest.cpp)

qt_protobuf_internal_add_test(TARGET ${TARGET}
    SOURCES ${SOURCES}
    PROTO_INCLUDES $<TARGET_PROPERTY:${QT_PROTOBUF_NAMESPACE}::ProtobufQtTypes,PROTO_INCLUDES>
    QML)
qt_protobuf_internal_add_target_windeployqt(TARGET ${TARGET}
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testimage.png ${CMAKE_CURRENT_BINARY_DIR}/testimage.png COPYONLY)

target_link_libraries(${TARGET} PRIVATE ${QT_PROTOBUF_NAMESPACE}::ProtobufQtTypes)
add_test(NAME ${TARGET} COMMAND ${TARGET})
