cmake_minimum_required(VERSION 2.8)

project(qtprotobuf)

find_package(Protobuf)

add_executable(${PROJECT_NAME} "src/generator/main.cpp" "src/generator/generator.cpp" "src/generator/enumsgenerator.cpp")
target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES} "-lprotoc")

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin)
