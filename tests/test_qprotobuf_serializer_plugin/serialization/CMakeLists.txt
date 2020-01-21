set(TARGET serializationplugin)

set(TARGET_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET})
set(TARGET_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
set(TARGET_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(TARGET_BINDIR ${CMAKE_INSTALL_BINDIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

file(GLOB SOURCES
    qtserializationplugin.cpp
    qprotobufjsonserializerimpl.cpp
    qprotobufserializerimpl.cpp)

file(GLOB HEADERS
    qtserializationplugin.h
    qtserialization_global.h)

add_library(${TARGET} SHARED ${SOURCES})
target_link_libraries(${TARGET} PRIVATE Qt5::Core Qt5::Qml ${QT_PROTOBUF_PROJECT}::QtProtobuf)
target_compile_definitions(${TARGET} PRIVATE SERIALIZATION_LIB)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/serializeinfo.json" "${CMAKE_CURRENT_BINARY_DIR}/serializeinfo.json" COPYONLY)
