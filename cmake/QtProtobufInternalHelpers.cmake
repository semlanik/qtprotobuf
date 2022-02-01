set(_qtprotobuf_all_known_multi_args SOURCES PUBLIC_HEADER INCLUDE_DIRECTORIES LIBRARIES
    PUBLIC_LIBRARIES PUBLIC_INCLUDE_DIRECTORIES DEFINES PUBLIC_DEFINES)

function(qtprotobuf_parse_arguments prefix option single multi)
    cmake_parse_arguments(arg "${option}" "${single}" "${_qtprotobuf_all_known_multi_args}" ${ARGN})
    foreach(_arg IN ITEMS ${_qtprotobuf_all_known_multi_args};${single};${option})
        if(${_arg} IN_LIST multi OR ${_arg} IN_LIST single OR ${_arg} IN_LIST option)
            set(${prefix}_${_arg} ${arg_${_arg}} PARENT_SCOPE)
        else()
            list(APPEND ${prefix}_UNPARSED_ARGUMENTS ${_arg} ${arg_${_arg}})
        endif()
    endforeach()
    set(${prefix}_UNPARSED_ARGUMENTS ${${prefix}_UNPARSED_ARGUMENTS} PARENT_SCOPE)
endfunction()

function(qt_protobuf_internal_add_test)
    set(options MULTI QML FIELDENUM)
    set(oneValueArgs QML_DIR TARGET EXTRA_NAMESPACE)
    set(multiValueArgs SOURCES EXCLUDE_HEADERS PROTO_FILES PROTO_INCLUDES)
    cmake_parse_arguments(add_test_target "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    ## test sources build
    # policy enables automoc for generated files
    if(${CMAKE_VERSION} VERSION_GREATER "3.10.0")
        cmake_policy(SET CMP0071 NEW)
    endif()

    set(GENERATED_SOURCES_DIR "${CMAKE_CURRENT_BINARY_DIR}/${add_test_target_TARGET}_generated")

    if(DEFINED add_test_target_PROTO_FILES)
        file(GLOB proto_files ABSOLUTE ${add_test_target_PROTO_FILES})
    else()
        file(GLOB proto_files ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto")
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
    if(add_test_target_EXTRA_NAMESPACE)
        set(EXTRA_OPTIONS ${EXTRA_OPTIONS} EXTRA_NAMESPACE ${add_test_target_EXTRA_NAMESPACE})
    endif()

    qtprotobuf_generate(TARGET ${add_test_target_TARGET}
        OUTPUT_DIRECTORY ${GENERATED_SOURCES_DIR}
        PROTO_FILES ${proto_files}
        EXCLUDE_HEADERS ${add_test_target_EXCLUDE_HEADERS}
        ${EXTRA_OPTIONS}
        PROTO_INCLUDES ${add_test_target_PROTO_INCLUDES})

    target_link_libraries(${add_test_target_TARGET} PUBLIC gtest_main
                                                           gtest
                                                           ${QT_PROTOBUF_NAMESPACE}::Protobuf
                                                           ${QT_PROTOBUF_NAMESPACE}::Grpc
                                                           ${QT_VERSIONED_PREFIX}::Core
                                                           ${QT_VERSIONED_PREFIX}::Test
                                                           ${QT_VERSIONED_PREFIX}::Network
                                                           ${CMAKE_THREAD_LIBS_INIT})
    if(${add_test_target_QML})
        target_link_libraries(${add_test_target_TARGET} PUBLIC ${QT_VERSIONED_PREFIX}::Qml)
    endif()
endfunction()

function(qt_protobuf_internal_add_example)
    set(options)
    set(oneValueArgs TARGET QML_DIR)
    set(multiValueArgs SOURCES PROTO_FILES RESOURCES)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    file(GLOB PROTO_FILES ABSOLUTE ${arg_PROTO_FILES})
    file(GLOB SOURCES ${arg_SOURCES})

    add_executable(${arg_TARGET} ${SOURCES} ${arg_RESOURCES})
    qtprotobuf_generate(TARGET ${arg_TARGET}
        OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated
        PROTO_FILES ${PROTO_FILES}
        QML)

    qt_protobuf_internal_add_target_windeployqt(TARGET ${arg_TARGET} QML_DIR ${arg_QML_DIR})

    target_link_libraries(${arg_TARGET} PRIVATE examples_common
                                            ${QT_PROTOBUF_NAMESPACE}::Protobuf
                                            ${QT_PROTOBUF_NAMESPACE}::Grpc
                                            ${QT_VERSIONED_PREFIX}::Quick
                                            ${QT_VERSIONED_PREFIX}::Qml)
endfunction()

function(qt_protobuf_internal_check_install)
    set(QT_PROTOBUF_INSTALL
        TRUE CACHE INTERNAL "Indicates wether QT_PROTOBUF_INSTALL might be installed or not" FORCE
    )

    if(NOT "${QT_PROTOBUF_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
        message(STATUS "QtProtobuf configured as sub-project.")
        set(QT_PROTOBUF_INSTALL FALSE CACHE INTERNAL "" FORCE)
        message(STATUS "All install rules are disabled.")
    endif()
endfunction()

function(qt_protobuf_extract_qt_variable variable)
    if(NOT DEFINED QT_QMAKE_EXECUTABLE)
        find_program(QT_QMAKE_EXECUTABLE "qmake")
        if(QT_QMAKE_EXECUTABLE STREQUAL QT_QMAKE_EXECUTABLE-NOTFOUND)
            message(FATAL_ERROR "Could not find qmake executable")
        endif()
    endif()
    execute_process(
        COMMAND "${QT_QMAKE_EXECUTABLE}" -query ${variable}
        OUTPUT_VARIABLE ${variable}
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(${variable} ${${variable}} PARENT_SCOPE)
endfunction()

function(qt_protobuf_internal_generate_qt_headers out_var)
    set(options)
    set(oneValueArgs "TARGET")
    set(multiValueArgs "PUBLIC_HEADER")
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(PUBLIC_HEADER IN LISTS arg_PUBLIC_HEADER)
        get_filename_component(PUBLIC_HEADER_BASE_NAME ${PUBLIC_HEADER} NAME FALSE)
        file(STRINGS ${PUBLIC_HEADER} CLASS_NAME REGEX "#pragma once //[a-zA-Z]+")
        if (NOT "${CLASS_NAME}" STREQUAL "")
            string(REPLACE "#pragma once //" "" CLASS_NAME "${CLASS_NAME}")
            message(STATUS "Generate Qt header for ${CLASS_NAME}")
            configure_file("${QT_PROTOBUF_CMAKE_DIR}/GeneratedHeaderTemplate"
                "${QT_PROTOBUF_BINARY_DIR}/include/${arg_TARGET}/${CLASS_NAME}" @ONLY
            )

            list(APPEND ${out_var}
                "${QT_PROTOBUF_BINARY_DIR}/include/${arg_TARGET}/${CLASS_NAME}"
            )
        endif()
    endforeach()
    set(${out_var} "${${out_var}}" PARENT_SCOPE)
endfunction()

function(qt_protobuf_internal_add_library target)
    qtprotobuf_parse_arguments(arg
        ""
        "INSTALL_LIBDIR;INSTALL_INCLUDEDIR"
        "SOURCES;PUBLIC_HEADER"
        ${ARGN}
    )
    set(target_export "${QT_PROTOBUF_NAMESPACE}${target}Targets")
    set(target_config "${QT_PROTOBUF_NAMESPACE}${target}Config")

    if(arg_INSTALL_INCLUDEDIR)
        set(target_install_includedir "${arg_INSTALL_INCLUDEDIR}")
    else()
        set(target_install_includedir
            "${CMAKE_INSTALL_INCLUDEDIR}/${QT_PROTOBUF_NAMESPACE}${target}"
        )
    endif()
    set(target_install_libdir "${CMAKE_INSTALL_LIBDIR}")
    set(target_install_cmakedir "${CMAKE_INSTALL_LIBDIR}/cmake/${QT_PROTOBUF_NAMESPACE}")

    if(NOT arg_SOURCES)
        message(FATAL_ERROR "SOURCES are not specified for ${target}")
    endif()

    file(GLOB sources ${arg_SOURCES})
    add_library(${target} ${sources})

    if(NOT BUILD_SHARED_LIBS)
        target_compile_definitions(${target} PUBLIC QT_PROTOBUF_STATIC)
    endif()

    string(TOUPPER "${target}" target_upper)
    target_compile_definitions(${target} PRIVATE QT_BUILD_${target_upper}_LIB)

    set_target_properties(${target}
        PROPERTIES
            VERSION ${PROJECT_VERSION}
            OUTPUT_NAME ${QT_VERSIONED_PREFIX}${target}
    )

    target_compile_features(${target}
        PUBLIC cxx_std_14
            cxx_auto_type
            cxx_decltype
            cxx_final
            cxx_override
            cxx_nullptr
            cxx_lambdas
            cxx_func_identifier
    )

    target_compile_definitions(${target}
        PUBLIC
            QT_PROTOBUF_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
            QT_PROTOBUF_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    )

    target_include_directories(${target}
        PUBLIC
           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
           "$<BUILD_INTERFACE:${QT_PROTOBUF_BINARY_DIR}/include/${target}>"
           "$<INSTALL_INTERFACE:${target_install_includedir}>"
    )

    qt_protobuf_internal_extend_target(${target} ${arg_UNPARSED_ARGUMENTS}
        PUBLIC_HEADER ${arg_PUBLIC_HEADER})

    add_library(${QT_PROTOBUF_NAMESPACE}::${target} ALIAS ${target})

    set(target_config_bin_dir "${QT_PROTOBUF_BINARY_DIR}/")
    include(CMakePackageConfigHelpers)
    configure_package_config_file("Qt${target}Config.cmake.in"
        "${QT_PROTOBUF_BINARY_DIR}/${target_config}.cmake"
        INSTALL_DESTINATION "${target_install_cmakedir}"
    )

    export(TARGETS ${target}
        NAMESPACE ${QT_PROTOBUF_NAMESPACE}::
        FILE ${target_export}.cmake
    )

    if(QT_PROTOBUF_INSTALL)
        if(NOT arg_INSTALL_LIBDIR)
            unset(arg_INSTALL_LIBDIR)
        endif()

        if(NOT arg_INSTALL_INCLUDEDIR)
            unset(arg_INSTALL_INCLUDEDIR)
        endif()

        set_target_properties(${target} PROPERTIES
            qt_protobuf_install_libdir "${arg_INSTALL_LIBDIR}"
            qt_protobuf_install_includedir "${arg_INSTALL_INCLUDEDIR}"
        )
        set_property(GLOBAL APPEND PROPERTY qt_protobuf_install_targets ${target})
    endif()

    qt_protobuf_internal_add_coverage_target(TARGET ${target})
endfunction()

function(qt_protobuf_internal_extend_target target)
    set(multi_arg
        SOURCES
        PUBLIC_HEADER
        INCLUDE_DIRECTORIES
        LIBRARIES
        PUBLIC_LIBRARIES
        PUBLIC_INCLUDE_DIRECTORIES
        DEFINES
        PUBLIC_DEFINES
    )
    qtprotobuf_parse_arguments(arg
        ""
        ""
        "${multi_arg}"
        ${ARGN}
    )

    if(NOT TARGET ${target})
        message(FATAL_ERROR "${target} is not a target")
    endif()

    if(arg_SOURCES)
        file(GLOB sources ${arg_SOURCES})
        target_sources(${target} PRIVATE ${sources})
    endif()

    if(arg_PUBLIC_HEADER)
        file(GLOB public_header_paths ${arg_PUBLIC_HEADER})
        qt_protobuf_internal_generate_qt_headers(generated_public_header
            PUBLIC_HEADER ${public_header_paths}
            TARGET ${target}
        )
        set_property(TARGET ${target} APPEND PROPERTY
            PUBLIC_HEADER "${public_header_paths};${generated_public_header}")
        target_sources(${target} PRIVATE ${public_header_paths})
    endif()

    if(arg_LIBRARIES)
        target_link_libraries(${target} PRIVATE ${arg_LIBRARIES})
    endif()

    if(arg_PUBLIC_LIBRARIES)
        target_link_libraries(${target} PUBLIC ${arg_PUBLIC_LIBRARIES})
    endif()

    if(arg_INCLUDE_DIRECTORIES)
        target_include_directories(${target} PRIVATE ${arg_INCLUDE_DIRECTORIES})
    endif()

    if(arg_PUBLIC_INCLUDE_DIRECTORIES)
        target_include_directories(${target} PUBLIC ${arg_PUBLIC_INCLUDE_DIRECTORIES})
    endif()

    if(arg_DEFINES)
        target_compile_definitions(${target} PRIVATE ${arg_DEFINES})
    endif()

    if(arg_PUBLIC_DEFINES)
       target_compile_definitions(${target} PUBLIC ${arg_PUBLIC_DEFINES})
    endif()
endfunction()

function(qt_protobuf_internal_add_executable target)
    qtprotobuf_parse_arguments(arg
        ""
        "INSTALL_BINDIR"
        "SOURCES"
        ${ARGN}
    )
    set(target_export "${QT_PROTOBUF_NAMESPACE}${target}Targets")
    set(target_config "${QT_PROTOBUF_NAMESPACE}${target}Config")

    set(target_install_cmakedir "${CMAKE_INSTALL_LIBDIR}/cmake/${QT_PROTOBUF_NAMESPACE}")

    file(GLOB sources ${arg_SOURCES})
    add_executable(${target} ${sources})

    qt_protobuf_internal_extend_target(${target} ${arg_UNPARSED_ARGUMENTS})

    add_executable(${QT_PROTOBUF_NAMESPACE}::${target} ALIAS ${target})

    include(CMakePackageConfigHelpers)
    set(target_config_bin_dir "${QT_PROTOBUF_BINARY_DIR}/")
    include(CMakePackageConfigHelpers)
    configure_package_config_file("Qt${target}Config.cmake.in"
        "${QT_PROTOBUF_BINARY_DIR}/${target_config}.cmake"
        INSTALL_DESTINATION "${target_install_cmakedir}"
    )

    export(TARGETS ${target} NAMESPACE ${QT_PROTOBUF_NAMESPACE}:: FILE ${target_export}.cmake)

    if(QT_PROTOBUF_INSTALL)
        if(arg_INSTALL_BINDIR)
            set_target_properties(${target} PROPERTIES
                qt_protobuf_install_bindir "${arg_INSTALL_BINDIR}"
            )
        endif()
        set_property(GLOBAL APPEND PROPERTY qt_protobuf_install_targets ${target})
    endif()
endfunction()

function(qt_protobuf_internal_install_targets)
    if(NOT QT_PROTOBUF_INSTALL)
        return()
    endif()
    get_property(install_targets GLOBAL PROPERTY qt_protobuf_install_targets)

    list(REMOVE_DUPLICATES install_targets)
    foreach(target ${install_targets})
        set(target_config "${QT_PROTOBUF_NAMESPACE}${target}Config")
        set(target_export "${QT_PROTOBUF_NAMESPACE}${target}Targets")
        set(target_install_cmakedir "${CMAKE_INSTALL_LIBDIR}/cmake/${QT_PROTOBUF_NAMESPACE}")

        get_target_property(type ${target} TYPE)
        if("${type}" STREQUAL "EXECUTABLE")
            get_target_property(target_install_bindir ${target} qt_protobuf_install_bindir)
            if(NOT target_install_bindir)
                set(target_install_bindir "${CMAKE_INSTALL_BINDIR}")
            endif()

            install(TARGETS ${target}
                EXPORT ${target_export} COMPONENT dev
                RUNTIME DESTINATION "${target_install_bindir}" COMPONENT lib
            )
        else()
            set(target_install_bindir "${CMAKE_INSTALL_BINDIR}")
            get_target_property(target_install_includedir ${target} qt_protobuf_install_includedir)
            if(NOT target_install_includedir)
                set(target_install_includedir
                    "${CMAKE_INSTALL_INCLUDEDIR}/${QT_PROTOBUF_NAMESPACE}${target}"
                )
            endif()

            get_target_property(target_install_libdir ${target} qt_protobuf_install_libdir)
            if(NOT target_install_libdir)
                set(target_install_libdir "${CMAKE_INSTALL_LIBDIR}")
            endif()

            install(TARGETS ${target}
                EXPORT ${target_export} COMPONENT dev
                ARCHIVE DESTINATION "${target_install_libdir}" COMPONENT lib
                PUBLIC_HEADER DESTINATION "${target_install_includedir}" COMPONENT dev
                LIBRARY DESTINATION "${target_install_libdir}" COMPONENT lib
                RUNTIME DESTINATION "${target_install_bindir}" COMPONENT lib
            )
        endif()

        install(EXPORT ${target_export}
            NAMESPACE ${QT_PROTOBUF_NAMESPACE}::
            FILE ${target_export}.cmake
            DESTINATION "${target_install_cmakedir}"
            COMPONENT dev
        )
        install(FILES "${QT_PROTOBUF_BINARY_DIR}/${target_config}.cmake"
            DESTINATION "${target_install_cmakedir}"
            COMPONENT dev
        )
    endforeach()
endfunction()

function(qt_protobuf_internal_generate_pri target)
    string(TOLOWER "${target}" target_lower)
    set(pri_name "qt_lib_${target_lower}.pri")
    get_target_property(QT_PROTOBUF_LIBRARY_OUTPUT_NAME ${target} OUTPUT_NAME)

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${pri_name}.in"
        "${QT_PROTOBUF_BINARY_DIR}/${pri_name}" @ONLY
    )

    if(QT_PROTOBUF_INSTALL)
        qt_protobuf_extract_qt_variable(QT_INSTALL_ARCHDATA)
        qt_protobuf_extract_qt_variable(QT_INSTALL_PREFIX)

        if("${QT_INSTALL_PREFIX}" STREQUAL "${CMAKE_INSTALL_PREFIX}")
            set(mkspec_install_dir "${QT_INSTALL_ARCHDATA}/mkspecs/modules")
        else()
            set(mkspec_install_dir "${CMAKE_INSTALL_PREFIX}/mkspecs/modules")
        endif()

        install(FILES "${QT_PROTOBUF_BINARY_DIR}/${pri_name}"
            DESTINATION "${mkspec_install_dir}"
            COMPONENT dev
        )
    endif()
endfunction()
