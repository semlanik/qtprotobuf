qt_internal_add_test(protobuf_wellknowntypes_test
    SOURCES
        simpletest.cpp
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest
        gtest_main
        Qt::ProtobufWellKnownTypes
)

qt6_protobuf_generate(TARGET protobuf_wellknowntypes_test
    PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/proto/wellknowntypes.proto"
    PROTO_INCLUDES
        $<TARGET_PROPERTY:Qt::ProtobufWellKnownTypes,PROTO_INCLUDES>
# TODO Qt6: Restore QML support
#    QML
)
