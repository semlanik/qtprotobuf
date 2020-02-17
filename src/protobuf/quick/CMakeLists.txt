set(TARGET protobufquickplugin)

set(TARGET_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Qml REQUIRED)

include(${QT_PROTOBUF_CMAKE_DIR}/QtProtobufCommon.cmake)

extract_qt_variable(QT_INSTALL_QML)

set(TARGET_IMPORTS_DIR ${QT_INSTALL_QML}/QtProtobuf)

file(GLOB SOURCES
    qtprotobufquickplugin.cpp)

file(GLOB HEADERS
    qtprotobufquickplugin.h
    qtprotobufquick_global.h)

add_library(${TARGET} SHARED ${SOURCES})
target_link_libraries(${TARGET} PRIVATE Qt5::Core Qt5::Qml ${QT_PROTOBUF_PROJECT}::QtProtobuf)
set_target_properties(${TARGET} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/QtProtobuf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/QtProtobuf"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/QtProtobuf"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/QtProtobuf")
target_compile_definitions(${TARGET} PRIVATE QT_PROTOBUF_QUICK_LIB)

#Check for QML private headers
if(NOT DEFINED Qt5Qml_PRIVATE_INCLUDE_DIRS OR Qt5Qml_PRIVATE_INCLUDE_DIRS STREQUAL "")
    message(FATAL_ERROR "Qt protobuf project requires Qml private headers defined in system")
endif()

foreach(QML_PRIVATE_HEADER IN LISTS Qt5Qml_PRIVATE_INCLUDE_DIRS)
    message(STATUS "Check for QtQml private headers dir: ${QML_PRIVATE_HEADER}")
    if(NOT IS_DIRECTORY ${QML_PRIVATE_HEADER})
        message(FATAL_ERROR "Qt protobuf project requires Qml private headers defined in system ${QML_PRIVATE_HEADER}")
    endif()
endforeach()

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ ${Qt5Qml_PRIVATE_INCLUDE_DIRS})
install(TARGETS ${TARGET} COMPONENT lib
    PUBLIC_HEADER DESTINATION "${TARGET_INCLUDE_DIR}" COMPONENT dev
    RUNTIME DESTINATION "${TARGET_IMPORTS_DIR}" COMPONENT lib
    LIBRARY DESTINATION "${TARGET_IMPORTS_DIR}" COMPONENT lib)

add_custom_command(TARGET ${TARGET}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/qmldir $<TARGET_FILE_DIR:${TARGET}>/qmldir
    COMMENT "Copying qmldir to binary directory")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION "${TARGET_IMPORTS_DIR}" COMPONENT lib)
