set(testgeneration "test_generation")
set(TESTS_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
file(MAKE_DIRECTORY ${TESTS_OUT_DIR})
file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto)
# test sources generation
if(WIN32)
    set(Protobuf_PROTOC_EXECUTABLE ${PROTOBUF_INSTALATION_PATH}/bin/protoc.exe)
endif()

add_custom_target(${testgeneration})
add_custom_command(TARGET ${testgeneration}
        COMMAND ${Protobuf_PROTOC_EXECUTABLE} --plugin=protoc-gen-${PROJECT_NAME}=$<TARGET_FILE:${PROJECT_NAME}> --qtprotobuf_out=${TESTS_OUT_DIR} -I=${CMAKE_CURRENT_SOURCE_DIR}/proto/ ${PROTO_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/proto/
        DEPENDS ${PROJECT_NAME} ${PROTO_FILES}
        COMMENT "Generating test headers"
)
add_dependencies(${testgeneration} ${PROJECT_NAME})

# test sources build
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC ON)

if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

file(GLOB GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/*.h)
file(GLOB GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/*.cpp)

#Manual moc_ files generation for qt
list(FILTER GENERATED_SOURCES EXCLUDE REGEX "moc_.*cpp")
QT5_WRAP_CPP(MOC_SOURCES ${GENERATED_HEADERS})

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src/protobuf ${CMAKE_SOURCE_DIR}/src/grpc)

if(WIN32)
    #Set  path to GTest build libraries
    set(GTEST_BOTH_LIBRARIES "")
    #Set  path to GTest include directory
    include_directories(${GTEST_INCLUDE_PATHS} "/")
endif()

file(GLOB SOURCES main.cpp
    simpletest.cpp
    serializationtest.cpp
    deserializationtest.cpp
    servertest.cpp
    clienttest.cpp)

set(testtarget "qtprotobuf_test")
add_executable(${testtarget} ${SOURCES} ${MOC_SOURCES} ${GENERATED_SOURCES})
if(WIN32)
    target_link_libraries(${testtarget} "${GTEST_BOTH_LIBRARIES}/gmock_main.lib" "${GTEST_BOTH_LIBRARIES}/gmock.lib" qtprotobufsupport qtgrpc Qt5::Core)
elseif(UNIX)
    target_link_libraries(${testtarget} gtest qtgrpc)
endif()
add_dependencies(${testtarget} ${testgeneration})
