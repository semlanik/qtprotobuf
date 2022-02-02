qt_internal_add_test(protobuf_extranamespace_test
    SOURCES
        extra_namespace_test.cpp
        # TODO Qt6: Missing source files?
        # serializationtest.cpp
        # deserializationtest.cpp
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest
        gtest_main
        Qt::Protobuf
)

qt6_protobuf_generate(TARGET protobuf_extranamespace_test
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qt_protobuf_generated"
    PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/proto/extra_namespace.proto"
    EXTRA_NAMESPACE "MyTestNamespace"
    FIELDENUM
    # TODO Qt6: Restore QML support
    #QML
)
