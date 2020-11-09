cmake_minimum_required(VERSION 3.5)

project(clienttutorial LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)
find_package(QtProtobufProject COMPONENTS QtProtobuf QtGrpc REQUIRED)

add_executable(clienttutorial main.cpp echoclientengine.cpp qml.qrc)

qtprotobuf_generate(TARGET clienttutorial PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tutorial.proto QML TRUE)

target_compile_definitions(clienttutorial
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(clienttutorial
  PRIVATE Qt5::Core Qt5::Quick)
target_link_libraries(clienttutorial PRIVATE QtProtobufProject::QtGrpc QtProtobufProject::QtProtobuf)
