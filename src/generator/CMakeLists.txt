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

if(DEFINED Protobuf_INCLUDE_DIRS)
    target_include_directories(${TARGET} PUBLIC ${Protobuf_INCLUDE_DIRS})
endif()

target_link_libraries(${TARGET} protobuf::libprotobuf protobuf::libprotoc)

install(TARGETS ${TARGET}
    RUNTIME DESTINATION ${TARGET_BIN_DIR})
