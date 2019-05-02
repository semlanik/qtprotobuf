project(qtprotobufgen VERSION ${QTPROTOBUF_PROJECT_VERSION} LANGUAGES CXX)

set(TARGET ${PROJECT_NAME})
set(TARGET_EXPORT ${TARGET}Targets)
set(TARGET_BIN_DIR ${CMAKE_INSTALL_BINDIR})

file(GLOB SOURCES main.cpp
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
    clientsourcegenerator.cpp)

file(GLOB HEADERS classgeneratorbase.h
    classsourcegeneratorbase.h
    clientgenerator.h
    clientsourcegenerator.h
    generator.h
    globalenumsgenerator.h
    globalenumssourcegenerator.h
    protobufclassgenerator.h
    protobufsourcegenerator.h
    servergenerator.h
    servicegeneratorbase.h
    templates.h
    utils.h)

add_executable(${TARGET} ${SOURCES})

if(WIN32)
    include_directories(${PROTOBUF_INSTALATION_PATH}/include)
    set(PROTOBUF_LIBRARIES_PATH ${PROTOBUF_INSTALATION_PATH}/lib)
    link_directories(${PROTOBUF_LIBRARIES_PATH} ${GRPC_LIBRARIES})
endif()

target_link_libraries(${TARGET} ${Protobuf_LIBRARIES} ${Protobuf_PROTOC_LIBRARY})

install(TARGETS ${TARGET}
    RUNTIME DESTINATION ${TARGET_BIN_DIR})
