set(TARGET qtprotobuf_test)

qt_protobuf_internal_find_dependencies()

file(GLOB SOURCES
    simpletest.cpp
    serializationtest.cpp
    deserializationtest.cpp
    serializationcomplexmessagemap.cpp
    converterstest.cpp
    jsonserializationtest.cpp
    jsondeserializationtest.cpp
    duplicatedmetatypestest.cpp
    nestedtest.cpp)
if(NOT WIN32)
    list(APPEND SOURCES internalstest.cpp)
endif()

qt_protobuf_internal_add_test(TARGET ${TARGET}
    SOURCES ${SOURCES}
    QML FIELDENUM)
qt_protobuf_internal_add_target_windeployqt(TARGET ${TARGET}
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ${TARGET} COMMAND ${TARGET})
