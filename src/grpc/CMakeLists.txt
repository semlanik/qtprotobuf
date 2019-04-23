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
    abstractclient.cpp
    abstractcredentials.cpp
    sslcredentials.cpp
    insecurecredentials.cpp)

file(GLOB HEADERS asyncreply.h
    abstractchannel.h
    http2channel.h
    abstractclient.h
    abstractcredentials.h
    sslcredentials.h
    insecurecredentials.h)

set(GRPC_SUPPORT_LIBRARY_TARGET qtgrpc)
add_library(${GRPC_SUPPORT_LIBRARY_TARGET} ${SOURCES})
set_target_properties(${GRPC_SUPPORT_LIBRARY_TARGET} PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(${GRPC_SUPPORT_LIBRARY_TARGET} PRIVATE qtprotobufsupport PUBLIC Qt5::Network Qt5::Core)
target_include_directories(${GRPC_SUPPORT_LIBRARY_TARGET}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

install(TARGETS ${GRPC_SUPPORT_LIBRARY_TARGET}
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
