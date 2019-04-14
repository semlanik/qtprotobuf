set(PROTO_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto/)
set(GENERATED_SOURCES_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GENERATED_SOURCES_DIR})
file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto)
## test sources generation
if(WIN32)
    #Needs to set path to protobuf libraries
    set(PROTOBUF_INSTALATION_PATH "")
    set(Protobuf_PROTOC_EXECUTABLE ${PROTOBUF_INSTALATION_PATH}/bin/protoc.exe)
endif()

set(test_sources_generation "test_grpc_sources_generation")
add_custom_target(${test_sources_generation})
add_custom_command(TARGET ${test_sources_generation}
        COMMAND ${Protobuf_PROTOC_EXECUTABLE} --${PROJECT_NAME}_opt=out=${GENERATED_SOURCES_DIR} --plugin=protoc-gen-${PROJECT_NAME}=$<TARGET_FILE:${PROJECT_NAME}> --qtprotobuf_out=${GENERATED_SOURCES_DIR} -I=${PROTO_FILES_DIR} ${PROTO_FILES}
        WORKING_DIRECTORY ${PROTO_FILES_DIR}
        DEPENDS ${PROTO_FILES}
        COMMENT "Generating test headers"
)
add_dependencies(${test_sources_generation} ${PROJECT_NAME})

set(EXPECTED_GENERATED_HEADERS
    simplestringmessage.h
    testserviceclient.h
    testserviceserver.h
)

foreach(EXPECTED_GENERATED_HEADER ${EXPECTED_GENERATED_HEADERS})
    get_filename_component(GENERATED_HEADER_BASENAME ${EXPECTED_GENERATED_HEADER} NAME_WE)

    list(APPEND GENERATED_SOURCES ${GENERATED_SOURCES_DIR}/${GENERATED_HEADER_BASENAME}.cpp)
    list(APPEND GENERATED_HEADERS ${GENERATED_SOURCES_DIR}/${GENERATED_HEADER_BASENAME}.h)

    set_property(SOURCE ${GENERATED_SOURCES_DIR}/${GENERATED_HEADER_BASENAME}.cpp PROPERTY SKIP_AUTOMOC ON)
endforeach(EXPECTED_GENERATED_HEADER)

qt5_wrap_cpp(MOC_SOURCES ${GENERATED_HEADERS})
list(APPEND GENERATED_SOURCES ${MOC_SOURCES})

# TODO: so far the following cpp files are not generated 
list(REMOVE_ITEM GENERATED_SOURCES
    ${GENERATED_SOURCES_DIR}/globalenums.cpp
    ${GENERATED_SOURCES_DIR}/testserviceserver.cpp
)
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

# TODO: but their headers should be included in sources for executable to be processed by moc
list(APPEND GENERATED_SOURCES
    ${GENERATED_SOURCES_DIR}/globalenums.h
    ${GENERATED_SOURCES_DIR}/testserviceserver.h
)

set_source_files_properties(${GENERATED_SOURCES_DIR}/globalenums.h PROPERTIES GENERATED TRUE)
set_source_files_properties(${GENERATED_SOURCES_DIR}/testserviceserver.h PROPERTIES GENERATED TRUE)

if(WIN32)
    #Set  path to GTest build libraries
    set(GTEST_BOTH_LIBRARIES "")

    #Set  path to GTest include directory
    include_directories(${GTEST_INCLUDE_PATHS} "/")
endif()

file(GLOB SOURCES main.cpp
    servertest.cpp
    clienttest.cpp)

set(testtarget "qtgrpc_test")
add_executable(${testtarget} ${SOURCES} ${GENERATED_SOURCES})

if(WIN32)
    target_link_libraries(${testtarget} qtgrpc qtprotobufsupport "${GTEST_BOTH_LIBRARIES}/gmock_main.lib" "${GTEST_BOTH_LIBRARIES}/gmock.lib" Qt5::Core Qt5::Qml Qt5::Network)
elseif(UNIX)
    target_link_libraries(${testtarget} gtest_main qtgrpc qtprotobufsupport)
endif()

target_include_directories(${testtarget} PRIVATE ${GENERATED_SOURCES_DIR})
add_dependencies(${testtarget} ${test_sources_generation})

