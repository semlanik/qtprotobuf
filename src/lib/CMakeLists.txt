find_package(Qt5 COMPONENTS Core REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

add_library(qtprotobufsupport universallistmodelbase.cpp universallistmodel.cpp protobufobject.cpp qtprotobuf.cpp)

set_target_properties(qtprotobufsupport PROPERTIES PUBLIC_HEADER "universallistmodelbase.h;universallistmodel.h;protobufobject.h;qtprotobuftypes.h;qtprotobuf.h")

install(TARGETS qtprotobufsupport
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
