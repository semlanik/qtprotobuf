function(qtprotobuf_link_target TARGET GENERATED_TARGET)
    target_sources(${TARGET} PRIVATE $<TARGET_OBJECTS:${GENERATED_TARGET}>)
    target_include_directories(${TARGET} PRIVATE $<TARGET_PROPERTY:${GENERATED_TARGET},INTERFACE_INCLUDE_DIRECTORIES>)
    add_dependencies(${TARGET} ${GENERATED_TARGET})
endfunction()

function(qtprotobuf_generate)
    set(options MULTI QML COMMENTS FOLDER FIELDENUM ALLOW_PROTO3_OPTIONAL)
    set(oneValueArgs OUT_DIR TARGET GENERATED_TARGET EXTRA_NAMESPACE)
    set(multiValueArgs GENERATED_HEADERS EXCLUDE_HEADERS PROTO_FILES PROTO_INCLUDES)
    cmake_parse_arguments(qtprotobuf_generate "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(DEFINED qtprotobuf_generate_GENERATED_TARGET)
        set(GENERATED_TARGET_NAME ${qtprotobuf_generate_GENERATED_TARGET})
    elseif(DEFINED qtprotobuf_generate_TARGET)
        set(GENERATED_TARGET_NAME ${qtprotobuf_generate_TARGET}_qtprotobuf_gen)
    else()
        message(FATAL_ERROR "Either TARGET or GENERATED_TARGET must be specified")
    endif()

    set(deps_target ${GENERATED_TARGET_NAME}_deps)

    set(QT_PROTOBUF_EXECUTABLE $<TARGET_FILE:${QT_PROTOBUF_NAMESPACE}::qtprotobufgen>)

    #Options handling
    set(GENERATION_TYPE "SIGNLE")
    if(qtprotobuf_generate_MULTI)
        set(GENERATION_TYPE "MULTI")
        set(qtprotobuf_generate_FOLDER TRUE)
        message(STATUS "Multi-file generation is set. Folder-based generation enabled automatically for ${GENERATED_TARGET_NAME}")
    endif()
    set(GENERATION_OPTIONS ${GENERATION_TYPE})

    if(qtprotobuf_generate_QML)
        if(NOT TARGET Qt5::Qml)
            message(FATAL_ERROR "Trying to enable QML support for ${GENERATED_TARGET_NAME}, but Qt5::Qml is not a target."
                " find_package(Qt5 COMPONENTS Qml) is missing?")
        endif()
        message(STATUS "Enabling QML generation for ${GENERATED_TARGET_NAME}")
        set(GENERATION_OPTIONS "${GENERATION_OPTIONS}:QML")
    endif()

    if(qtprotobuf_generate_COMMENTS)
        message(STATUS "Enabled COMMENTS generation for ${GENERATED_TARGET_NAME}")
        set(GENERATION_OPTIONS "${GENERATION_OPTIONS}:COMMENTS")
    endif()

    if(qtprotobuf_generate_FOLDER)
        message(STATUS "Enabled FOLDER generation for ${GENERATED_TARGET_NAME}")
        set(GENERATION_OPTIONS "${GENERATION_OPTIONS}:FOLDER")
        set(FOLDER_ENABLED "FOLDER")
    endif()

    if(qtprotobuf_generate_FIELDENUM)
        message(STATUS "Enabled FIELDENUM generation for ${GENERATED_TARGET_NAME}")
        set(GENERATION_OPTIONS "${GENERATION_OPTIONS}:FIELDENUM")
    endif()

    if(qtprotobuf_generate_EXTRA_NAMESPACE)
        set(GENERATION_OPTIONS
            "${GENERATION_OPTIONS}:EXTRA_NAMESPACE=\"${qtprotobuf_generate_EXTRA_NAMESPACE}\""
        )
    endif()

    if(WIN32)
        set(PROTOC_COMMAND set QT_PROTOBUF_OPTIONS=${GENERATION_OPTIONS}&& $<TARGET_FILE:protobuf::protoc>)
    else()
        set(PROTOC_COMMAND "QT_PROTOBUF_OPTIONS=${GENERATION_OPTIONS}" $<TARGET_FILE:protobuf::protoc>)
    endif()

    set(PROTO_INCLUDES ${qtprotobuf_generate_PROTO_INCLUDES})

    if(DEFINED qtprotobuf_generate_GENERATED_HEADERS)
        set(GENERATED_HEADERS ${qtprotobuf_generate_GENERATED_HEADERS})
    else()
        find_program(GO_EXECUTABLE "go")
        if (GO_EXECUTABLE STREQUAL GO_EXECUTABLE-NOTFOUND)
            message(FATAL_ERROR "Golang is mandatory dependency for QtProtobuf if GENERATED_HEADERS is not specified. Please install it and ensure that it's accessible by PATH environment variable")
        endif()

        get_target_property(PROTO_PARSER ${QT_PROTOBUF_NAMESPACE}::qtprotobufgen PROTO_PARSER)
        if(NOT PROTO_PARSER)
            message(FATAL_ERROR "Unable to locate parsemessages.go script")
        endif()

        foreach(PROTO_FILE IN LISTS qtprotobuf_generate_PROTO_FILES)
            get_filename_component(BASE_DIR ${PROTO_FILE} DIRECTORY)
            set(PROTO_INCLUDES "-I\"${BASE_DIR}\"" ${PROTO_INCLUDES})
            execute_process(COMMAND ${GO_EXECUTABLE} run ${PROTO_PARSER} ${PROTO_FILE} ${GENERATION_TYPE} ${FOLDER_ENABLED} OUTPUT_VARIABLE GENERATED_HEADERS_PART ERROR_VARIABLE PARSER_ERROR)
            list(APPEND GENERATED_HEADERS ${GENERATED_HEADERS_PART})
        endforeach()
    endif()

    if(qtprotobuf_generate_MULTI)
        #TODO: add globalenums by default. But it's better to verify if proto file contains any global enum
        set(GENERATED_HEADERS ${GENERATED_HEADERS} globalenums.h)
    endif()

    if(NOT "${PARSER_ERROR}" STREQUAL "")
        message(FATAL_ERROR "Go parser error ${PARSER_ERROR}")
    endif()

    foreach(EXCLUDE_HEADER IN LISTS qtprotobuf_generate_EXCLUDE_HEADERS)
        list(REMOVE_ITEM GENERATED_HEADERS ${EXCLUDE_HEADER})
    endforeach()

    list(REMOVE_DUPLICATES GENERATED_HEADERS)

    if(NOT DEFINED qtprotobuf_generate_OUT_DIR)
        set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    else()
        set(OUT_DIR ${qtprotobuf_generate_OUT_DIR})
    endif()

    file(MAKE_DIRECTORY ${OUT_DIR})

    unset(GENERATED_SOURCES_FULL)
    unset(GENERATED_HEADERS_FULL)
    foreach(GENERATED_HEADER IN LISTS GENERATED_HEADERS)
        get_filename_component(GENERATED_DIRECTORY ${GENERATED_HEADER} DIRECTORY)
        get_filename_component(GENERATED_BASENAME ${GENERATED_HEADER} NAME)
        string(REGEX REPLACE "\\.[^.]*$" "" GENERATED_BASENAME ${GENERATED_BASENAME})

        if(qtprotobuf_generate_FOLDER)
            list(APPEND GENERATED_SOURCES_FULL "${OUT_DIR}/${GENERATED_DIRECTORY}/${GENERATED_BASENAME}.cpp")
            list(APPEND GENERATED_HEADERS_FULL "${OUT_DIR}/${GENERATED_DIRECTORY}/${GENERATED_BASENAME}.h")
        else()
            list(APPEND GENERATED_SOURCES_FULL "${OUT_DIR}/${GENERATED_BASENAME}.cpp")
            list(APPEND GENERATED_HEADERS_FULL "${OUT_DIR}/${GENERATED_BASENAME}.h")
        endif()
    endforeach()

    set_source_files_properties(${GENERATED_SOURCES_FULL};${GENERATED_HEADERS_FULL} PROPERTIES
        GENERATED TRUE
        SKIP_AUTOMOC ON
        SKIP_AUTOUIC ON
        SKIP_AUTOGEN ON
    )

    qt5_wrap_cpp(MOC_SOURCES ${GENERATED_HEADERS_FULL})

    add_custom_command(
            OUTPUT ${GENERATED_SOURCES_FULL} ${GENERATED_HEADERS_FULL}
            COMMAND ${PROTOC_COMMAND}
                --plugin=protoc-gen-qtprotobufgen=${QT_PROTOBUF_EXECUTABLE}
                $<IF:$<BOOL:${qtprotobuf_generate_ALLOW_PROTO3_OPTIONAL}>,--experimental_allow_proto3_optional,>
                --qtprotobufgen_out=${OUT_DIR}
                ${PROTO_INCLUDES}
                ${qtprotobuf_generate_PROTO_FILES}
            WORKING_DIRECTORY ${OUT_DIR}
            DEPENDS ${qtprotobuf_generate_PROTO_FILES} ${QT_PROTOBUF_EXECUTABLE}
            COMMENT "Generating QtProtobuf ${GENERATED_TARGET_NAME} sources..."
            COMMAND_EXPAND_LISTS
    )

    add_custom_target(${deps_target} DEPENDS ${GENERATED_SOURCES_FULL} ${GENERATED_HEADERS_FULL}
        ${qtprotobuf_generate_PROTO_FILES}
    )

    add_library(${GENERATED_TARGET_NAME} OBJECT ${GENERATED_SOURCES_FULL} ${MOC_SOURCES})
    add_dependencies(${GENERATED_TARGET_NAME} ${deps_target})
    if(qtprotobuf_generate_TARGET)
        set_property(TARGET ${qtprotobuf_generate_TARGET} APPEND PROPERTY PUBLIC_HEADER ${GENERATED_HEADERS_FULL})
    endif()
    set_target_properties(${GENERATED_TARGET_NAME} PROPERTIES PUBLIC_HEADER "${GENERATED_HEADERS_FULL}")

    #Add include directories in case if targets are found by find_project or in source tree
    target_include_directories(${GENERATED_TARGET_NAME} PUBLIC ${OUT_DIR} PRIVATE
        $<TARGET_PROPERTY:${QT_PROTOBUF_NAMESPACE}::Protobuf,INTERFACE_INCLUDE_DIRECTORIES>)

    #TODO: Do not link targets if they are not used in .proto files.
    if(TARGET ${QT_PROTOBUF_NAMESPACE}::Grpc)
        target_include_directories(${GENERATED_TARGET_NAME} PRIVATE
            $<TARGET_PROPERTY:${QT_PROTOBUF_NAMESPACE}::Grpc,INTERFACE_INCLUDE_DIRECTORIES>)
    endif()

    if(TARGET ${QT_PROTOBUF_NAMESPACE}::ProtobufWellKnownTypes)
        target_include_directories(${GENERATED_TARGET_NAME} PRIVATE
            $<TARGET_PROPERTY:${QT_PROTOBUF_NAMESPACE}::ProtobufWellKnownTypes,INTERFACE_INCLUDE_DIRECTORIES>)
    endif()

    if(TARGET ${QT_PROTOBUF_NAMESPACE}::ProtobufQtTypes)
        target_include_directories(${GENERATED_TARGET_NAME} PRIVATE
            $<TARGET_PROPERTY:${QT_PROTOBUF_NAMESPACE}::ProtobufQtTypes,INTERFACE_INCLUDE_DIRECTORIES>)
    endif()

    #Automatically link whole static library to specified in parameters target
    if(DEFINED qtprotobuf_generate_TARGET)
        qtprotobuf_link_target(${qtprotobuf_generate_TARGET} ${GENERATED_TARGET_NAME})
    endif()
endfunction()
