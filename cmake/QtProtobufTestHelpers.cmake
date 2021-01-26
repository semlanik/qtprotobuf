function(qt_protobuf_internal_protobuf_generate_all)
    set(options)
    set(oneValueArgs OUT_DIR TARGET)
    set(multiValueArgs GENERATED_SOURCES PROTO_FILES)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(GEN_TARGET "${arg_TARGET}_generate")

    foreach(PROTO_FILE IN LISTS arg_PROTO_FILES)
        get_filename_component(BASE_DIR "${PROTO_FILE}" DIRECTORY)
        list(APPEND PROTO_INCLUDES "-I${BASE_DIR}")
    endforeach()

    if(NOT DEFINED arg_OUT_DIR)
        set(OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    else()
        set(OUT_DIR "${arg_OUT_DIR}")
    endif()

    file(MAKE_DIRECTORY "${OUT_DIR}")

    if(NOT TARGET gRPC::grpc_cpp_plugin)
        message(FATAL_ERROR "gRPC plugin is not found")
    endif()

    add_custom_command(
        OUTPUT ${GENERATED_SOURCES}
        COMMAND protobuf::protoc
        ARGS --grpc_out="${OUT_DIR}"
            --cpp_out="${OUT_DIR}"
            ${PROTO_INCLUDES}
            "--plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
            ${arg_PROTO_FILES}
        DEPENDS ${arg_PROTO_FILES}
    )

    add_custom_target(${GEN_TARGET} DEPENDS ${arg_PROTO_FILES} ${GENERATED_SOURCES})
    add_dependencies(${arg_TARGET} ${GEN_TARGET})
endfunction()

function(qt_protobuf_internal_add_target_qml)
    set(options)
    set(oneValueArgs TARGET QMLDIR_FILE)
    set(multiValueArgs QML_FILES)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_custom_target(${arg_TARGET}_qml DEPENDS ${arg_QML_FILES} ${arg_QMLDIR_FILE})
    foreach(QML_FILE IN LISTS arg_QML_FILES)
        add_custom_command(TARGET ${arg_TARGET}_qml
            COMMAND ${CMAKE_COMMAND} -E copy "${QML_FILE}" "${CMAKE_CURRENT_BINARY_DIR}"
        )
    endforeach()
    if(DEFINED ${arg_QMLDIR_FILE})
        add_custom_command(TARGET ${arg_TARGET}_qml
            COMMAND ${CMAKE_COMMAND} -E copy "${arg_QMLDIR_FILE}" "${CMAKE_CURRENT_BINARY_DIR}"
        )
    endif()
    add_dependencies(${arg_TARGET} ${arg_TARGET}_qml)
endfunction()

function(qt_protobuf_internal_add_target_windeployqt)
    if(WIN32)
        set(options)
        set(oneValueArgs "QML_DIR" "TARGET")
        set(multiValueArgs)
        cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
        find_program(WINDEPLOYQT_EXECUTABLE "windeployqt" PATHS ${PATH})
        if(WINDEPLOYQT_EXECUTABLE)
            if(DEFINED arg_QML_DIR)
                set(QML_DIR --qmldir ${arg_QML_DIR})
            endif()
            add_custom_command(TARGET ${arg_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND}
                        -E copy $<TARGET_FILE:${QT_PROTOBUF_NAMESPACE}::Protobuf>
                        $<TARGET_FILE_DIR:${arg_TARGET}>
                COMMAND ${CMAKE_COMMAND}
                         -E copy $<TARGET_FILE:${QT_PROTOBUF_NAMESPACE}::Grpc>
                         $<TARGET_FILE_DIR:${arg_TARGET}>
                COMMAND ${WINDEPLOYQT_EXECUTABLE}
                        ${QML_DIR} $<TARGET_FILE_DIR:${arg_TARGET}>)
        else()
            message(WARNING "windeployqt is not found in specified PATH! \
Please, copy dependencies manually")
        endif()
    endif()
endfunction()

macro(qt_protobuf_internal_find_dependencies)
    find_package(${QT_VERSIONED_PREFIX} COMPONENTS Core Qml Network REQUIRED)
    find_package(Threads REQUIRED)
    if(NOT TARGET ${QT_PROTOBUF_NAMESPACE}::Protobuf)
        find_package(${QT_PROTOBUF_NAMESPACE} COMPONENTS Protobuf REQUIRED)
    endif()
    if(NOT TARGET ${QT_PROTOBUF_NAMESPACE}::Grpc)
        find_package(${QT_PROTOBUF_NAMESPACE} COMPONENTS Grpc REQUIRED)
    endif()
    if(QT_PROTOBUF_STATIC)
        add_definitions(-DQT_PROTOBUF_STATIC)# add_definitions is used because old cmake versions
                                             # compatibility
    endif()

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
endmacro()