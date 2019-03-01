cmake_minimum_required(VERSION 2.8)

project(qtprotobuf)

find_package(Protobuf)
message("Protobuf libraries"${Protobuf_LIBRARIES})
add_executable(${PROJECT_NAME} "src/generator/main.cpp" "src/generator/generator.cpp")
target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES} "-lprotoc")
