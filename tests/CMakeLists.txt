set(testtarget "qtprotobuf_test")

find_package(GTest REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

file(GLOB GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/*.h)
file(GLOB GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/*.cpp)

#Manual moc_ files generation for qt
list(FILTER GENERATED_SOURCES EXCLUDE REGEX "moc_.*cpp")
QT5_WRAP_CPP(MOC_SRCS ${GENERATED_HEADERS})

include_directories(${GTEST_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})


file(GLOB HEADERS ${TESTS_OUT_DIR})

add_executable(${testtarget} "main.cpp" "simpletest.cpp" ${MOC_SRCS} ${GENERATED_SOURCES})
target_link_libraries(${testtarget} ${GTEST_BOTH_LIBRARIES} Qt5::Core)
add_dependencies(${testtarget} ${testgeneration})
