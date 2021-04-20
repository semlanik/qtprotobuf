set(TARGET qtprotobuf_extranamespace_test)

qt_protobuf_internal_find_dependencies()

file(GLOB SOURCES
    extra_namespace_test.cpp
    serializationtest.cpp
    deserializationtest.cpp)

qt_protobuf_internal_add_test(TARGET ${TARGET}
    SOURCES ${SOURCES}
    QML FIELDENUM EXTRA_NAMESPACE "MyTestNamespace")
qt_protobuf_internal_add_target_windeployqt(TARGET ${TARGET}
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ${TARGET} COMMAND ${TARGET})

