add_executable(${PROJECT_NAME}
    main.cpp
    generator.cpp
    classgeneratorbase.cpp
    servergenerator.cpp
    protobufclassgenerator.cpp
    globalenumsgenerator.cpp
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
    if(NOT Protobuf_FOUND)
        target_link_libraries(${PROJECT_NAME} libprotoc)
    else()
        target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Protobuf_PROTOC_LIBRARY})
    endif()
endif()

# executable is called from parent CMakeLists.txt to generate test code
# this sets variable to access the generator exectable
set(QTPROTOBUF_GENERATOR_EXEC_PATH ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} PARENT_SCOPE)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin)
