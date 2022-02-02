set(qttypes_install_includedir ${CMAKE_INSTALL_INCLUDEDIR}/${QT_PROTOBUF_NAMESPACE}Protobuf)

qt_internal_add_module(ProtobufQtTypes
    SOURCES
        qtprotobufqttypes.cpp qtprotobufqttypes.h
        qtprotobufqttypesglobal.h
    #TODO Qt6: install generated header files
    #INSTALL_INCLUDEDIR
        #"${qttypes_install_includedir}"
    # PUBLIC_INCLUDE_DIRECTORIES
    #     "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/qt_protobuf_generated>"
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::Protobuf
)

qt_internal_extend_target(ProtobufWellKnownTypes
    CONDITION TARGET Qt::Qml
    LIBRARIES
        Qt::Qml
)

 set(qml_enabled "")
 if(TARGET Qt::Qml)
     set(qml_enabled QML)
 endif()

file(GLOB proto_files ${CMAKE_CURRENT_SOURCE_DIR}/QtProtobuf/Qt*.proto)

qt6_protobuf_generate(TARGET ProtobufQtTypes
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/qt_protobuf_generated
    PROTO_FILES ${proto_files}
    FOLDER
    ${qml_enabled}
    ${extra_type_libraries_options}
)

set_target_properties(ProtobufQtTypes PROPERTIES
    QT_PROTO_INCLUDES
        "-I\"${CMAKE_CURRENT_SOURCE_DIR}\";\
-I\"${CMAKE_INSTALL_PREFIX}/${qttypes_install_includedir}\""
)
set_property(TARGET ProtobufQtTypes APPEND PROPERTY EXPORT_PROPERTIES QT_PROTO_INCLUDES)

qt_install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/QtProtobuf/QtCore.proto"
        "${CMAKE_CURRENT_SOURCE_DIR}/QtProtobuf/QtGui.proto"
    DESTINATION "${qttypes_install_includedir}"
    COMPONENT dev
)
