add_executable(${PROJECT_NAME}
    main.cpp
    generator.cpp
    classgeneratorbase.cpp
    servergenerator.cpp
    protobufclassgenerator.cpp
    globalenumsgenerator.cpp
    globalenumssourcegenerator.cpp
    servicegeneratorbase.cpp
    templates.cpp
    clientgenerator.cpp
    classsourcegeneratorbase.cpp
    protobufsourcegenerator.cpp
    clientsourcegenerator.cpp
)

if(WIN32)
    #Needs to set path to protobuf libraries
    set(PROTOBUF_INSTALATION_PATH "")
    include_directories(${PROTOBUF_INSTALATION_PATH}/include)

    set(PROTOBUF_LIBRARIES_PATH ${PROTOBUF_INSTALATION_PATH}/lib)
    target_link_libraries(${PROJECT_NAME} "${PROTOBUF_LIBRARIES_PATH}/libprotobuf.lib")
    target_link_libraries(${PROJECT_NAME} "${PROTOBUF_LIBRARIES_PATH}/libprotoc.lib")
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Protobuf_PROTOC_LIBRARY})
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin)
