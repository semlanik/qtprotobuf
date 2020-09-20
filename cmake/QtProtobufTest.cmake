find_package(${QT_PROTOBUF_PROJECT} COMPONENTS QtProtobuf QtGrpc REQUIRED)

include(${QT_PROTOBUF_CMAKE_DIR}/QtProtobufCommon.cmake)

function(add_test_target)
    set(options MULTI QML)
    set(oneValueArgs QML_DIR TARGET)
    set(multiValueArgs SOURCES GENERATED_HEADERS EXCLUDE_HEADERS PROTO_FILES PROTO_INCLUDES)
    cmake_parse_arguments(add_test_target "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    find_package(Threads REQUIRED)
    find_package(Qt5 COMPONENTS Test REQUIRED)
    ## test sources build
    # policy enables automoc for generated files
    if(${CMAKE_VERSION} VERSION_GREATER "3.10.0")
        cmake_policy(SET CMP0071 NEW)
    endif()

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

    qtprotobuf_generate(TARGET ${add_test_target_TARGET}
        OUT_DIR ${GENERATED_SOURCES_DIR}
        PROTO_FILES ${PROTO_FILES}
        GENERATED_HEADERS ${add_test_target_GENERATED_HEADERS}
        EXCLUDE_HEADERS ${add_test_target_EXCLUDE_HEADERS}
        ${EXTRA_OPTIONS}
        PROTO_INCLUDES ${add_test_target_PROTO_INCLUDES})

    if(Qt5_POSITION_INDEPENDENT_CODE)
        set_target_properties(${add_test_target_TARGET} PROPERTIES POSITION_INDEPENDENT_CODE FALSE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif()
    target_link_libraries(${add_test_target_TARGET} PUBLIC gtest_main gtest ${QT_PROTOBUF_PROJECT}::QtProtobuf ${QT_PROTOBUF_PROJECT}::QtGrpc Qt5::Core Qt5::Test Qt5::Network ${CMAKE_THREAD_LIBS_INIT})
    if (${add_test_target_QML})
        target_link_libraries(${add_test_target_TARGET} PUBLIC Qt5::Qml)
    endif()
endfunction(add_test_target)
