cmake_minimum_required(VERSION 2.8)

project(qtprotobuf)

find_package(Protobuf)

add_subdirectory("src/lib")

add_executable(${PROJECT_NAME} "src/generator/main.cpp" "src/generator/generator.cpp"
    "src/generator/classgeneratorbase.cpp")

if (WIN32)
    #Needs to set path to protobuf libraries
    set(PROTOBUF_INSTALATION_PATH "")
    include_directories(${PROTOBUF_INSTALATION_PATH}/include)

    set(PROTOBUF_LIBRARIES_PATH ${PROTOBUF_INSTALATION_PATH}/lib)
    target_link_libraries(${PROJECT_NAME} "${PROTOBUF_LIBRARIES_PATH}/libprotobuf.lib")
    target_link_libraries(${PROJECT_NAME} "${PROTOBUF_LIBRARIES_PATH}/libprotoc.lib")

elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES} "-lprotoc")
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin)
