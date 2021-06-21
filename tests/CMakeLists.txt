if(QT_PROTOBUF_STANDALONE_TESTS)
    find_package(QtProtobuf COMPONENTS Protobuf ProtobufGenerator
        OPTIONAL_COMPONENTS Grpc ProtobufWellKnownTypes ProtobufQtTypes REQUIRED
    )
endif()

include(QtProtobufTestHelpers)

add_subdirectory("test_protobuf")
if(TARGET ${QT_VERSIONED_PREFIX}::QuickTest)
    add_subdirectory("test_qml")
endif()
add_subdirectory("test_protobuf_multifile")
add_subdirectory("test_extra_namespace")
add_subdirectory("test_qmakebuild")
if(NOT QT_PROTOBUF_STANDALONE_TESTS) # Disable in standalone mode as it requires some private
                                     # headers to work properly.
    add_subdirectory("test_extra_namespace_qml")
    add_subdirectory("test_qprotobuf_serializer_plugin")
endif()

if(TARGET ${QT_PROTOBUF_NAMESPACE}::ProtobufWellKnownTypes)
    add_subdirectory("test_wellknowntypes")
endif()

if(TARGET ${QT_PROTOBUF_NAMESPACE}::ProtobufQtTypes)
    add_subdirectory("test_qttypes")
endif()

if(WrapgRPC_FOUND AND TARGET ${QT_PROTOBUF_NAMESPACE}::Grpc)
    if(UNIX)
        set(TEST_DRIVER_NAME "test_driver.sh")
    elseif(WIN32)
        set(TEST_DRIVER_NAME "test_driver.bat")
    endif()

    add_subdirectory("test_grpc")
    if(TARGET ${QT_VERSIONED_PREFIX}::QuickTest)
        add_subdirectory("test_grpc_qml")
    endif()
else()
    message(WARNING "gRPC not found: some tests cannot be built.")
endif()
