qt_protobuf_generate(GENERATED_TARGET protobuf_qttypes_basic_gen
    PROTO_INCLUDES $<TARGET_PROPERTY:Qt::ProtobufQtTypes,QT_PROTO_INCLUDES>
    PROTO_FILES
        proto/qttypes.proto
    # TODO Qt6: Restore QML support
    # QML
)

qt_internal_add_test(protobuf_qttypes_core
    SOURCES
        qtcoretest.cpp
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest
        gtest_main
        Qt::ProtobufQtTypes
)
_qt_internal_link_protobuf_objects(protobuf_qttypes_core protobuf_qttypes_basic_gen)

qt_internal_add_test(protobuf_qttypes_gui
    SOURCES
        qtguitest.cpp
    TESTDATA
        "${CMAKE_CURRENT_SOURCE_DIR}/testimage.png"
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest
        gtest_main
        Qt::ProtobufQtTypes
)
_qt_internal_link_protobuf_objects(protobuf_qttypes_gui protobuf_qttypes_basic_gen)
