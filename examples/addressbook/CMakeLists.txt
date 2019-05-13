set(TARGET addressbook)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Quick Network REQUIRED)
find_package(QtProtobuf CONFIG REQUIRED)
find_package(QtGrpc CONFIG REQUIRED)

set(GENERATED_HEADERS
    addressbookclient.h
    address.h
    contact.h
    contacts.h
    globalenums.h
    job.h
    listframe.h
    phonenumber.h
    globalenums.h
    none.h
    callstatus.h)

file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/proto/addressbook.proto)

generate_qtprotobuf(TARGET ${TARGET}
    OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated
    PROTO_FILES ${PROTO_FILES}
    GENERATED_HEADERS ${GENERATED_HEADERS})

file(GLOB SOURCES main.cpp
    addressbookengine.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../addressbookserver/cert.pem DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${TARGET} ${SOURCES} resources.qrc)
add_dependencies(${TARGET} ${QtProtobuf_GENERATED} examples_common)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_link_libraries(${TARGET} examples_common QtProtobufProject::QtProtobuf QtProtobufProject::QtGrpc ${QtProtobuf_GENERATED} Qt5::Quick Qt5::Qml)
