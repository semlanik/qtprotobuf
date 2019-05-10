set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(GLOB HEADERS
    universallistmodelbase.h
    universallistmodel.h
)

file(GLOB SOURCES
    universallistmodelbase.cpp
    universallistmodel.cpp
)

# headers added to make them visible in IDE
add_library(qttools ${SOURCES} ${HEADERS})

target_include_directories(qttools PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(qttools Qt5::Core)
