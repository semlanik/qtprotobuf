add_subdirectory("test_protobuf")
add_subdirectory("test_grpc")
add_subdirectory("test_qml")
add_subdirectory("test_protobuf_multifile")
if(NOT WIN32)#TODO: There are linking issues with windows build of well-known types...
	add_subdirectory("test_wellknowntypes")
endif()
