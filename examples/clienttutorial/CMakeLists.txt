cmake_minimum_required(VERSION 3.5)

project(clienttutorial LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)
set(CMAKE_POSITION_INDEPENDENT_CODE ${Qt5_POSITION_INDEPENDENT_CODE})
set(QT_PROTOBUF_NAMESPACE QtProtobuf)
if(NOT TARGET ${QT_PROTOBUF_NAMESPACE}::qtprotobufgen)
    find_package(${QT_PROTOBUF_NAMESPACE} COMPONENTS ProtobufGenerator CONFIG REQUIRED)
endif()
if(NOT TARGET ${QT_PROTOBUF_NAMESPACE}::Protobuf)
    find_package(${QT_PROTOBUF_NAMESPACE} COMPONENTS Protobuf CONFIG REQUIRED)
endif()
if(NOT TARGET ${QT_PROTOBUF_NAMESPACE}::Grpc)
    find_package(${QT_PROTOBUF_NAMESPACE} COMPONENTS Grpc CONFIG REQUIRED)
endif()

add_executable(clienttutorial main.cpp echoclientengine.cpp qml.qrc)

qt6_protobuf_generate(TARGET clienttutorial
                    PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tutorial.proto
                    QML TRUE)

target_compile_definitions(clienttutorial
                           PRIVATE "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>")
if(QT_PROTOBUF_STATIC)
   add_definitions(-DQT_PROTOBUF_STATIC)# add_definitions is used because old cmake versions
                                        # compatibility
endif()
target_link_libraries(clienttutorial PRIVATE Qt5::Core
                                             Qt5::Quick
                                             ${QT_PROTOBUF_NAMESPACE}::Grpc
                                             ${QT_PROTOBUF_NAMESPACE}::Protobuf)
