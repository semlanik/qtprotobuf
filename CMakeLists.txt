cmake_minimum_required(VERSION 2.8)

project(qtprotobuf)
set(testgeneration "test_generation")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (DEFINED $ENV{MAKE_TESTS})
    set(MAKE_TESTS ${MAKE_TESTS})
elseif(NOT DEFINED MAKE_TESTS)
    set(MAKE_TESTS ON)
endif()

find_package(Protobuf)

add_subdirectory("src/lib")

add_executable(${PROJECT_NAME} src/generator/main.cpp
    src/generator/generator.cpp
    src/generator/classgeneratorbase.cpp
    src/generator/servergenerator.cpp
    src/generator/protobufclassgenerator.cpp
    src/generator/globalenumsgenerator.cpp
    src/generator/servicegeneratorbase.cpp)

if (WIN32)
    #Needs to set path to protobuf libraries
    set(PROTOBUF_INSTALATION_PATH "")
    include_directories(${PROTOBUF_INSTALATION_PATH}/include)

    set(PROTOBUF_LIBRARIES_PATH ${PROTOBUF_INSTALATION_PATH}/lib)
    target_link_libraries(${PROJECT_NAME} "${PROTOBUF_LIBRARIES_PATH}/libprotobuf.lib")
    target_link_libraries(${PROJECT_NAME} "${PROTOBUF_LIBRARIES_PATH}/libprotoc.lib")

elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES} "-lprotoc")
endif()

if (MAKE_TESTS)
    set(TESTS_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/tests)
    file(MAKE_DIRECTORY ${TESTS_OUT_DIR})
    add_custom_target(${testgeneration})

    if (WIN32)
        include_directories("src/lib")
        add_custom_command(TARGET ${testgeneration} POST_BUILD
            COMMAND ${PROTOBUF_INSTALATION_PATH}/bin/protoc.exe --plugin=protoc-gen-${PROJECT_NAME}=${CMAKE_BINARY_DIR}/qtprotobuf.exe --qtprotobuf_out=${CMAKE_CURRENT_BINARY_DIR}/tests
            *.proto
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto/
            COMMENT "Generating test headers"
            )
    elseif (UNIX)
        add_custom_command(TARGET ${testgeneration} POST_BUILD
            COMMAND protoc --plugin=protoc-gen-${PROJECT_NAME}=${CMAKE_BINARY_DIR}/qtprotobuf --qtprotobuf_out=${CMAKE_CURRENT_BINARY_DIR}/tests
            *.proto
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/proto/
            COMMENT "Generating test headers"
            )
    endif()

    add_dependencies(${testgeneration} ${PROJECT_NAME})
    add_subdirectory("tests")
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin)
