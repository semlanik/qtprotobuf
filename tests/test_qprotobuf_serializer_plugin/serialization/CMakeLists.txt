set(TARGET serializationplugin)

set(TARGET_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET})
set(TARGET_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
set(TARGET_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(TARGET_BIN_DIR ${CMAKE_INSTALL_BINDIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

if(NOT DEFINED QT_QMAKE_EXECUTABLE)
    find_program(QT_QMAKE_EXECUTABLE "qmake")
    if(QT_QMAKE_EXECUTABLE STREQUAL QT_QMAKE_EXECUTABLE-NOTFOUND)
        message(FATAL_ERROR "Could not find qmake executable")
    endif()
endif()

file(GLOB SOURCES
    qtserializationplugin.cpp
    qprotobufjsonserializerimpl.cpp
    qprotobufserializerimpl.cpp)

file(GLOB HEADERS
    qtserializationplugin.h
    qtserialization_global.h)

add_library(${TARGET} SHARED ${SOURCES})
target_link_libraries(${TARGET} PRIVATE Qt5::Core Qt5::Qml ${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobuf)
target_compile_definitions(${TARGET} PRIVATE SERIALIZATION_LIB)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/serializeinfo.json" "${CMAKE_CURRENT_BINARY_DIR}/serializeinfo.json" COPYONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/serializeinfo.json" DESTINATION "$<TARGET_PROPERTY:${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobuf,QT_PROTOBUF_PLUGIN_PATH>")

install(TARGETS ${TARGET}
    PUBLIC_HEADER DESTINATION "${TARGET_INCLUDE_DIR}"
    RUNTIME DESTINATION "$<TARGET_PROPERTY:${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobuf,QT_PROTOBUF_PLUGIN_PATH>"
    LIBRARY DESTINATION "$<TARGET_PROPERTY:${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobuf,QT_PROTOBUF_PLUGIN_PATH>")
