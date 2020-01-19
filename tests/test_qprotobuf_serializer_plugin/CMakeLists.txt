set(TARGET qtprotobuf_plugin_test)

include(${QTPROTOBUF_CMAKE_DIR}/QtProtobufTest.cmake)

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Test REQUIRED)

file(GLOB SOURCES
    serializationplugintest.cpp)

if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} PRIVATE gtest_main gtest ${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobuf ${QTPROTOBUF_COMMON_NAMESPACE}::QtGrpc Qt5::Core Qt5::Test Qt5::Network ${CMAKE_THREAD_LIBS_INIT})
qtprotobuf_link_archive(${TARGET} qtprotobuf_test_qtprotobuf_gen)
add_target_windeployqt(TARGET ${TARGET}
    QML_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("serialization")

add_test(NAME ${TARGET} COMMAND ${TARGET})
set_tests_properties(${TARGET} PROPERTIES
    ENVIRONMENT QT_PROTOBUF_PLUGIN_PATH=$<TARGET_FILE_DIR:serializationplugin>)

