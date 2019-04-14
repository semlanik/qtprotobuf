set(GRPC_SUPPORT_LIBRARY_TARGET qtgrpc)

# FIXME: replace with 'target_include_directories'
include_directories(${CMAKE_SOURCE_DIR}/src/protobuf)

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

find_package(Qt5 COMPONENTS Core Network REQUIRED)

add_library(${GRPC_SUPPORT_LIBRARY_TARGET} ${SOURCES})
set_target_properties(${GRPC_SUPPORT_LIBRARY_TARGET} PROPERTIES PUBLIC_HEADER "${HEADERS}")
if(WIN32)
    target_link_libraries(${GRPC_SUPPORT_LIBRARY_TARGET} PRIVATE qtprotobufsupport PUBLIC Qt5::Network Qt5::Core)
elseif(UNIX)
    target_link_libraries(${GRPC_SUPPORT_LIBRARY_TARGET} PRIVATE qtprotobufsupport PUBLIC Qt5::Network)
endif()
install(TARGETS ${GRPC_SUPPORT_LIBRARY_TARGET}
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
