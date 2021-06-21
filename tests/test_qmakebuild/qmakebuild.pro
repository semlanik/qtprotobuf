QT -= gui
QT += protobuf grpc core

CONFIG += console
CONFIG -= app_bundle

SOURCES += \
        main.cpp

PROTO_FILES = test1.proto
include($$CMAKE_CURRENT_BINARY_DIR/qt_lib_protobuf.pri)
qtprotobuf_generate(true)
