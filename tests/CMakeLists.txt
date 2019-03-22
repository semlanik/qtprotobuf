find_package(GTest)
if (UNIX AND NOT GTest_FOUND)
    # FIXME: if protobuf was downloaded then the following include is not more needed
    include(../cmake/DownloadProject.cmake)
    download_project(PROJ                GTest
        GIT_REPOSITORY      https://github.com/google/googletest.git
        GIT_TAG             v1.8.x
        )
    # uncomment if gmock is going to be needed
    set(BUILD_GMOCK OFF CACHE BOOL "Disable gmock build functionality" FORCE)
    add_subdirectory(${GTest_SOURCE_DIR} ${GTest_BINARY_DIR})
endif()

find_package(Qt5 COMPONENTS Core REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC ON)

if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

file(GLOB GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/*.h)
file(GLOB GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/*.cpp)

#Manual moc_ files generation for qt
list(FILTER GENERATED_SOURCES EXCLUDE REGEX "moc_.*cpp")
QT5_WRAP_CPP(MOC_SOURCES ${GENERATED_HEADERS})

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src/lib)

if(WIN32)
    #Set  path to GTest build libraries
    set(GTEST_BOTH_LIBRARIES "")
    #Set  path to GTest include directory
    include_directories(${GTEST_INCLUDE_PATHS} "/")
elseif(UNIX)
    include_directories(${GTEST_INCLUDE_DIRS})
endif()

file(GLOB HEADERS ${TESTS_OUT_DIR}/*.h)
file(GLOB SOURCES main.cpp
    simpletest.cpp
    serializationtest.cpp
    deserializationtest.cpp
    servertest.cpp
    clienttest.cpp)

file(GLOB PROTOS proto/*.proto)

set(testtarget "qtprotobuf_test")
add_executable(${testtarget} ${SOURCES} ${MOC_SOURCES} ${GENERATED_SOURCES} ${PROTOS})
if(WIN32)
    target_link_libraries(${testtarget} "${GTEST_BOTH_LIBRARIES}/gmock_main.lib" "${GTEST_BOTH_LIBRARIES}/gmock.lib" qtprotobufsupport Qt5::Core)
elseif(UNIX)
    target_link_libraries(${testtarget} gtest qtprotobufsupport Qt5::Core)
endif()
add_dependencies(${testtarget} ${testgeneration})
