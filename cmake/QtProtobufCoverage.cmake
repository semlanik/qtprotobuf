if(QT_PROTOBUF_MAKE_COVERAGE AND UNIX)
    message(STATUS "Enable gcov")
    add_custom_target(coverage)
    add_custom_target(_qt_protobuf_internal_coverage_tests)
    add_custom_command(TARGET _qt_protobuf_internal_coverage_tests
        COMMAND ${CMAKE_CTEST_COMMAND}
        WORKING_DIRECTORY ${QT_PROTOBUF_BINARY_DIR}
    )
endif()

function(qt_protobuf_internal_add_coverage_target)
    if(NOT QT_PROTOBUF_MAKE_COVERAGE OR NOT UNIX)
        return()
    endif()

    set(options)
    set(oneValueArgs "TARGET")
    set(multiValueArgs)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(GCOV_OUTPUT_DIR ${QT_PROTOBUF_BINARY_DIR}/gcov/${arg_TARGET})
    set(OBJECTS_DIR ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${arg_TARGET}.dir)
    set(COVERAGE_TARGET ${arg_TARGET}_coverage)

    if(NOT TARGET ${arg_TARGET})
        message(FATAL_ERROR "Unable to add a coverage target, '${arg_TARGET}' is not a traget")
    endif()

    find_program(GCOV_EXECUTABLE "gcov")
    if(NOT GCOV_EXECUTABLE)
        message(FATAL_ERROR "Coverage is enabled for target ${arg_TARGET}, \
but gcov is not installed")
    endif()

    target_compile_options(${arg_TARGET} PUBLIC "-fprofile-arcs" "-ftest-coverage")
    target_link_options(${arg_TARGET} PUBLIC "--coverage")

    add_dependencies(_qt_protobuf_internal_coverage_tests ${arg_TARGET})
    add_custom_target(${COVERAGE_TARGET} DEPENDS _qt_protobuf_internal_coverage_tests)

    file(MAKE_DIRECTORY "${GCOV_OUTPUT_DIR}")
    add_custom_command(TARGET ${COVERAGE_TARGET}
        COMMAND "${GCOV_EXECUTABLE}" "${OBJECTS_DIR}/*.o" -b -c
        WORKING_DIRECTORY ${GCOV_OUTPUT_DIR}
    )

    find_program(GCOVR_EXECUTABLE "gcovr")
    if(GCOVR_EXECUTABLE)
        set(GCOVR_OUTPUT_DIR "${GCOV_OUTPUT_DIR}/html")
        file(MAKE_DIRECTORY "${GCOVR_OUTPUT_DIR}")
        add_custom_command(TARGET ${COVERAGE_TARGET}
            COMMAND "${GCOVR_EXECUTABLE}" -g -k -r "${CMAKE_SOURCE_DIR}" --html --html-details -o
                    "${GCOVR_OUTPUT_DIR}/${arg_TARGET}.html" "${OBJECTS_DIR}" .
            WORKING_DIRECTORY ${GCOV_OUTPUT_DIR}
        )
    endif()
    add_dependencies(coverage ${COVERAGE_TARGET})
endfunction()
