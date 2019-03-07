set(testtarget "qtprotobuf_test")

if (WIN32)
    find_package(GTest)
elseif (UNIX)
    find_package(GTest REQUIRED)
endif()

find_package(Qt5 COMPONENTS Core REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

file(GLOB GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/*.h)
file(GLOB GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/*.cpp)

#Manual moc_ files generation for qt
list(FILTER GENERATED_SOURCES EXCLUDE REGEX "moc_.*cpp")
QT5_WRAP_CPP(MOC_SOURCES ${GENERATED_HEADERS})

if (WIN32)
    #Set  path to GTest build libraries
    set(GTEST_BOTH_LIBRARIES "")
    #Set  path to GTest include directory
    include_directories(${GTEST_INCLUDE_PATHS} "/")
    #Need for generated code
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
elseif (UNIX)
    include_directories(${GTEST_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src/lib)
endif()

file(GLOB HEADERS ${TESTS_OUT_DIR})
file(GLOB SOURCES main.cpp
    simpletest.cpp
    serializationtest.cpp)

file(GLOB PROTOS proto/*.proto)

add_executable(${testtarget} ${SOURCES} ${MOC_SOURCES} ${GENERATED_SOURCES} ${PROTOS})
if (WIN32)
    target_link_libraries(${testtarget} "${GTEST_BOTH_LIBRARIES}/gmock_main.lib" "${GTEST_BOTH_LIBRARIES}/gmock.lib"  Qt5::Core)
elseif (UNIX)
    target_link_libraries(${testtarget} ${GTEST_BOTH_LIBRARIES} Qt5::Core)
endif()
add_dependencies(${testtarget} ${testgeneration})
