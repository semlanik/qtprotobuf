set(qttypes_install_includedir ${CMAKE_INSTALL_INCLUDEDIR}/${QT_PROTOBUF_NAMESPACE}Protobuf)

qt_protobuf_internal_add_library(ProtobufQtTypes
    SOURCES
        qtprotobufqttypes.cpp
    PUBLIC_HEADER
        qtprotobufqttypes.h
        qtprotobufqttypesglobal.h
    INSTALL_INCLUDEDIR
        "${qttypes_install_includedir}"
    PUBLIC_INCLUDE_DIRECTORIES
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>"
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Qml
        ${QT_PROTOBUF_NAMESPACE}::Protobuf
)


file(GLOB PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/QtProtobuf/Qt*.proto)

set(qml_enabled "")
if(TARGET Qt5::Qml)
    set(qml_enabled QML)
endif()

qtprotobuf_generate(TARGET ProtobufQtTypes
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated
    PROTO_FILES ${PROTO_FILE}
    FOLDER
    ${qml_enabled}
    ${extra_type_libraries_options}
)

target_compile_definitions(ProtobufQtTypes PRIVATE QT_BUILD_PROTOBUF_QT_TYPES_LIB)

set_target_properties(ProtobufQtTypes PROPERTIES
    PROTO_INCLUDES
        "-I\"${CMAKE_CURRENT_SOURCE_DIR}\";\
-I\"${CMAKE_INSTALL_PREFIX}/${qttypes_install_includedir}\""
)

set_property(TARGET ProtobufQtTypes APPEND PROPERTY EXPORT_PROPERTIES PROTO_INCLUDES)

if(QT_PROTOBUF_INSTALL)
    install(FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/QtProtobuf/QtCore.proto"
            "${CMAKE_CURRENT_SOURCE_DIR}/QtProtobuf/QtGui.proto"
        DESTINATION "${qttypes_install_includedir}"
        COMPONENT dev
    )
endif()
