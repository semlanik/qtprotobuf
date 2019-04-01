find_package(Qt5 COMPONENTS Core REQUIRED)
if (Qt5Core_VERSION VERSION_LESS "5.11.0")
    message(FATAL_ERROR "Required Qt version is 5.11+")
endif()

set(SUPPORT_LIBRARY_TARGET qtprotobufsupport)

include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()


file(GLOB SOURCES universallistmodelbase.cpp
    universallistmodel.cpp
    protobufobject.cpp
    qtprotobuf.cpp
    qtprotobuflogging.cpp)

file(GLOB HEADERS universallistmodelbase.h
    universallistmodel.h
    protobufobject.h
    qtprotobuftypes.h
    qtprotobuf.h)

add_library(${SUPPORT_LIBRARY_TARGET} ${SOURCES})
set_target_properties(${SUPPORT_LIBRARY_TARGET} PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(${SUPPORT_LIBRARY_TARGET} PUBLIC Qt5::Core)

install(TARGETS ${SUPPORT_LIBRARY_TARGET}
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
