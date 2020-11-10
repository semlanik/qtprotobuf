set(TARGET qttypes_test)

include(${QT_PROTOBUF_CMAKE_DIR}/QtProtobufInternalHelpers.cmake)
qt_protobuf_internal_find_dependencies()

if(NOT TARGET QtProtobufProject::QtProtobufQtTypes)
    find_package(QtProtobufProject CONFIG COMPONENTS QtProtobufQtTypes REQUIRED)
endif()

file(GLOB SOURCES
    qtcoretest.cpp
    qtguitest.cpp)

qt_protobuf_internal_add_test_target(TARGET ${TARGET}
    SOURCES ${SOURCES}
    PROTO_INCLUDES $<TARGET_PROPERTY:${QT_PROTOBUF_PROJECT}::QtProtobufQtTypes,PROTO_INCLUDES>
    QML)
add_target_windeployqt(TARGET ${TARGET}
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testimage.png ${CMAKE_CURRENT_BINARY_DIR}/testimage.png COPYONLY)

target_link_libraries(${TARGET} PRIVATE ${QT_PROTOBUF_PROJECT}::QtProtobufQtTypes)
add_test(NAME ${TARGET} COMMAND ${TARGET})
