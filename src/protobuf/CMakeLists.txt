if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/3rdparty/microjson/CMakeLists.txt")
    set(MICROJSON_MAKE_TESTS OFF)
    set(MICROJSON_OBJECT_LIB_ONLY ON)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/microjson")
    message(STATUS "Using microjson from 3rdparty")
else()
    message(FATAL_ERROR "microjson is not found.")
endif()

qt_internal_add_module(Protobuf
    SOURCES
        qtprotobuf.cpp
        qtprotobuflogging.cpp qtprotobuflogging_p.h
        qprotobufserializerregistry.cpp qprotobufserializerregistry_p.h
        qabstractprotobufserializer.cpp qabstractprotobufserializer.h
        qabstractprotobufserializercommon.h
        qprotobufjsonserializer.cpp qprotobufjsonserializer.h
        qprotobufserializer.cpp qprotobufserializer.h
        qprotobufserializer_p.h
        qprotobufmetaproperty.cpp qprotobufmetaproperty.h
        qprotobufmetaobject.cpp qprotobufmetaobject.h
        qtprotobufglobal.h
        qtprotobuftypes.h
        qprotobufobject.h
        qprotobufselfcheckiterator.h
        qprotobufserializationplugininterface.h
        qprotobuflazymessagepointer.h
    PUBLIC_LIBRARIES
        Qt::Core
    LIBRARIES
        microjson
    EXTRA_CMAKE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/cmake/${INSTALL_CMAKE_NAMESPACE}ProtobufMacro.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/cmake/QtPreParseProtoFile.cmake"
    EXTRA_CMAKE_INCLUDES
        "${INSTALL_CMAKE_NAMESPACE}ProtobufMacro.cmake"
    EXCEPTIONS
)

qt_internal_extend_target(Protobuf
    CONDITION TARGET Qt::Qml
    SOURCES
        qtprotobufqmllistpropertyconstructor.h
    PUBLIC_LIBRARIES
        Qt::Qml
)
