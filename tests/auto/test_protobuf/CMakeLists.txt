unset(extra_generator_args)
if(TARGET Qt::Qml)
    list(APPEND extra_generator_args QML)
endif()

qt6_protobuf_generate(GENERATED_TARGET protobuf_basic_types_gen
    PROTO_FILES
        basicmessages.proto
        repeatedmessages.proto
        mapmessages.proto
        simpletest.proto
        globalenums.proto
        globalenumssamenamespace.proto
        nopackage.proto
        nopackageexternal.proto
        sequencetest.proto
        externalpackagetest.proto
        annotation.proto # TODO Qt6: move to manual tests
    PROTO_FILES_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../shared/proto/"
    FIELDENUM
    ${extra_generator_args}
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qt_protobuf_generated"
)

qt_internal_add_test(protobuf_basic
    SOURCES
        simpletest.cpp
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest_main
        gtest
        Qt::Protobuf
)
_qt_internal_link_protobuf_objects(protobuf_basic protobuf_basic_types_gen)

qt_internal_add_test(protobuf_serialization
    SOURCES
        serializationtest.cpp serializationtest.h
        serializationcomplexmessagemap.cpp
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest_main
        gtest
        Qt::Protobuf
)
_qt_internal_link_protobuf_objects(protobuf_serialization protobuf_basic_types_gen)

qt_internal_add_test(protobuf_deserialization
    SOURCES
        deserializationtest.cpp
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest_main
        gtest
        Qt::Protobuf
)
_qt_internal_link_protobuf_objects(protobuf_deserialization protobuf_basic_types_gen)

qt_internal_add_test(protobuf_converter
    SOURCES
        converterstest.cpp
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest_main
        gtest
        Qt::Protobuf
)

qt_internal_add_test(protobuf_jsonserialization
    SOURCES
        jsonserializationtest.cpp
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest_main
        gtest
        Qt::Protobuf
)
_qt_internal_link_protobuf_objects(protobuf_jsonserialization protobuf_basic_types_gen)

qt_internal_add_test(protobuf_jsondeserialization
    SOURCES
        jsondeserializationtest.cpp
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest_main
        gtest
        Qt::Protobuf
)
_qt_internal_link_protobuf_objects(protobuf_jsondeserialization protobuf_basic_types_gen)

qt_internal_add_test(protobuf_duplicatedmetatypes
    SOURCES
        duplicatedmetatypestest.cpp
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest_main
        gtest
        Qt::Protobuf
)

qt6_protobuf_generate(TARGET protobuf_duplicatedmetatypes
    PROTO_FILES
        duplicated_metatypes.proto
        duplicated_metatypes_external.proto
    PROTO_FILES_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../shared/proto/"
    # TODO Qt6: Restore QML support
    ${extra_generator_args}
    FIELDENUM
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qt_protobuf_generated"
)
_qt_internal_link_protobuf_objects(protobuf_duplicatedmetatypes protobuf_basic_types_gen)

qt_internal_add_test(protobuf_nestedtypes
    SOURCES
        nestedtest.cpp
    INCLUDE_DIRECTORIES
        ../shared
    LIBRARIES
        gtest_main
        gtest
        Qt::Protobuf
)
_qt_internal_link_protobuf_objects(protobuf_nestedtypes protobuf_basic_types_gen)

qt6_protobuf_generate(TARGET protobuf_nestedtypes
    PROTO_FILES
        nestedmessages.proto
    PROTO_FILES_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../shared/proto/"
    ${extra_generator_args}
    FIELDENUM
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qt_protobuf_generated"
)

if(NOT WIN32)
    qt_internal_add_test(protobuf_internals
        SOURCES
            internalstest.cpp
        INCLUDE_DIRECTORIES
            ../shared
        LIBRARIES
            gtest_main
            gtest
            Qt::Protobuf
    )
    _qt_internal_link_protobuf_objects(protobuf_internals protobuf_basic_types_gen)
endif()
