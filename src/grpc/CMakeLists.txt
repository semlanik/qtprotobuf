find_package(Qt5 COMPONENTS Core Network REQUIRED)
if (Qt5Core_VERSION VERSION_LESS "5.8.0")
    message(FATAL_ERROR "Required Qt version is 5.8+")
endif()

set(GRPC_SUPPORT_LIBRARY_TARGET qtgrpc)

include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src/protobuf)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()


file(GLOB SOURCES asyncreply.cpp
    abstractchannel.cpp
    http2channel.cpp
    abstractclient.cpp)

file(GLOB HEADERS asyncreply.h
    abstractchannel.h
    http2channel.h
    abstractclient.h)

add_library(${GRPC_SUPPORT_LIBRARY_TARGET} ${SOURCES})
set_target_properties(${GRPC_SUPPORT_LIBRARY_TARGET} PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(${GRPC_SUPPORT_LIBRARY_TARGET} qtprotobufsupport Qt5::Core Qt5::Network)

install(TARGETS ${GRPC_SUPPORT_LIBRARY_TARGET}
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
