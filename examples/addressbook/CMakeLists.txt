set(TARGET addressbook)

qt_protobuf_internal_find_dependencies()

qt_protobuf_internal_add_example(TARGET ${TARGET}
                   PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/proto/addressbook.proto"
                   SOURCES main.cpp
                           addressbookengine.cpp
                   RESOURCES resources.qrc
                   QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)

add_custom_command(TARGET ${TARGET}
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../addressbookserver/cert.pem"
            "$<TARGET_FILE_DIR:${TARGET}>"
)

set(CLIENT_EXEC_PATH ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} PARENT_SCOPE)
