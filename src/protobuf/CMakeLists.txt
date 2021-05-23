qt_protobuf_extract_qt_variable(QT_INSTALL_PLUGINS)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/3rdparty/microjson/CMakeLists.txt")
    set(MICROJSON_MAKE_TESTS OFF)
    set(MICROJSON_OBJECT_LIB_ONLY ON)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/microjson")
    message(STATUS "Using microjson from 3rdparty")
else()
    message(FATAL_ERROR "microjson is not found.")
endif()

qt_protobuf_internal_add_library(Protobuf
    SOURCES
        qtprotobuf.cpp
        qtprotobuflogging.cpp
        qprotobufserializerregistry.cpp
        qabstractprotobufserializer.cpp
        qprotobufjsonserializer.cpp
        qprotobufserializer.cpp
        qprotobufmetaproperty.cpp
        qprotobufmetaobject.cpp
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
        qprotobufserializationplugininterface.h
        qprotobuflazymessagepointer.h
    PUBLIC_HEADER
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
        qprotobufserializationplugininterface.h
        qprotobuflazymessagepointer.h
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::Qml
    PUBLIC_DEFINES
        QT_PROTOBUF_PLUGIN_PATH="${QT_INSTALL_PLUGINS}/protobuf"
)

qtprotobuf_link_target(Protobuf microjson)
set_target_properties(Protobuf PROPERTIES
    QT_PROTOBUF_PLUGIN_PATH "${QT_INSTALL_PLUGINS}/protobuf"
)
set_property(TARGET Protobuf APPEND PROPERTY EXPORT_PROPERTIES QT_PROTOBUF_PLUGIN_PATH)

if(NOT BUILD_SHARED_LIBS)
    set(QT_PROTOBUF_EXTRA_COMPILE_DIFINITIONS QT_PROTOBUF_STATIC)
    set(QT_PROTOBUF_EXTRA_CONFIG "staticlib") # extra config for .pri file in case if static
                                              # build enabled
endif()

qt_protobuf_internal_generate_pri(Protobuf)

if(TARGET Qt5::Quick)
    add_subdirectory("quick")
endif()
