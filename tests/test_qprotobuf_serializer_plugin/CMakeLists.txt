set(TARGET qtprotobuf_plugin_test)

qt_protobuf_internal_find_dependencies()
find_package(Qt5 CONFIG COMPONENTS Test REQUIRED)

find_package(Threads REQUIRED)

file(GLOB SOURCES
    serializationplugintest.cpp)

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} PRIVATE gtest_main
                                        gtest
                                        ${QT_PROTOBUF_NAMESPACE}::Protobuf
                                        ${QT_PROTOBUF_NAMESPACE}::Grpc
                                        Qt5::Core
                                        Qt5::Test
                                        Qt5::Network
                                        ${CMAKE_THREAD_LIBS_INIT})
qt_protobuf_internal_add_target_windeployqt(TARGET ${TARGET}
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("serialization")

add_test(NAME ${TARGET} COMMAND ${TARGET})
set_tests_properties(${TARGET} PROPERTIES
    ENVIRONMENT QT_PROTOBUF_PLUGIN_PATH=$<TARGET_FILE_DIR:serializationplugin>)

