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

set(test_sources_generation "test_generation")
add_custom_target(${test_sources_generation})
add_custom_command(TARGET ${test_sources_generation}
        COMMAND ${Protobuf_PROTOC_EXECUTABLE} --${PROJECT_NAME}_opt=out=${GENERATED_SOURCES_DIR} --plugin=protoc-gen-${PROJECT_NAME}=$<TARGET_FILE:${PROJECT_NAME}> --qtprotobuf_out=${GENERATED_SOURCES_DIR} -I=${PROTO_FILES_DIR} ${PROTO_FILES}
        WORKING_DIRECTORY ${PROTO_FILES_DIR}
        DEPENDS ${PROTO_FILES}
        COMMENT "Generating test headers"
)
add_dependencies(${test_sources_generation} ${PROJECT_NAME})

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
    simplefixed32int32mapmessage.h
    simplefixed32int64mapmessage.h
    simplefixed32sint32mapmessage.h
    simplefixed32sint64mapmessage.h
    simplefixed32stringmapmessage.h
    simplefixed32uint32mapmessage.h
    simplefixed32uint64mapmessage.h
    simplefixed64complexmessagemapmessage.h
    simplefixed64int32mapmessage.h
    simplefixed64int64mapmessage.h
    simplefixed64sint32mapmessage.h
    simplefixed64sint64mapmessage.h
    simplefixed64stringmapmessage.h
    simplefixed64uint32mapmessage.h
    simplefixed64uint64mapmessage.h
    simplefixedint32message.h
    simplefixedint64message.h
    simplefloatmessage.h
    simpleint32complexmessagemapmessage.h
    simpleint32int32mapmessage.h
    simpleint32int64mapmessage.h
    simpleint32sint32mapmessage.h
    simpleint32sint64mapmessage.h
    simpleint32stringmapmessage.h
    simpleint32uint32mapmessage.h
    simpleint32uint64mapmessage.h
    simpleint64complexmessagemapmessage.h
    simpleint64int32mapmessage.h
    simpleint64int64mapmessage.h
    simpleint64message.h
    simpleint64sint32mapmessage.h
    simpleint64sint64mapmessage.h
    simpleint64stringmapmessage.h
    simpleint64uint32mapmessage.h
    simpleint64uint64mapmessage.h
    simpleintmessage.h
    simplesfixed32complexmessagemapmessage.h
    simplesfixed32int32mapmessage.h
    simplesfixed32int64mapmessage.h
    simplesfixed32sint32mapmessage.h
    simplesfixed32sint64mapmessage.h
    simplesfixed32stringmapmessage.h
    simplesfixed32uint32mapmessage.h
    simplesfixed32uint64mapmessage.h
    simplesfixed64complexmessagemapmessage.h
    simplesfixed64int32mapmessage.h
    simplesfixed64int64mapmessage.h
    simplesfixed64sint32mapmessage.h
    simplesfixed64sint64mapmessage.h
    simplesfixed64stringmapmessage.h
    simplesfixed64uint32mapmessage.h
    simplesfixed64uint64mapmessage.h
    simplesfixedint32message.h
    simplesfixedint64message.h
    simplesint32complexmessagemapmessage.h
    simplesint32int32mapmessage.h
    simplesint32int64mapmessage.h
    simplesint32sint32mapmessage.h
    simplesint32sint64mapmessage.h
    simplesint32stringmapmessage.h
    simplesint32uint32mapmessage.h
    simplesint32uint64mapmessage.h
    simplesint64complexmessagemapmessage.h
    simplesint64int32mapmessage.h
    simplesint64int64mapmessage.h
    simplesint64message.h
    simplesint64sint32mapmessage.h
    simplesint64sint64mapmessage.h
    simplesint64stringmapmessage.h
    simplesint64uint32mapmessage.h
    simplesint64uint64mapmessage.h
    simplesintmessage.h
    simplestringcomplexmessagemapmessage.h
    simplestringint32mapmessage.h
    simplestringint64mapmessage.h
    simplestringmessage.h
    simplestringsint32mapmessage.h
    simplestringsint64mapmessage.h
    simplestringstringmapmessage.h
    simplestringuint32mapmessage.h
    simplestringuint64mapmessage.h
    simpleuint32complexmessagemapmessage.h
    simpleuint32int32mapmessage.h
    simpleuint32int64mapmessage.h
    simpleuint32sint32mapmessage.h
    simpleuint32sint64mapmessage.h
    simpleuint32stringmapmessage.h
    simpleuint32uint32mapmessage.h
    simpleuint32uint64mapmessage.h
    simpleuint64complexmessagemapmessage.h
    simpleuint64int32mapmessage.h
    simpleuint64int64mapmessage.h
    simpleuint64message.h
    simpleuint64sint32mapmessage.h
    simpleuint64sint64mapmessage.h
    simpleuint64stringmapmessage.h
    simpleuint64uint32mapmessage.h
    simpleuint64uint64mapmessage.h
    simpleuintmessage.h
    stepchildenummessage.h
)

foreach(EXPECTED_GENERATED_HEADER ${EXPECTED_GENERATED_HEADERS})
    get_filename_component(GENERATED_HEADER_BASENAME ${EXPECTED_GENERATED_HEADER} NAME_WE)

    list(APPEND GENERATED_SOURCES ${GENERATED_SOURCES_DIR}/${GENERATED_HEADER_BASENAME}.cpp)
    list(APPEND GENERATED_HEADERS ${GENERATED_SOURCES_DIR}/${GENERATED_HEADER_BASENAME}.h)

    set_property(SOURCE ${GENERATED_SOURCES_DIR}/${GENERATED_HEADER_BASENAME}.cpp PROPERTY SKIP_AUTOMOC ON)
endforeach(EXPECTED_GENERATED_HEADER)

qt5_wrap_cpp(MOC_SOURCES ${GENERATED_HEADERS})
list(APPEND GENERATED_SOURCES ${MOC_SOURCES})

set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

# TODO: but their headers should be included in sources for executable to be processed by moc
list(APPEND GENERATED_SOURCES
    ${GENERATED_SOURCES_DIR}/globalenums.h
)

set_source_files_properties(${GENERATED_SOURCES_DIR}/globalenums.h PROPERTIES GENERATED TRUE)

if(WIN32)
    #Set  path to GTest build libraries
    set(GTEST_BOTH_LIBRARIES "")

    #Set  path to GTest include directory
    include_directories(${GTEST_INCLUDE_PATHS} "/")
endif()

file(GLOB SOURCES
    simpletest.cpp
    serializationtest.cpp
    deserializationtest.cpp
    serializationcomplexmessagemap.cpp)

set(testtarget "qtprotobuf_test")
add_executable(${testtarget} ${SOURCES} ${GENERATED_SOURCES})

if(WIN32)
    target_link_libraries(${testtarget} qtgrpc qtprotobufsupport "${GTEST_BOTH_LIBRARIES}/gtest_main.lib" "${GTEST_BOTH_LIBRARIES}/gtest.lib" Qt5::Core Qt5::Qml Qt5::Network)
elseif(UNIX)
    target_link_libraries(${testtarget} gtest_main gtest qtgrpc qtprotobufsupport)
endif()

target_include_directories(${testtarget} PRIVATE ${GENERATED_SOURCES_DIR})
add_dependencies(${testtarget} ${test_sources_generation})
