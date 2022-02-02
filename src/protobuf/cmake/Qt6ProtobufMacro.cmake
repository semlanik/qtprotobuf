# TODO Qt6: give the variable a proper name while migrating to Qt6
set(_qt_internal_qtprotobufgen_cmake_dir "${CMAKE_CURRENT_LIST_DIR}")

function(_qt_internal_link_protobuf_objects target)
    foreach(generated_target IN LISTS ARGN)
        target_sources(${target} PRIVATE $<TARGET_OBJECTS:${generated_target}>)
        target_include_directories(${target} PRIVATE $<TARGET_PROPERTY:${generated_target},INTERFACE_INCLUDE_DIRECTORIES>)
        add_dependencies(${target} ${generated_target})
    endforeach()
endfunction()

# TODO Qt6: Use WrapProtobuf instead
find_package(Protobuf)

# TODO Qt6:
#     - Collect PROTO_INCLUDES from the LINK_LIBRARIES property of TARGET
#     - Compile static/shared library instead of object library
#     - Move protobuf types registration to a separate register file, like it's done for QML
#       and build only this file into OBJECT library.
#     - Collect proto files from the source files of the ${TARGET}
function(qt6_protobuf_generate)
    set(options MULTI QML COMMENTS FOLDER FIELDENUM)
    set(oneValueArgs OUTPUT_DIRECTORY TARGET GENERATED_TARGET EXTRA_NAMESPACE PROTO_FILES_BASE_DIR)
    set(multiValueArgs EXCLUDE_HEADERS PROTO_FILES PROTO_INCLUDES)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    unset(proto_files)
    foreach(proto_file IN LISTS arg_PROTO_FILES)
        if(NOT IS_ABSOLUTE "${proto_file}")
            if(arg_PROTO_FILES_BASE_DIR)
                set(proto_file "${arg_PROTO_FILES_BASE_DIR}/${proto_file}")
            endif()
            get_filename_component(proto_file "${proto_file}" ABSOLUTE)
        endif()
        list(APPEND proto_files "${proto_file}")
    endforeach()
    list(REMOVE_DUPLICATES proto_files)

    if("${proto_files}" STREQUAL "")
        message(FATAL_ERROR "PROTO_FILES list is empty.")
    endif()


    if(DEFINED arg_GENERATED_TARGET)
        set(generated_target_name ${arg_GENERATED_TARGET})
    elseif(TARGET ${arg_TARGET})
        set(generated_target_name ${arg_TARGET}_qtprotobuf_gen)
    else()
        message(FATAL_ERROR "Either 'TARGET' or 'GENERATED_TARGET' argument needs to be set.")
    endif()

    set(generation_options "")
    set(extra_pre_parse_options "")

    set(generation_type "SIGNLE")
    if(arg_MULTI)
        set(generation_type "MULTI")
        list(APPEND extra_pre_parse_options "-DMULTI=TRUE")
        set(arg_FOLDER TRUE)
        message(STATUS "Enabling Multi-file generation. Folder-based generation is enabled"
            " automatically for ${generated_target_name}.")
    endif()
    list(APPEND generation_options "${generation_type}")

    if(arg_QML)
        if(NOT TARGET Qt::Qml)
            message(FATAL_ERROR "Trying to enable QML support for '${generated_target_name}',"
                " but Qt::Qml is not a target."
                "\nfind_package(Qt<VERSION> COMPONENTS Qml) is missing?"
            )
        endif()
        message(STATUS "Enabling QML generation for ${generated_target_name}")
        list(APPEND generation_options "QML")
    endif()

    if(arg_COMMENTS)
        message(STATUS "Enabling COMMENTS generation for ${generated_target_name}")
        list(APPEND generation_options "COMMENTS")
    endif()

    if(arg_FOLDER)
        message(STATUS "Enabling FOLDER generation for ${generated_target_name}")
        list(APPEND generation_options "FOLDER")
        list(APPEND extra_pre_parse_options "-DFOLDER=TRUE")
    endif()

    if(arg_FIELDENUM)
        message(STATUS "Enabling FIELDENUM generation for ${generated_target_name}")
        list(APPEND generation_options "FIELDENUM")
    endif()

    list(JOIN generation_options ":" generation_options_string)
    if(arg_EXTRA_NAMESPACE)
        set(generation_options_string "${generation_options_string}:EXTRA_NAMESPACE=\"${arg_EXTRA_NAMESPACE}\"")
    endif()

    if(WIN32)
        set(PROTOC_COMMAND set QT_PROTOBUF_OPTIONS=${generation_options_string}&& $<TARGET_FILE:protobuf::protoc>)
    else()
        set(PROTOC_COMMAND "QT_PROTOBUF_OPTIONS=${generation_options_string}" $<TARGET_FILE:protobuf::protoc>)
    endif()

    set(proto_includes ${arg_PROTO_INCLUDES})

    set(pre_parse_script "${_qt_internal_qtprotobufgen_cmake_dir}/QtPreParseProtoFile.cmake")

    if(NOT EXISTS "${pre_parse_script}")
        message(FATAL_ERROR "Unable to locate the QtPreParseProtoFile script"
            " ${pre_parse_script}.")
    endif()

    unset(generated_files)
    foreach(proto_file IN LISTS proto_files)
        get_filename_component(proto_file_base_dir "${proto_file}" DIRECTORY)
        list(PREPEND proto_includes "-I\"${proto_file_base_dir}\"")
        execute_process(COMMAND ${CMAKE_COMMAND}
            "${extra_pre_parse_options}"
            -DPROTO_FILE=${proto_file}
            -P ${pre_parse_script}
            OUTPUT_VARIABLE output
            ERROR_VARIABLE error_output # CMake writes messages to standard error.
            RESULT_VARIABLE result)
        string(STRIP "${output}" output)
        string(STRIP "${error_output}" error_output)
        if(NOT result EQUAL 0)
            message(FATAL_ERROR "Unable to parse '${proto_file}': ${error_output}")
        endif()
        list(APPEND generated_files ${output} ${error_output})
    endforeach()

    if(NOT generated_files)
        message(FATAL_ERROR "Unable to get the list of generated sources for target"
            " '${generated_target_name}'")
    endif()

    if(arg_MULTI)
        # TODO: add globalenums by default. But it's better to verify if proto file contains a
        # global enum.
        list(APPEND generated_files globalenums.h)
    endif()

    foreach(header IN LISTS arg_EXCLUDE_HEADERS)
        list(REMOVE_ITEM generated_files ${header})
    endforeach()

    list(REMOVE_DUPLICATES generated_files)

    if(NOT DEFINED arg_OUTPUT_DIRECTORY OR "${arg_OUTPUT_DIRECTORY}" STREQUAL "")
        set(out_dir ${CMAKE_CURRENT_BINARY_DIR})
    else()
        set(out_dir ${arg_OUTPUT_DIRECTORY})
    endif()

    file(MAKE_DIRECTORY ${out_dir})

    list(TRANSFORM generated_files PREPEND "${out_dir}/")

    set_source_files_properties(${generated_files} PROPERTIES
        GENERATED TRUE
        SKIP_AUTOGEN ON
        SKIP_AUTOMOC ON
        SKIP_AUTORCC ON
        SKIP_AUTOUIC ON
    )

    # Filter generated headers
    unset(generated_headers)
    foreach(header IN LISTS generated_files)
        get_filename_component(extension "${header}" LAST_EXT)
        if(extension STREQUAL ".h")
            list(APPEND generated_headers "${header}")
        endif()
    endforeach()

    qt_wrap_cpp(moc_sources ${generated_headers})

    get_property(num_deps GLOBAL PROPERTY ${generated_target_name}_deps_num)
    if(NOT num_deps)
        set(num_deps 0)
    endif()
    set(deps_target ${generated_target_name}_deps_${num_deps})

    math(EXPR num_deps "${num_deps} + 1")
    set_property(GLOBAL PROPERTY ${generated_target_name}_deps_num "${num_deps}")

    add_custom_command(OUTPUT ${generated_files}
        COMMAND ${PROTOC_COMMAND}
            --plugin=protoc-gen-qtprotobufgen=$<TARGET_FILE:${QT_CMAKE_EXPORT_NAMESPACE}::qtprotobufgen>
            --qtprotobufgen_out=${out_dir}
            ${proto_includes}
            ${proto_files}
        WORKING_DIRECTORY ${out_dir}
        DEPENDS
        ${QT_CMAKE_EXPORT_NAMESPACE}::qtprotobufgen
            ${proto_files}
        COMMENT "Generating QtProtobuf ${generated_target_name} sources..."
        COMMAND_EXPAND_LISTS
    )
    add_custom_target(${deps_target} DEPENDS ${generated_files})

    if(NOT TARGET ${generated_target_name})
        add_library(${generated_target_name} OBJECT ${generated_files} ${moc_sources})
    else()
        target_sources(${generated_target_name} PRIVATE ${generated_files} ${moc_sources})
    endif()

    add_dependencies(${generated_target_name} ${deps_target})

    set_target_properties(${generated_target_name} PROPERTIES PUBLIC_HEADER "${generated_headers}")

    #Add include directories in case if targets are found by find_project or in source tree
    target_include_directories(${generated_target_name}
        PRIVATE $<TARGET_PROPERTY:${QT_CMAKE_EXPORT_NAMESPACE}::Protobuf,INTERFACE_INCLUDE_DIRECTORIES>)

    #TODO: Do not link targets if they are not used in .proto files.
    if(TARGET ${QT_CMAKE_EXPORT_NAMESPACE}::Grpc)
        target_include_directories(${generated_target_name} PRIVATE
            $<TARGET_PROPERTY:${QT_CMAKE_EXPORT_NAMESPACE}::Grpc,INTERFACE_INCLUDE_DIRECTORIES>)
    endif()

    if(TARGET ${QT_CMAKE_EXPORT_NAMESPACE}::ProtobufWellKnownTypes)
        target_include_directories(${generated_target_name} PRIVATE
            $<TARGET_PROPERTY:${QT_CMAKE_EXPORT_NAMESPACE}::ProtobufWellKnownTypes,INTERFACE_INCLUDE_DIRECTORIES>)
    endif()

    if(TARGET ${QT_CMAKE_EXPORT_NAMESPACE}::ProtobufQtTypes)
        target_include_directories(${generated_target_name} PRIVATE
            $<TARGET_PROPERTY:${QT_CMAKE_EXPORT_NAMESPACE}::ProtobufQtTypes,INTERFACE_INCLUDE_DIRECTORIES>)
    endif()

    target_include_directories(${generated_target_name} PUBLIC "$<BUILD_INTERFACE:${out_dir}>")

    # Automatically link whole static library to specified in parameters target
    if(TARGET ${arg_TARGET})
        _qt_internal_link_protobuf_objects(${arg_TARGET} ${generated_target_name})
        target_include_directories(${arg_TARGET} PUBLIC "$<BUILD_INTERFACE:${out_dir}>")
    endif()
endfunction()

if(NOT QT_NO_CREATE_VERSIONLESS_FUNCTIONS)
    function(qt_protobuf_generate)
        qt6_protobuf_generate(${ARGV})
    endfunction()
endif()
