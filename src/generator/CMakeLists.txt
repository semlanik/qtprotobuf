set(TARGET ${GENERATOR_TARGET})
set(TARGET_EXPORT ${TARGET}Targets)

set(TARGET_BINDIR ${CMAKE_INSTALL_BINDIR})
set(TARGET_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

file(GLOB SOURCES main.cpp
    generatorcommon.cpp
    generatoroptions.cpp
    templates.cpp
    generatorbase.cpp
    singlefilegenerator.cpp
    multifilegenerator.cpp
    baseprinter.cpp
    messagedeclarationprinter.cpp
    messagedefinitionprinter.cpp
    enumdeclarationprinter.cpp
    enumdefinitionprinter.cpp
    servicedeclarationprinterbase.cpp
    clientdeclarationprinter.cpp
    clientdefinitionprinter.cpp
    serverdeclarationprinter.cpp)

file(GLOB HEADERS utils.h
    generatorcommon.h
    generatoroptions.h
    templates.h
    generatorbase.h
    singlefilegenerator.h
    multifilegenerator.h
    baseprinter.h
    descriptorprinterbase.h
    messagedeclarationprinter.h
    messagedefinitionprinter.h
    enumdeclarationprinter.h
    enumdefinitionprinter.h
    servicedeclarationprinterbase.h
    clientdeclarationprinter.h
    clientdefinitionprinter.h
    serverdeclarationprinter.h)

add_executable(${TARGET} ${SOURCES})

add_executable(${QT_PROTOBUF_PROJECT}::${TARGET} ALIAS ${TARGET})

if(DEFINED Protobuf_INCLUDE_DIRS)
    target_include_directories(${TARGET} PUBLIC ${Protobuf_INCLUDE_DIRS})
endif()

target_link_libraries(${TARGET} protobuf::libprotobuf protobuf::libprotoc)

install(TARGETS ${TARGET}
    EXPORT ${TARGET_EXPORT} COMPONENT dev
    RUNTIME DESTINATION ${TARGET_BINDIR} COMPONENT lib)

install(EXPORT ${TARGET_EXPORT} NAMESPACE ${QT_PROTOBUF_PROJECT}:: FILE ${TARGET_EXPORT}.cmake DESTINATION ${TARGET_CMAKE_DIR} COMPONENT dev)

export(TARGETS ${TARGET} NAMESPACE ${QT_PROTOBUF_PROJECT}:: FILE ${TARGET_EXPORT}.cmake)
