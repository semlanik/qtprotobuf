include(${QT_PROTOBUF_CMAKE_DIR}/QtProtobufCommon.cmake)

macro(qt_protobuf_internal_find_dependencies)
    find_package(Qt5 COMPONENTS Core Qml Network REQUIRED)
    find_package(Threads REQUIRED)
    if(NOT TARGET ${QT_PROTOBUF_PROJECT}::QtProtobuf)
        find_package(${QT_PROTOBUF_PROJECT} COMPONENTS QtProtobuf REQUIRED)
    endif()
    if(NOT TARGET ${QT_PROTOBUF_PROJECT}::QtGrpc)
        find_package(${QT_PROTOBUF_PROJECT} COMPONENTS QtGrpc REQUIRED)
    endif()
    if(QT_PROTOBUF_STATIC)
        add_compile_definitions(QT_PROTOBUF_STATIC)#Old cmake versions compatibility
    endif()

    set(CMAKE_POSITION_INDEPENDENT_CODE ${Qt5_POSITION_INDEPENDENT_CODE})
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
endmacro()


function(qt_protobuf_internal_add_test_target)
    set(options MULTI QML FIELDENUM)
    set(oneValueArgs QML_DIR TARGET)
    set(multiValueArgs SOURCES GENERATED_HEADERS EXCLUDE_HEADERS PROTO_FILES PROTO_INCLUDES)
    cmake_parse_arguments(add_test_target "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    find_package(Qt5 COMPONENTS Test REQUIRED)

    ## test sources build
    # policy enables automoc for generated files
    if(${CMAKE_VERSION} VERSION_GREATER "3.10.0")
        cmake_policy(SET CMP0071 NEW)
    endif()

    set(CMAKE_POSITION_INDEPENDENT_CODE ${Qt5_POSITION_INDEPENDENT_CODE})

    set(GENERATED_SOURCES_DIR ${CMAKE_CURRENT_BINARY_DIR}/${add_test_target_TARGET}_generated)

    if(DEFINED add_test_target_PROTO_FILES)
        file(GLOB PROTO_FILES ABSOLUTE ${add_test_target_PROTO_FILES})
    else()
        file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto)
    endif()

    add_executable(${add_test_target_TARGET} ${add_test_target_SOURCES})

    set(EXTRA_OPTIONS COMMENTS)
    if(add_test_target_MULTI)
        set(EXTRA_OPTIONS ${EXTRA_OPTIONS} MULTI)
    endif()
    if(add_test_target_QML)
        set(EXTRA_OPTIONS ${EXTRA_OPTIONS} QML)
    endif()
    if(add_test_target_FIELDENUM)
        set(EXTRA_OPTIONS ${EXTRA_OPTIONS} FIELDENUM)
    endif()

    qtprotobuf_generate(TARGET ${add_test_target_TARGET}
        OUT_DIR ${GENERATED_SOURCES_DIR}
        PROTO_FILES ${PROTO_FILES}
        GENERATED_HEADERS ${add_test_target_GENERATED_HEADERS}
        EXCLUDE_HEADERS ${add_test_target_EXCLUDE_HEADERS}
        ${EXTRA_OPTIONS}
        PROTO_INCLUDES ${add_test_target_PROTO_INCLUDES})

    target_link_libraries(${add_test_target_TARGET} PUBLIC gtest_main
                                                           gtest
                                                           ${QT_PROTOBUF_PROJECT}::QtProtobuf
                                                           ${QT_PROTOBUF_PROJECT}::QtGrpc
                                                           Qt5::Core
                                                           Qt5::Test
                                                           Qt5::Network
                                                           ${CMAKE_THREAD_LIBS_INIT})
    if (${add_test_target_QML})
        target_link_libraries(${add_test_target_TARGET} PUBLIC Qt5::Qml)
    endif()
endfunction()

function(qt_protobuf_internal_add_example_target)
    set(options)
    set(oneValueArgs TARGET QML_DIR)
    set(multiValueArgs SOURCES PROTO_FILES RESOURCES)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    find_package(Qt5 COMPONENTS Quick REQUIRED)

    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    file(GLOB PROTO_FILES ABSOLUTE ${arg_PROTO_FILES})
    file(GLOB SOURCES ${arg_SOURCES})

    add_executable(${TARGET} ${SOURCES} ${arg_RESOURCES})
    qtprotobuf_generate(TARGET ${TARGET}
        OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated
        PROTO_FILES ${PROTO_FILES}
        QML)

    add_target_windeployqt(TARGET ${TARGET} QML_DIR ${arg_QML_DIR})

    target_link_libraries(${TARGET} PRIVATE examples_common
                                            QtProtobufProject::QtProtobuf
                                            QtProtobufProject::QtGrpc
                                            Qt5::Quick
                                            Qt5::Qml)
endfunction()
