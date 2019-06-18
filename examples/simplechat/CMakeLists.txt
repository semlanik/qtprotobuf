set(TARGET simplechat)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Quick Network REQUIRED)
find_package(QtProtobufProject CONFIG REQUIRED COMPONENTS QtProtobuf QtGrpc)

set(GENERATED_HEADERS
    chatmessage.h
    chatmessages.h
    user.h
    users.h
    none.h
    simplechatclient.h)

file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/proto/simplechat.proto)

generate_qtprotobuf(TARGET ${TARGET}
    OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated
    PROTO_FILES ${PROTO_FILES}
    GENERATED_HEADERS ${GENERATED_HEADERS})

file(GLOB SOURCES main.cpp
    simplechatengine.cpp
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../simplechatserver/cert.pem DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${TARGET} ${SOURCES} resources.qrc)
add_target_windeployqt(TARGET ${TARGET})

add_dependencies(${TARGET} ${QtProtobuf_GENERATED} examples_common)
target_link_libraries(${TARGET} examples_common QtProtobufProject::QtProtobuf QtProtobufProject::QtGrpc ${QtProtobuf_GENERATED} Qt5::Quick Qt5::Qml)

set(CLIENT_EXEC_PATH ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} PARENT_SCOPE)
