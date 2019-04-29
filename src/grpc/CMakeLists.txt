set(TARGET ${GRPC_LIBRARY_TARGET})
set(SHARED_TARGET ${TARGET}-shared)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

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

set(INCLUDE_DIRS INTERFACE ${CURRENT_SOURCE_DIR} PUBLIC ${CMAKE_SOURCE_DIR}/src/protobuf ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS})

add_library(${TARGET} ${SOURCES})
target_include_directories(${TARGET} ${INCLUDE_DIRS})

add_library(${SHARED_TARGET} SHARED)
set_target_properties(${SHARED_TARGET} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "${HEADERS}" OUTPUT_NAME ${TARGET})
target_include_directories(${SHARED_TARGET} ${INCLUDE_DIRS})
target_link_libraries(${SHARED_TARGET} PRIVATE $<TARGET_FILE:${TARGET}> PUBLIC ${PROTOBUF_LIBRARY_TARGET} Qt5::Network Qt5::Core)
add_dependencies(${SHARED_TARGET} ${PROTOBUF_LIBRARY_TARGET})

install(TARGETS ${TARGET} ${SHARED_TARGET}
    ARCHIVE DESTINATION ${TARGET_LIB_DIR}
    PUBLIC_HEADER DESTINATION ${TARGET_INCLUDE_DIR}
    LIBRARY DESTINATION ${TARGET_LIB_DIR})
