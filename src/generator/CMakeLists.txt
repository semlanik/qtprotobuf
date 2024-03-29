qt_protobuf_internal_add_executable(qtprotobufgen
    SOURCES
        utils.h
        descriptorprinterbase.h
        main.cpp
        generatorcommon.cpp generatorcommon.h
        generatoroptions.cpp generatoroptions.h
        templates.cpp templates.h
        generatorbase.cpp generatorbase.h
        singlefilegenerator.cpp singlefilegenerator.h
        multifilegenerator.cpp multifilegenerator.h
        baseprinter.cpp baseprinter.h
        messagedeclarationprinter.cpp messagedeclarationprinter.h
        messagedefinitionprinter.cpp messagedefinitionprinter.h
        enumdeclarationprinter.cpp enumdeclarationprinter.h
        enumdefinitionprinter.cpp enumdefinitionprinter.h
        servicedeclarationprinterbase.cpp servicedeclarationprinterbase.h
        clientdeclarationprinter.cpp clientdeclarationprinter.h
        clientdefinitionprinter.cpp clientdefinitionprinter.h
        serverdeclarationprinter.cpp serverdeclarationprinter.h
    LIBRARIES
        protobuf::libprotobuf
        protobuf::libprotoc
)

configure_file("${QT_PROTOBUF_CMAKE_DIR}/QtProtobufGen.cmake"
    "${QT_PROTOBUF_BINARY_DIR}/QtProtobufGen.cmake" COPYONLY
)
configure_file("${QT_PROTOBUF_CMAKE_DIR}/QtPreParseProtoFile.cmake"
    "${QT_PROTOBUF_BINARY_DIR}/QtPreParseProtoFile.cmake" COPYONLY
)

if(QT_PROTOBUF_INSTALL)
    install(FILES
           "${QT_PROTOBUF_BINARY_DIR}/QtProtobufGen.cmake"
           "${QT_PROTOBUF_BINARY_DIR}/QtPreParseProtoFile.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${QT_PROTOBUF_NAMESPACE}"
        COMPONENT dev
    )
endif()
