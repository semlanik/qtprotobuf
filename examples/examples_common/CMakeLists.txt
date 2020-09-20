set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core CONFIG REQUIRED)

file(GLOB HEADERS
    universallistmodelbase.h
    universallistmodel.h
)

file(GLOB SOURCES
    universallistmodelbase.cpp
    universallistmodel.cpp
)

# headers added to make them visible in IDE
add_library(examples_common ${SOURCES} ${HEADERS})

target_include_directories(examples_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(examples_common Qt5::Core)
