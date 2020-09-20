set(TARGET QtProtobuf)
set(TARGET_EXPORT ${TARGET}Targets)
set(TARGET_CONFIG ${TARGET}Config)

set(TARGET_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET})
set(TARGET_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
set(TARGET_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(TARGET_BINDIR ${CMAKE_INSTALL_BINDIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Qml REQUIRED)

if(Qt5Core_VERSION VERSION_LESS "5.12.4")
    # grpc target requires QT version not less than 5.12.4
    # earlier versions Http2DirectAttribute is broken: https://doc.qt.io/qt-5/whatsnew511.html
    #                                                  https://bugreports.qt.io/browse/QTBUG-74765
    message(FATAL_ERROR "Required Qt version is 5.12.4+")
endif()

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/3rdparty/microjson/CMakeLists.txt")
    set(MICROJSON_MAKE_TESTS OFF)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/microjson" EXCLUDE_FROM_ALL)
    message(STATUS "Using microjson from 3rdparty")
endif()

if(NOT TARGET microjson)
    find_package(microjson CONFIG REQUIRED)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ${Qt5_POSITION_INDEPENDENT_CODE})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${QT_PROTOBUF_CMAKE_DIR}/Coverage.cmake)
include(${QT_PROTOBUF_CMAKE_DIR}/GenerateQtHeaders.cmake)
include(${QT_PROTOBUF_CMAKE_DIR}/QtProtobufCommon.cmake)

file(GLOB SOURCES
    qtprotobuf.cpp
    qtprotobuflogging.cpp
    qprotobufserializerregistry.cpp
    qabstractprotobufserializer.cpp
    qprotobufjsonserializer.cpp
    qprotobufserializer.cpp
    qprotobufmetaproperty.cpp
    qprotobufmetaobject.cpp)

file(GLOB HEADERS
    qtprotobufglobal.h
    qtprotobuftypes.h
    qtprotobuflogging.h
    qprotobufobject.h
    qprotobufserializerregistry_p.h
    qqmllistpropertyconstructor.h
    qabstractprotobufserializer.h
    qabstractprotobufserializer_p.h
    qprotobufserializer.h
    qprotobufserializer_p.h
    qprotobufjsonserializer.h
    qprotobufselfcheckiterator.h
    qprotobufmetaproperty.h
    qprotobufmetaobject.h
    qprotobufserializationplugininterface.h)

file(GLOB PUBLIC_HEADER
    qtprotobufglobal.h
    qtprotobuftypes.h
    qtprotobuflogging.h
    qprotobufobject.h
    qqmllistpropertyconstructor.h
    qabstractprotobufserializer.h
    qabstractprotobufserializer_p.h
    qprotobufserializer.h
    qprotobufjsonserializer.h
    qprotobufselfcheckiterator.h
    qprotobufmetaproperty.h
    qprotobufmetaobject.h
    qprotobufserializationplugininterface.h)

protobuf_generate_qt_headers(PUBLIC_HEADER ${PUBLIC_HEADER} COMPONENT ${TARGET})

add_library(${TARGET} ${SOURCES})

set(QT_PROTOBUF_STATIC OFF)
if(NOT BUILD_SHARED_LIBS)
    set(QT_PROTOBUF_STATIC ON)
    if(WIN32)
        message(WARNING "Static version of QtProtobuf is not fully tested on Win32 platforms")
    endif()
    target_compile_definitions(${TARGET} PRIVATE QT_PROTOBUF_STATIC)
    set(QT_PROTOBUF_EXTRA_COMPILE_DIFINITIONS QT_PROTOBUF_STATIC)
    set(QT_PROTOBUF_EXTRA_CONFIG "staticlib") #extra config for .pri file in case if static build enabled
endif()

if(NOT DEFINED QT_QMAKE_EXECUTABLE)
    find_program(QT_QMAKE_EXECUTABLE "qmake" NO_DEFAULT_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
    if(QT_QMAKE_EXECUTABLE STREQUAL QT_QMAKE_EXECUTABLE-NOTFOUND)
        find_program(QT_QMAKE_EXECUTABLE "qmake")
        if(QT_QMAKE_EXECUTABLE STREQUAL QT_QMAKE_EXECUTABLE-NOTFOUND)
            message(FATAL_ERROR "Could not find qmake executable")
        else()
            message(STATUS "Use qmake ${QT_QMAKE_EXECUTABLE}")
        endif()
    else()
        message(STATUS "Use qmake ${QT_QMAKE_EXECUTABLE}")
    endif()
endif()

extract_qt_variable(QT_INSTALL_PLUGINS)
extract_qt_variable(QT_HOST_DATA)

set_target_properties(${TARGET} PROPERTIES QT_PROTOBUF_PLUGIN_PATH "${QT_INSTALL_PLUGINS}/protobuf")
target_compile_definitions(${TARGET} PUBLIC QT_PROTOBUF_PLUGIN_PATH="${QT_INSTALL_PLUGINS}/protobuf")

target_compile_definitions(${TARGET} PRIVATE QT_BUILD_PROTOBUF_LIB PUBLIC QT_PROTOBUF_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    QT_PROTOBUF_VERSION_MINOR=${PROJECT_VERSION_MINOR})

set_target_properties(${TARGET} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "${PUBLIC_HEADER};${GENERATED_PUBLIC_HEADER}" OUTPUT_NAME ${TARGET})
target_include_directories(${TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${QT_PROTOBUF_BINARY_DIR}/include/${TARGET}>
    $<INSTALL_INTERFACE:${TARGET_INCLUDE_DIR}>
)

target_compile_features(${TARGET} PUBLIC cxx_std_14
                                         cxx_auto_type
                                         cxx_decltype
                                         cxx_final
                                         cxx_override
                                         cxx_nullptr
                                         cxx_lambdas
                                         cxx_func_identifier)

target_link_libraries(${TARGET} PUBLIC Qt5::Core Qt5::Qml microjson)

add_library(${QT_PROTOBUF_PROJECT}::${TARGET} ALIAS ${TARGET})

install(TARGETS ${TARGET}
    EXPORT ${TARGET_EXPORT} COMPONENT dev
    ARCHIVE DESTINATION ${TARGET_LIB_DIR} COMPONENT lib
    PUBLIC_HEADER DESTINATION ${TARGET_INCLUDE_DIR} COMPONENT dev
    LIBRARY DESTINATION ${TARGET_LIB_DIR} COMPONENT lib
    RUNTIME DESTINATION ${TARGET_BINDIR} COMPONENT lib)

install(EXPORT ${TARGET_EXPORT} NAMESPACE ${QT_PROTOBUF_PROJECT}:: FILE ${TARGET_EXPORT}.cmake DESTINATION ${TARGET_CMAKE_DIR} COMPONENT dev)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${TARGET_CONFIG}.cmake.in" "${QT_PROTOBUF_BINARY_DIR}/${TARGET_CONFIG}.cmake"
    INSTALL_DESTINATION "${TARGET_CMAKE_DIR}")

install(FILES "${QT_PROTOBUF_BINARY_DIR}/${TARGET_CONFIG}.cmake" DESTINATION "${TARGET_CMAKE_DIR}" COMPONENT dev)

export(TARGETS ${TARGET} NAMESPACE ${QT_PROTOBUF_PROJECT}:: FILE ${TARGET_EXPORT}.cmake)

set(QT_PROTOBUF_EXECUTABLE_INSTALL ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${GENERATOR_TARGET})

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/qt_lib_protobuf.pri.in" "${QT_PROTOBUF_BINARY_DIR}/qt_lib_protobuf.pri" @ONLY)
install(FILES "${QT_PROTOBUF_BINARY_DIR}/qt_lib_protobuf.pri" DESTINATION "${QT_HOST_DATA}/mkspecs/modules" COMPONENT dev)

add_subdirectory("quick")

add_coverage_target(TARGET ${TARGET})
