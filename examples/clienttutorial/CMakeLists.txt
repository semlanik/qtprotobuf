cmake_minimum_required(VERSION 3.5)

project(clienttutorial LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)
set(CMAKE_POSITION_INDEPENDENT_CODE ${Qt5_POSITION_INDEPENDENT_CODE})

if(NOT TARGET ${QT_PROTOBUF_PROJECT}::QtProtobuf)
    find_package(${QT_PROTOBUF_PROJECT} COMPONENTS QtProtobuf REQUIRED)
endif()
if(NOT TARGET ${QT_PROTOBUF_PROJECT}::QtGrpc)
    find_package(${QT_PROTOBUF_PROJECT} COMPONENTS QtGrpc REQUIRED)
endif()

add_executable(clienttutorial main.cpp echoclientengine.cpp qml.qrc)

qtprotobuf_generate(TARGET clienttutorial
                    PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tutorial.proto
                    QML TRUE)

target_compile_definitions(clienttutorial
                           PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
if(QT_PROTOBUF_STATIC)
   add_compile_definitions(QT_PROTOBUF_STATIC)#Old cmake versions compatibility
endif()
target_link_libraries(clienttutorial PRIVATE Qt5::Core
                                             Qt5::Quick
                                             QtProtobufProject::QtGrpc
                                             QtProtobufProject::QtProtobuf)
