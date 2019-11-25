set(TARGET qtprotobuf_test)

include(${QTPROTOBUF_CMAKE_DIR}/QtProtobufCommon.cmake)

file(GLOB SOURCES
    simpletest.cpp
    serializationtest.cpp
    deserializationtest.cpp
    serializationcomplexmessagemap.cpp
    converterstest.cpp
    jsonserializationtest.cpp)

add_test_target(TARGET ${TARGET}
    EXCLUDE_HEADERS nestedsimpleintmessage.h
    SOURCES ${SOURCES}
    QML TRUE)
add_target_windeployqt(TARGET ${TARGET}
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ${TARGET} COMMAND ${TARGET})
