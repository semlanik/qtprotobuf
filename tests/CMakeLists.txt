set(testgeneration "test_generation")
set(TESTS_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
file(MAKE_DIRECTORY ${TESTS_OUT_DIR})
file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto)
## test sources generation
if(WIN32)
    #Needs to set path to protobuf libraries
    set(PROTOBUF_INSTALATION_PATH "")
    set(Protobuf_PROTOC_EXECUTABLE ${PROTOBUF_INSTALATION_PATH}/bin/protoc.exe)
endif()

add_custom_target(${testgeneration})
add_custom_command(TARGET ${testgeneration}
        COMMAND ${Protobuf_PROTOC_EXECUTABLE} --${PROJECT_NAME}_opt=out=${TESTS_OUT_DIR} --plugin=protoc-gen-${PROJECT_NAME}=$<TARGET_FILE:${PROJECT_NAME}> --qtprotobuf_out=${TESTS_OUT_DIR} -I=${CMAKE_CURRENT_SOURCE_DIR}/proto/ ${PROTO_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/proto/
        DEPENDS ${PROTO_FILES}
        COMMENT "Generating test headers"
)

add_dependencies(${testgeneration} ${PROJECT_NAME})

## test sources build
# policy enables automoc for generated files
if(${CMAKE_VERSION} VERSION_GREATER "3.10.0")
    cmake_policy(SET CMP0071 NEW)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(EXPECTED_GENERATED_HEADERS 
    complexmessage.h
    externalcomplexmessage.h
    globalenums.h
    repeatedbytesmessage.h
    repeatedcomplexmessage.h
    repeateddoublemessage.h
    repeatedexternalcomplexmessage.h
    repeatedfixedint64message.h
    repeatedfixedintmessage.h
    repeatedfloatmessage.h
    repeatedint64message.h
    repeatedintmessage.h
    repeatedsfixedint64message.h
    repeatedsfixedintmessage.h
    repeatedsint64message.h
    repeatedsintmessage.h
    repeatedstringmessage.h
    repeateduint64message.h
    repeateduintmessage.h
    simpleboolmessage.h
    simplebytesmessage.h
    simpledoublemessage.h
    simpleenumlistmessage.h
    simpleenummessage.h
    simpleexternalenummessage.h
    simpleexternalmessage.h
    simplefileenummessage.h
    simplefixed32complexmessagemapmessage.h
    simplefixed32sint32mapmessage.h
    simplefixed32sint64mapmessage.h
    simplefixed32stringmapmessage.h
    simplefixed32suint32mapmessage.h
    simplefixed64complexmessagemapmessage.h
    simplefixed64sint32mapmessage.h
    simplefixed64sint64mapmessage.h
    simplefixed64stringmapmessage.h
    simplefixed64suint32mapmessage.h
    simplefixedint32message.h
    simplefixedint64message.h
    simplefloatmessage.h
    simpleint32complexmessagemapmessage.h
    simpleint32sint32mapmessage.h
    simpleint32sint64mapmessage.h
    simpleint32stringmapmessage.h
    simpleint32suint32mapmessage.h
    simpleint64complexmessagemapmessage.h
    simpleint64message.h
    simpleint64sint32mapmessage.h
    simpleint64sint64mapmessage.h
    simpleint64stringmapmessage.h
    simpleint64suint32mapmessage.h
    simpleintmessage.h
    simplesfixed32complexmessagemapmessage.h
    simplesfixed32sint32mapmessage.h
    simplesfixed32sint64mapmessage.h
    simplesfixed32stringmapmessage.h
    simplesfixed32suint32mapmessage.h
    simplesfixed64complexmessagemapmessage.h
    simplesfixed64sint32mapmessage.h
    simplesfixed64sint64mapmessage.h
    simplesfixed64stringmapmessage.h
    simplesfixed64suint32mapmessage.h
    simplesfixedint32message.h
    simplesfixedint64message.h
    simplesint32complexmessagemapmessage.h
    simplesint32mapmessage.h
    simplesint32sint32mapmessage.h
    simplesint32sint64mapmessage.h
    simplesint32stringmapmessage.h
    simplesint32suint32mapmessage.h
    simplesint64complexmessagemapmessage.h
    simplesint64message.h
    simplesint64sint32mapmessage.h
    simplesint64sint64mapmessage.h
    simplesint64stringmapmessage.h
    simplesint64suint32mapmessage.h
    simplesintmessage.h
    simplestringcomplexmessagemapmessage.h
    simplestringmapmessage.h
    simplestringmessage.h
    simplestringsint32mapmessage.h
    simplestringsint64mapmessage.h
    simplestringstringmapmessage.h
    simplestringsuint32mapmessage.h
    simpleuint32complexmessagemapmessage.h
    simpleuint32sint32mapmessage.h
    simpleuint32sint64mapmessage.h
    simpleuint32stringmapmessage.h
    simpleuint32suint32mapmessage.h
    simpleuint64complexmessagemapmessage.h
    simpleuint64message.h
    simpleuint64sint32mapmessage.h
    simpleuint64sint64mapmessage.h
    simpleuint64stringmapmessage.h
    simpleuint64suint32mapmessage.h
    simpleuintmessage.h
    stepchildenummessage.h
    testserviceclient.h
    testserviceserver.h
)

foreach(EXPECTED_GENERATED_HEADER ${EXPECTED_GENERATED_HEADERS})
    get_filename_component(GENERATED_HEADER_BASENAME ${EXPECTED_GENERATED_HEADER} NAME_WE)

    list(APPEND GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_HEADER_BASENAME}.cpp)
    list(APPEND GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_HEADER_BASENAME}.h)

    set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_HEADER_BASENAME}.cpp PROPERTY SKIP_AUTOMOC ON)
endforeach(EXPECTED_GENERATED_HEADER)

qt5_wrap_cpp(MOC_SOURCES ${GENERATED_HEADERS})
list(APPEND GENERATED_SOURCES ${MOC_SOURCES})

# TODO: so far the following cpp files are not generated 
list(REMOVE_ITEM GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/testserviceserver.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/globalenums.cpp
)
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

# TODO: but their headers should be included in sources for executable to be processed by moc
list(APPEND GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/testserviceserver.h
    ${CMAKE_CURRENT_BINARY_DIR}/globalenums.h
)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/globalenums.h PROPERTIES GENERATED TRUE)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/testserviceserver.h PROPERTIES GENERATED TRUE)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src/protobuf ${CMAKE_SOURCE_DIR}/src/grpc)

if(WIN32)
    #Set  path to GTest build libraries
    set(GTEST_BOTH_LIBRARIES "")

    #Set  path to GTest include directory
    include_directories(${GTEST_INCLUDE_PATHS} "/")
    find_package(Qt5 COMPONENTS Core Quick Network REQUIRED)
endif()

file(GLOB SOURCES main.cpp
    simpletest.cpp
    serializationtest.cpp
    deserializationtest.cpp
    servertest.cpp
    clienttest.cpp
    serializationcomplexmessagemap.cpp)

set(testtarget "qtprotobuf_test")
add_executable(${testtarget} ${SOURCES} ${GENERATED_SOURCES})

if(WIN32)
    target_link_libraries(${testtarget} qtgrpc qtprotobufsupport "${GTEST_BOTH_LIBRARIES}/gmock_main.lib" "${GTEST_BOTH_LIBRARIES}/gmock.lib" Qt5::Quick Qt5::Core Qt5::Network)
elseif(UNIX)
    target_link_libraries(${testtarget} gtest qtgrpc)
endif()
add_dependencies(${testtarget} ${testgeneration})
