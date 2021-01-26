qt_protobuf_internal_add_library(Grpc
    SOURCES
        qgrpcasyncoperationbase.cpp
        qgrpcasyncreply.cpp
        qgrpcsubscription.cpp
        qgrpcstatus.cpp
        qabstractgrpcchannel.cpp
        qgrpchttp2channel.cpp
        qabstractgrpcclient.cpp
        qgrpccredentials.cpp
        qgrpcsslcredentials.cpp
        qgrpcinsecurecredentials.cpp
        qgrpcuserpasswordcredentials.cpp
    PUBLIC_HEADER
        qgrpcasyncoperationbase_p.h
        qgrpcasyncreply.h
        qgrpcsubscription.h
        qgrpcstatus.h
        qabstractgrpcchannel.h
        qgrpchttp2channel.h
        qabstractgrpcclient.h
        qabstractgrpccredentials.h
        qgrpccredentials.h
        qgrpcsslcredentials.h
        qgrpcinsecurecredentials.h
        qgrpcuserpasswordcredentials.h
        qtgrpcglobal.h
    PUBLIC_LIBRARIES
        ${QT_PROTOBUF_NAMESPACE}::Protobuf
        Qt5::Core
        Qt5::Network
)

if(QT_PROTOBUF_NATIVE_GRPC_CHANNEL)
    qt_protobuf_internal_extend_target(Grpc
        SOURCES
            qgrpcchannel.cpp qgrpcchannel_p.h
        PUBLIC_HEADER
            qgrpcchannel.h
        PUBLIC_DEFINES
            QT_PROTOBUF_NATIVE_GRPC_CHANNEL
        PUBLIC_LIBRARIES
            gRPC::grpc++
            ${CMAKE_THREAD_LIBS_INIT}
    )
endif()

if(NOT BUILD_SHARED_LIBS)
    set(QT_PROTOBUF_EXTRA_CONFIG "staticlib") #extra config for .pri file in case if static build enabled
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/qt_lib_grpc.pri.in"
    "${QT_PROTOBUF_BINARY_DIR}/qt_lib_grpc.pri" @ONLY
)

if(QT_PROTOBUF_INSTALL)
    qt_protobuf_extract_qt_variable(QT_HOST_DATA)
    install(FILES "${QT_PROTOBUF_BINARY_DIR}/qt_lib_grpc.pri"
            DESTINATION "${QT_HOST_DATA}/mkspecs/modules"
            COMPONENT dev
    )
endif()

if(TARGET Qt5::Quick)
    add_subdirectory("quick")
endif()
