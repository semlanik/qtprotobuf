set(TARGET addressbook)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Quick Network REQUIRED)
find_package(QtProtobufProject CONFIG REQUIRED COMPONENTS QtProtobuf QtGrpc)

file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/proto/addressbook.proto)

qtprotobuf_generate(TARGET ${TARGET}
    OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated
    PROTO_FILES ${PROTO_FILES}
    QML TRUE)

file(GLOB SOURCES main.cpp
    addressbookengine.cpp)

add_executable(${TARGET} ${SOURCES} resources.qrc)
add_target_windeployqt(TARGET ${TARGET} QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)

add_custom_command(TARGET ${TARGET}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../addressbookserver/cert.pem $<TARGET_FILE_DIR:${TARGET}>)

add_dependencies(${TARGET} ${QtProtobuf_GENERATED} examples_common)
target_link_libraries(${TARGET} examples_common QtProtobufProject::QtProtobuf QtProtobufProject::QtGrpc ${QtProtobuf_GENERATED} Qt5::Quick Qt5::Qml)

set(CLIENT_EXEC_PATH ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} PARENT_SCOPE)
