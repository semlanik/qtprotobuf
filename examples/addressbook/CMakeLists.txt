set(addressbookgeneration "addressbook_generation")
set(ADDRESSBOOK_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
file(MAKE_DIRECTORY ${ADDRESSBOOK_OUT_DIR})
file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/proto/addressbook.proto)
## test sources generation
if(WIN32)
    #Needs to set path to protobuf libraries
    set(PROTOBUF_INSTALATION_PATH "")
    set(Protobuf_PROTOC_EXECUTABLE ${PROTOBUF_INSTALATION_PATH}/bin/protoc.exe)
endif()

add_custom_target(${addressbookgeneration})
add_custom_command(TARGET ${addressbookgeneration}
        COMMAND ${Protobuf_PROTOC_EXECUTABLE} --${PROJECT_NAME}_opt=out=${ADDRESSBOOK_OUT_DIR} --plugin=protoc-gen-${PROJECT_NAME}=$<TARGET_FILE:${PROJECT_NAME}> --qtprotobuf_out=${ADDRESSBOOK_OUT_DIR} -I=${CMAKE_CURRENT_SOURCE_DIR}/proto/ ${PROTO_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/proto/
        DEPENDS ${PROTO_FILES}
        COMMENT "Generating test headers"
)
add_dependencies(${addressbookgeneration} ${PROJECT_NAME})

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

find_package(Qt5 COMPONENTS Core Quick Network REQUIRED)

set(EXPECTED_GENERATED_HEADERS 
    globalenums.h
)

foreach(EXPECTED_GENERATED_HEADER ${EXPECTED_GENERATED_HEADERS})
    get_filename_component(GENERATED_HEADER_BASENAME ${EXPECTED_GENERATED_HEADER} NAME_WE)

    list(APPEND GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_HEADER_BASENAME}.cpp)
    list(APPEND GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_HEADER_BASENAME}.h)
    set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_HEADER_BASENAME}.cpp PROPERTY SKIP_AUTOMOC ON)
endforeach(EXPECTED_GENERATED_HEADER)

if(${CMAKE_VERSION} VERSION_LESS "3.10.0")
    qt5_wrap_cpp(MOC_SOURCES ${GENERATED_HEADERS})
    list(APPEND GENERATED_SOURCES ${MOC_SOURCES})
endif()

# TODO: so far the following cpp files are not generated 
list(REMOVE_ITEM GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/globalenums.cpp
)
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

# TODO: but their headers should be included in sources for executable to be processed by moc
list(APPEND GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/globalenums.h
)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/globalenums.h PROPERTIES GENERATED TRUE)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src/protobuf ${CMAKE_SOURCE_DIR}/src/grpc)

if(WIN32)
    #Set  path to GTest build libraries
    set(GTEST_BOTH_LIBRARIES "")
    #Set  path to GTest include directory
    include_directories(${GTEST_INCLUDE_PATHS} "/")
endif()

file(GLOB SOURCES main.cpp)

set(addressbook "addressbook_example")
add_executable(${addressbook} ${SOURCES} ${GENERATED_SOURCES})
if(WIN32)
    target_link_libraries(${addressbook} qtgrpc Qt5::Quick)
elseif(UNIX)
    target_link_libraries(${addressbook} qtgrpc Qt5::Quick)
endif()
add_dependencies(${addressbook} ${addressbookgeneration})
