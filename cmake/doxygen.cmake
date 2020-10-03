# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
    find_program(DOXYGEN_EXECUTABLE doxygen)
endif()
if(DEFINED DOXYGEN_EXECUTABLE)
    message(STATUS "Using doxygen to generate documenation: ${DOXYGEN_EXECUTABLE}")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/customdoxygen.css ${CMAKE_CURRENT_BINARY_DIR}/customdoxygen.css COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/header.html ${CMAKE_CURRENT_BINARY_DIR}/header.html COPYONLY)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/TitilliumWeb.woff2 ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html/TitilliumWeb.woff2 COPYONLY)

    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif()
