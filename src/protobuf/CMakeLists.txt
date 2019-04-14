

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

find_package(Qt5 COMPONENTS Core Qml REQUIRED)

file(GLOB SOURCES
    qprotobufobject_p.cpp
    qtprotobuf.cpp
    qtprotobuflogging.cpp)

file(GLOB HEADERS
    qprotobufobject_p.h
    qprotobufobject.h
    qtprotobuftypes.h
    qtprotobuf.h)

set(SUPPORT_LIBRARY_TARGET qtprotobufsupport)
add_library(${SUPPORT_LIBRARY_TARGET} ${SOURCES})
set_target_properties(${SUPPORT_LIBRARY_TARGET} PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(${SUPPORT_LIBRARY_TARGET} PUBLIC Qt5::Core Qt5::Qml)
# FIXME: in order to make separation between private and public headers, they should reside in different directories
target_include_directories(${SUPPORT_LIBRARY_TARGET}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

install(TARGETS ${SUPPORT_LIBRARY_TARGET}
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
