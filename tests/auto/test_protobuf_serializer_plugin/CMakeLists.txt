find_package(Threads REQUIRED)
qt_internal_add_test(protobuf_serialiaser_plugin_test
    SOURCES
        serializationplugintest.cpp
    LIBRARIES
        gtest_main
        gtest
        Qt::Protobuf
        Qt::Grpc
        Qt::Core
        Qt::Test
        Qt::Network
        ${CMAKE_THREAD_LIBS_INIT}
)

add_subdirectory("serialization")
