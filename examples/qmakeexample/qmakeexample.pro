QT -= gui
QT += protobuf grpc core

CONFIG += console
CONFIG -= app_bundle

SOURCES += \
        main.cpp

PROTO_FILES = test1.proto \
              test2.proto
qtprotobuf_generate(false)
