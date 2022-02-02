qt_internal_add_test(protobuf_qml_basic
    QMLTEST
    SOURCES
        main.cpp test.h
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Qml
    TESTDATA
        qml/tst_simple.qml
)

#TODO Qt6: Linker hangs when linking protobuf_basic_types_gen
#_qt_internal_link_protobuf_objects(protobuf_qml_basic protobuf_basic_types_gen)
