add_subdirectory(test_protobuf)
add_subdirectory(test_protobuf_extra_namespace)
#add_subdirectory(test_protobuf_multifile)
# TODO Qt6: Need to use qt_internal_add_plugin for the test plugin.
# add_subdirectory(test_protobuf_serializer_plugin)
add_subdirectory(test_wellknowntypes)
add_subdirectory(test_qttypes)
add_subdirectory(test_grpc)

if(TARGET Qt::Quick OR TARGET Qt::QuickTest)
#    add_subdirectory(test_protobuf_qml)
endif()
