# TODO: keep quick plugin for future features, but it's useless for now
set(TARGET protobufquickplugin)

set(TARGET_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Qml REQUIRED)

if(NOT DEFINED QT_QMAKE_EXECUTABLE)
    find_program(QT_QMAKE_EXECUTABLE "qmake")
    if(QT_QMAKE_EXECUTABLE STREQUAL QT_QMAKE_EXECUTABLE-NOTFOUND)
        message(FATAL_ERROR "Could not find qmake executable")
    endif()
endif()

execute_process(
    COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_QML
    OUTPUT_VARIABLE TARGET_IMPORTS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(TARGET_IMPORTS_DIR ${TARGET_IMPORTS_DIR}/QtProtobuf)

file(GLOB SOURCES
    qtprotobufquickplugin.cpp)

file(GLOB HEADERS
    qtprotobufquickplugin.h
    qtprotobufquick_global.h)

add_library(${TARGET} SHARED ${SOURCES})
target_link_libraries(${TARGET} PRIVATE Qt5::Core Qt5::Qml ${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobuf)
set_target_properties(${TARGET} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/QtProtobuf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/QtProtobuf"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/QtProtobuf"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/QtProtobuf")
target_compile_definitions(${TARGET} PRIVATE QTPROTOBUFQUICK_LIB)

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
install(TARGETS ${TARGET}
    PUBLIC_HEADER DESTINATION "${TARGET_INCLUDE_DIR}"
    RUNTIME DESTINATION "${TARGET_IMPORTS_DIR}"
    LIBRARY DESTINATION "${TARGET_IMPORTS_DIR}")

add_custom_command(TARGET ${TARGET}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/qmldir $<TARGET_FILE_DIR:${TARGET}>/qmldir
    COMMENT "Copying qmldir to binary directory")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION "${TARGET_IMPORTS_DIR}")
