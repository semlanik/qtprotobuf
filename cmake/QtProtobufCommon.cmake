function(protobuf_generate_all)
    set(options)
    set(oneValueArgs OUT_DIR TARGET)
    set(multiValueArgs GENERATED_SOURCES PROTO_FILES)
    cmake_parse_arguments(protobuf_generate_all "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(GEN_TARGET "${protobuf_generate_all_TARGET}_generate")
    add_custom_target(${GEN_TARGET})

    foreach(PROTO_FILE IN LISTS protobuf_generate_all_PROTO_FILES)
        get_filename_component(BASE_DIR ${PROTO_FILE} DIRECTORY)
        set(PROTO_INCLUDES -I"${BASE_DIR}" ${PROTO_INCUDES})
    endforeach()

    if(NOT DEFINED protobuf_generate_all_OUT_DIR)
        set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    else()
        set(OUT_DIR ${protobuf_generate_all_OUT_DIR})
    endif()

    file(MAKE_DIRECTORY ${OUT_DIR})

    if(NOT DEFINED gRPC_CPP_PLUGIN_EXECUTABLE)
        message(FATAL_ERROR "gRPC plugin is not found set full path to grpc_cpp_plugin using -DgRPC_CPP_PLUGIN_EXECUTABLE=</path/to/grpc_cpp_plugin>")
    endif()

    add_custom_command(
        TARGET ${GEN_TARGET}
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        ARGS --grpc_out="${OUT_DIR}"
        --cpp_out="${OUT_DIR}"
        ${PROTO_INCLUDES}
        --plugin=protoc-gen-grpc="${gRPC_CPP_PLUGIN_EXECUTABLE}"
        ${protobuf_generate_all_PROTO_FILES}
        DEPENDS ${protobuf_generate_all_PROTO_FILES})
    add_dependencies(${protobuf_generate_all_TARGET} ${GEN_TARGET})
endfunction(protobuf_generate_all)

function(add_test_target)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs SOURCES GENERATED_HEADERS)
    cmake_parse_arguments(add_test_target "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    ## test sources build
    # policy enables automoc for generated files
    if(${CMAKE_VERSION} VERSION_GREATER "3.10.0")
        cmake_policy(SET CMP0071 NEW)
    endif()

    set(GENERATED_SOURCES_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

    file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto)

    generate_qtprotobuf(TARGET ${add_test_target_TARGET}
        OUT_DIR ${GENERATED_SOURCES_DIR}
        PROTO_FILES ${PROTO_FILES}
        GENERATED_HEADERS ${add_test_target_GENERATED_HEADERS})

    add_executable(${add_test_target_TARGET} ${add_test_target_SOURCES})
    if(Qt5_POSITION_INDEPENDENT_CODE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
        set_target_properties(${add_test_target_TARGET} PROPERTIES POSITION_INDEPENDENT_CODE FALSE)
    endif()
    add_dependencies(${add_test_target_TARGET} ${QtProtobuf_GENERATED})
    target_link_libraries(${add_test_target_TARGET} gtest_main gtest ${QtProtobuf_GENERATED} ${GRPC_LIBRARY_TARGET} ${PROTOBUF_LIBRARY_TARGET} Qt5::Core Qt5::Qml Qt5::Network)
    target_include_directories(${add_test_target_TARGET} PRIVATE ${GENERATED_SOURCES_DIR})
endfunction(add_test_target)
