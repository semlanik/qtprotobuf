set(TARGET ${PROTOBUF_LIBRARY_TARGET})
set(SHARED_TARGET ${TARGET}-shared)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Qml REQUIRED)

file(GLOB SOURCES
    qprotobufobject_p.cpp
    qtprotobuf.cpp
    qtprotobuflogging.cpp)

file(GLOB HEADERS
    qprotobufobject_p.h
    qprotobufobject.h
    qtprotobuftypes.h
    qtprotobuf.h
    selfcheckiterator.h
    qtprotobuflogging.h)

set(INCLUDE_DIRS INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC ${Qt5Core_INCLUDE_DIRS} ${Qt5Qml_INCLUDE_DIRS})

add_library(${TARGET} ${SOURCES})
target_include_directories(${TARGET} ${INCLUDE_DIRS})

add_library(${SHARED_TARGET} SHARED)
set_target_properties(${SHARED_TARGET} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "${HEADERS}" OUTPUT_NAME ${TARGET})
target_include_directories(${SHARED_TARGET} ${INCLUDE_DIRS})
target_link_libraries(${SHARED_TARGET} PRIVATE $<TARGET_FILE:${TARGET}> PUBLIC Qt5::Core Qt5::Qml)
add_dependencies(${SHARED_TARGET} ${PROTOBUF_LIBRARY_TARGET})

install(TARGETS ${TARGET} ${SHARED_TARGET}
    ARCHIVE DESTINATION ${TARGET_LIB_DIR}
    PUBLIC_HEADER DESTINATION ${TARGET_INCLUDE_DIR}
    LIBRARY DESTINATION ${TARGET_LIB_DIR})
