set(SUPPORT_LIBRARY_TARGET qtprotobufsupport)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

find_package(Qt5 COMPONENTS Core Qml REQUIRED)

add_library(${SUPPORT_LIBRARY_TARGET} ${SOURCES})
set_target_properties(${SUPPORT_LIBRARY_TARGET} PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(${SUPPORT_LIBRARY_TARGET} PUBLIC Qt5::Core Qt5::Qml)

install(TARGETS ${SUPPORT_LIBRARY_TARGET}
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
