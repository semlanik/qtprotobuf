
QT += protobuf grpc core

SOURCES = field_enum_example.cpp
PROTO_FILES = field_enum_example.proto
qt6_protobuf_generate(false)
