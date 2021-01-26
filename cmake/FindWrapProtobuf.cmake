cmake_minimum_required(VERSION 3.10)

find_package(Protobuf QUIET)

unset(WrapProtobuf_FOUND)
if(NOT Protobuf_FOUND OR NOT TARGET protobuf::libprotobuf
    OR NOT TARGET protobuf::protoc OR NOT TARGET protobuf::libprotoc)

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(FIND_LIBRARY_USE_LIB64_PATHS TRUE)
        set(FIND_LIBRARY_USE_LIBX32_PATHS FALSE)
        set(FIND_LIBRARY_USE_LIB32_PATHS FALSE)
    else()
        set(FIND_LIBRARY_USE_LIBX32_PATHS TRUE)
        set(FIND_LIBRARY_USE_LIB32_PATHS TRUE)
        set(FIND_LIBRARY_USE_LIB64_PATHS FALSE)
    endif()

    find_library(Protobuf_LIBRARY protobuf)
    if(NOT TARGET protobuf::libprotobuf AND Protobuf_LIBRARY)
        add_library(protobuf::libprotobuf UNKNOWN IMPORTED)
        set_target_properties(protobuf::libprotobuf PROPERTIES
            IMPORTED_LOCATION ${Protobuf_LIBRARY}
        )
        if(DEFINED Protobuf_INCLUDE_DIRS)
            set_target_properties(protobuf::libprotobuf PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES ${Protobuf_INCLUDE_DIRS})
        endif()
    endif()

    find_program(Protobuf_PROTOC_EXECUTABLE protoc)
    if(NOT TARGET protobuf::protoc AND Protobuf_PROTOC_EXECUTABLE)
        add_executable(protobuf::protoc IMPORTED)
        set_target_properties(protobuf::protoc PROPERTIES IMPORTED_LOCATION ${Protobuf_PROTOC_EXECUTABLE})
    endif()

    find_package(Threads)
    find_library(Protobuf_PROTOC_LIBRARY protoc)
    if(NOT TARGET protobuf::libprotoc AND Protobuf_PROTOC_LIBRARY)
        add_library(protobuf::libprotoc UNKNOWN IMPORTED)
        set_target_properties(protobuf::libprotoc PROPERTIES IMPORTED_LOCATION ${Protobuf_PROTOC_LIBRARY}
            INTERFACE_LINK_LIBRARIES Threads::Threads)
    endif()

    if(TARGET protobuf::libprotoc AND TARGET protobuf::protoc AND TARGET protobuf::libprotobuf)
        set(WrapProtobuf_FOUND TRUE)
    endif()
else()
    set(WrapProtobuf_FOUND TRUE)
endif()
