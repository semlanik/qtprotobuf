function(protobuf_generate_all)
    set(options)
    set(oneValueArgs OUT_DIR TARGET)
    set(multiValueArgs GENERATED_SOURCES PROTO_FILES)
    cmake_parse_arguments(protobuf_generate_all "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(GEN_TARGET "${protobuf_generate_all_TARGET}_generate")

    foreach(PROTO_FILE IN LISTS protobuf_generate_all_PROTO_FILES)
        get_filename_component(BASE_DIR ${PROTO_FILE} DIRECTORY)
        set(PROTO_INCLUDES -I"${BASE_DIR}" ${PROTO_INCLUDES})
    endforeach()

    if(NOT DEFINED protobuf_generate_all_OUT_DIR)
        set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    else()
        set(OUT_DIR ${protobuf_generate_all_OUT_DIR})
    endif()

    file(MAKE_DIRECTORY ${OUT_DIR})

    if(NOT TARGET gRPC::grpc_cpp_plugin)
        message(FATAL_ERROR "gRPC plugin is not found")
    endif()

    add_custom_command(
        OUTPUT ${GENERATED_SOURCES}
        COMMAND $<TARGET_FILE:protobuf::protoc>
        ARGS --grpc_out="${OUT_DIR}"
            --cpp_out="${OUT_DIR}"
            ${PROTO_INCLUDES}
            --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
            ${protobuf_generate_all_PROTO_FILES}
        DEPENDS ${protobuf_generate_all_PROTO_FILES}
    )

    add_custom_target(${GEN_TARGET} DEPENDS ${protobuf_generate_all_PROTO_FILES} ${GENERATED_SOURCES})
    add_dependencies(${protobuf_generate_all_TARGET} ${GEN_TARGET})
endfunction(protobuf_generate_all)

function(add_target_qml)
    set(options)
    set(oneValueArgs TARGET QMLDIR_FILE)
    set(multiValueArgs QML_FILES)
    cmake_parse_arguments(add_target_qml "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_custom_target(${add_target_qml_TARGET}_qml DEPENDS ${add_qml_QML_FILES} ${add_target_qml_QMLDIR_FILE})
    foreach(QML_FILE IN LISTS add_target_qml_QML_FILES)
        add_custom_command(TARGET ${add_target_qml_TARGET}_qml COMMAND ${CMAKE_COMMAND} -E copy ${QML_FILE}
            ${CMAKE_CURRENT_BINARY_DIR})
    endforeach()
    if(DEFINED ${add_target_qml_QMLDIR_FILE})
        add_custom_command(TARGET ${add_target_qml_TARGET}_qml COMMAND ${CMAKE_COMMAND} -E copy ${add_qml_QMLDIR_FILE}
            ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    add_dependencies(${add_target_qml_TARGET} ${add_target_qml_TARGET}_qml)
endfunction()

function(add_target_windeployqt)
if(WIN32)
    set(options)
    set(oneValueArgs QML_DIR TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(add_target_windeployqt "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    find_program(WINDEPLOYQT_EXECUTABLE "windeployqt" PATHS ${PATH})
    if(WINDEPLOYQT_EXECUTABLE STREQUAL WINDEPLOYQT_EXECUTABLE-NOTFOUND)
        message(WARNING "windeployqt is not found in specified PATH! Please, copy dependencies manually")
    else()
        if(DEFINED add_target_windeployqt_QML_DIR)
            set(QML_DIR --qmldir ${add_target_windeployqt_QML_DIR})
        endif()
        add_custom_command(TARGET ${add_target_windeployqt_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:QtProtobuf> $<TARGET_FILE_DIR:${add_target_windeployqt_TARGET}>
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:QtGrpc> $<TARGET_FILE_DIR:${add_target_windeployqt_TARGET}>
            COMMAND ${WINDEPLOYQT_EXECUTABLE} ${QML_DIR} $<TARGET_FILE_DIR:${add_target_windeployqt_TARGET}>)
    endif()
endif()
endfunction()

if(NOT DEFINED QT_QMAKE_EXECUTABLE)
    find_program(QT_QMAKE_EXECUTABLE "qmake")
    if(QT_QMAKE_EXECUTABLE STREQUAL QT_QMAKE_EXECUTABLE-NOTFOUND)
        message(FATAL_ERROR "Could not find qmake executable")
    endif()
endif()

function(extract_qt_variable VARIABLE)
    execute_process(
        COMMAND ${QT_QMAKE_EXECUTABLE} -query ${VARIABLE}
        OUTPUT_VARIABLE ${VARIABLE}
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endfunction()
