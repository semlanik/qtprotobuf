function(generate_qtprotobuf)
    set(options)
    set(oneValueArgs OUT_DIR TARGET MULTI QML GENERATED_HEADERS_VAR)
    set(multiValueArgs GENERATED_HEADERS EXCLUDE_HEADERS PROTO_FILES PROTO_INCLUDES)
    cmake_parse_arguments(generate_qtprotobuf "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(QtProtobuf_GENERATED ${generate_qtprotobuf_TARGET}_qtprotobuf_gen PARENT_SCOPE)
    set(QtProtobuf_GENERATED ${generate_qtprotobuf_TARGET}_qtprotobuf_gen)

    set(GEN_TARGET ${generate_qtprotobuf_TARGET}_qtprotobuf_generate)
    set(GENERATED_HEADERS ${generate_qtprotobuf_GENERATED_HEADERS})

    if(NOT DEFINED QTPROTOBUF_EXECUTABLE)
        set(QTPROTOBUF_EXECUTABLE "${QTPROTOBUF_EXECUTABLE_INSTALL}")
    endif()

    set(GENERATION_TYPE "SINGLE")
    if("${generate_qtprotobuf_MULTI}" STREQUAL "TRUE")
        set(GENERATION_TYPE "MULTI")
        #TODO: add globalenums by default. But it's better to verify if proto file contains any global enum
        set(GENERATED_HEADERS ${GENERATED_HEADERS} globalenums.h)
    endif()

    set(GENERATION_OPTIONS ${GENERATION_TYPE})
    if("${generate_qtprotobuf_QML}" STREQUAL "TRUE")
        message(STATUS "Enabled QML generation for ${generate_qtprotobuf_TARGET}")
        set(GENERATION_OPTIONS "${GENERATION_OPTIONS}:QML")
    endif()

    find_program(GO_EXECUTABLE "go")
    if (GO_EXECUTABLE STREQUAL GO_EXECUTABLE-NOTFOUND)
        message(FATAL_ERROR "Golang is mandatory dependency for QtProtobuf. Please install it and ensure that it's accessible by PATH environment variable")
    endif()

    set(PROTO_INCLUDES ${generate_qtprotobuf_PROTO_INCLUDES})
    foreach(PROTO_FILE IN LISTS generate_qtprotobuf_PROTO_FILES)
        get_filename_component(BASE_DIR ${PROTO_FILE} DIRECTORY)
        set(PROTO_INCLUDES -I"${BASE_DIR}" ${PROTO_INCLUDES})
        execute_process(COMMAND ${GO_EXECUTABLE} run ${PROTO_PARSER} ${PROTO_FILE} ${GENERATION_TYPE} OUTPUT_VARIABLE GENERATED_HEADERS_PART ERROR_VARIABLE PARSER_ERROR)
        set(GENERATED_HEADERS ${GENERATED_HEADERS} ${GENERATED_HEADERS_PART})
    endforeach()

    if(NOT "${PARSER_ERROR}" STREQUAL "")
        message(FATAL_ERROR "Go parser error ${PARSER_ERROR}")
    endif()

    if(DEFINED generate_qtprotobuf_GENERATED_HEADERS)
        set(GENERATED_HEADERS ${generate_qtprotobuf_GENERATED_HEADERS})
    endif()

    foreach(EXCLUDE_HEADER IN LISTS generate_qtprotobuf_EXCLUDE_HEADERS)
        list(REMOVE_ITEM GENERATED_HEADERS ${EXCLUDE_HEADER})
    endforeach()

    list(REMOVE_DUPLICATES GENERATED_HEADERS)

    if(NOT DEFINED generate_qtprotobuf_OUT_DIR)
        set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    else()
        set(OUT_DIR ${generate_qtprotobuf_OUT_DIR})
    endif()

    file(MAKE_DIRECTORY ${OUT_DIR})

    unset(QTPROTOBUF_GENERATED_SOURCES)
    unset(QTPROTOBUF_GENERATED_HEADERS)
    foreach(GENERATED_HEADER IN LISTS GENERATED_HEADERS)
        get_filename_component(GENERATED_BASENAME ${GENERATED_HEADER} NAME)
        string(REGEX REPLACE "\\.[^.]*$" "" GENERATED_BASENAME ${GENERATED_BASENAME})

        list(APPEND QTPROTOBUF_GENERATED_SOURCES ${OUT_DIR}/${GENERATED_BASENAME}.cpp)
        list(APPEND QTPROTOBUF_GENERATED_HEADERS ${OUT_DIR}/${GENERATED_BASENAME}.h)
        set_property(SOURCE ${OUT_DIR}/${GENERATED_BASENAME}.cpp PROPERTY SKIP_AUTOMOC ON)
    endforeach()

    if(WIN32)
        set(PROTOC_COMMAND set QT_PROTOBUF_OPTIONS=${GENERATION_OPTIONS}&& $<TARGET_FILE:protobuf::protoc>)
    else()
        set(PROTOC_COMMAND "QT_PROTOBUF_OPTIONS=${GENERATION_OPTIONS}" $<TARGET_FILE:protobuf::protoc>)
    endif()

    add_custom_command(
            OUTPUT ${QTPROTOBUF_GENERATED_SOURCES} ${QTPROTOBUF_GENERATED_HEADERS}
            COMMAND ${PROTOC_COMMAND}
                --plugin=protoc-gen-${GENERATOR_TARGET}=${QTPROTOBUF_EXECUTABLE}
                --${GENERATOR_TARGET}_out=${OUT_DIR}
                ${PROTO_INCLUDES}
                ${generate_qtprotobuf_PROTO_FILES}
            WORKING_DIRECTORY ${OUT_DIR}
            DEPENDS ${generate_qtprotobuf_PROTO_FILES} ${QTPROTOBUF_EXECUTABLE}
            COMMENT "Generating QtProtobuf ${generate_qtprotobuf_TARGET} sources..."
    )

    add_custom_target(${GEN_TARGET} DEPENDS ${QTPROTOBUF_GENERATED_SOURCES} ${QTPROTOBUF_GENERATED_HEADERS} ${generate_qtprotobuf_PROTO_FILES})
    if(NOT "${generate_qtprotobuf_GENERATED_HEADERS_VAR}" STREQUAL "")
        set(${generate_qtprotobuf_GENERATED_HEADERS_VAR} ${QTPROTOBUF_GENERATED_HEADERS} PARENT_SCOPE)
    endif()
    qt5_wrap_cpp(MOC_SOURCES ${QTPROTOBUF_GENERATED_HEADERS})
    list(APPEND GENERATED_SOURCES ${MOC_SOURCES})
    set_source_files_properties(${QTPROTOBUF_GENERATED_SOURCES} PROPERTIES GENERATED TRUE)
    add_library(${QtProtobuf_GENERATED} ${QTPROTOBUF_GENERATED_SOURCES} ${MOC_SOURCES})
    add_dependencies(${QtProtobuf_GENERATED} ${GEN_TARGET})
    target_include_directories(${QtProtobuf_GENERATED} PUBLIC ${OUT_DIR} PRIVATE ${Qt5Core_INCLUDE_DIRS}
        $<TARGET_PROPERTY:${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobuf,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:${QTPROTOBUF_COMMON_NAMESPACE}::QtGrpc,INTERFACE_INCLUDE_DIRECTORIES> ${OUT_DIR})
    if(NOT WIN32)
        target_include_directories(${QtProtobuf_GENERATED} PRIVATE
            $<TARGET_PROPERTY:${QTPROTOBUF_COMMON_NAMESPACE}::QtProtobufWellKnownTypes,INTERFACE_INCLUDE_DIRECTORIES>)
    endif()
endfunction()

function(qtprotobuf_link_archive TARGET GENERATED_TARGET)
    set_target_properties(${TARGET} PROPERTIES LINK_DIRECTORIES "$<TARGET_FILE_DIR:${GENERATED_TARGET}>")
    get_target_property(EXISTING_FLAGS ${TARGET} LINK_FLAGS)
    if(WIN32)
        if("${EXISTING_FLAGS}" STREQUAL "EXISTING_FLAGS-NOTFOUND")
            set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/WHOLEARCHIVE:${GENERATED_TARGET}")
        else()
            set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${EXISTING_FLAGS} /WHOLEARCHIVE:${GENERATED_TARGET}")
        endif()
    else()
        if("${EXISTING_FLAGS}" STREQUAL "EXISTING_FLAGS-NOTFOUND")
            set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-Wl,--whole-archive -l${GENERATED_TARGET} -Wl,--no-whole-archive")
        else()
            set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${EXISTING_FLAGS} -Wl,--whole-archive -l${GENERATED_TARGET} -Wl,--no-whole-archive")
        endif()
    endif()
    target_include_directories(${TARGET} PRIVATE $<TARGET_PROPERTY:${GENERATED_TARGET},INCLUDE_DIRECTORIES>)
    add_dependencies(${TARGET} ${GENERATED_TARGET})
endfunction()
