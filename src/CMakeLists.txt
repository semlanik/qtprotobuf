add_subdirectory(generator)

add_subdirectory(protobuf)
add_subdirectory(wellknowntypes)
add_subdirectory(qttypes)

if(TARGET Qt::Quick)
    add_subdirectory(protobufquick)
endif()

if(TARGET Qt::Network)
    add_subdirectory(grpc)
endif()
