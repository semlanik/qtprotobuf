qt_internal_add_module(Grpc
    SOURCES
        qgrpcasyncoperationbase.cpp qgrpcasyncoperationbase.h
        qgrpccallreply.cpp qgrpccallreply.h
        qgrpcstream.cpp qgrpcstream.h
        qgrpcstatus.cpp qgrpcstatus.h
        qabstractgrpcchannel.cpp qabstractgrpcchannel.h
        qgrpchttp2channel.cpp qgrpchttp2channel.h
        qabstractgrpcclient.cpp qabstractgrpcclient.h
        qgrpccredentials.cpp qgrpccredentials.h
        qgrpcsslcredentials.cpp qgrpcsslcredentials.h
        qgrpcinsecurecredentials.cpp qgrpcinsecurecredentials.h
        qgrpcuserpasswordcredentials.cpp qgrpcuserpasswordcredentials.h
        qtgrpclogging.cpp qtgrpclogging_p.h
        qabstractgrpccredentials.h
        qtgrpcglobal.h
    PUBLIC_LIBRARIES
        Qt::Protobuf
        Qt::Core
        Qt::Network
    LIBRARIES
       Qt::ProtobufPrivate
    EXCEPTIONS
)

# TODO Qt6: Perhaps it will be removed after the new implementation of HTTP/2 channel is introduced
# if(QT_PROTOBUF_NATIVE_GRPC_CHANNEL)
#     qt_protobuf_internal_extend_target(Grpc
#         SOURCES
#             qgrpcchannel.cpp qgrpcchannel_p.h
#         PUBLIC_HEADER
#             qgrpcchannel.h
#         PUBLIC_DEFINES
#             QT_PROTOBUF_NATIVE_GRPC_CHANNEL
#         PUBLIC_LIBRARIES
#             gRPC::grpc++
#             ${CMAKE_THREAD_LIBS_INIT}
#     )
# endif()

# TODO Qt6: Restore QML support
# if(TARGET Qt5::Quick)
#     add_subdirectory("quick")
# endif()
