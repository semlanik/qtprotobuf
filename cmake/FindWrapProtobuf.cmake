cmake_minimum_required(VERSION 3.10)

find_package(Protobuf QUIET)

unset(WrapProtobuf_FOUND)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(FIND_LIBRARY_USE_LIB64_PATHS TRUE)
    set(FIND_LIBRARY_USE_LIBX32_PATHS FALSE)
    set(FIND_LIBRARY_USE_LIB32_PATHS FALSE)
else()
    set(FIND_LIBRARY_USE_LIBX32_PATHS TRUE)
    set(FIND_LIBRARY_USE_LIB32_PATHS TRUE)
    set(FIND_LIBRARY_USE_LIB64_PATHS FALSE)
endif()

if("Protobuf" IN_LIST WrapProtobuf_FIND_COMPONENTS OR NOT WrapProtobuf_FIND_COMPONENTS)
    set(WrapProtobuf_Protobuf_FOUND TRUE)
    if(NOT TARGET protobuf::libprotobuf)
        find_library(Protobuf_LIBRARY protobuf)
        if(Protobuf_LIBRARY)
            add_library(protobuf::libprotobuf UNKNOWN IMPORTED)
            set_target_properties(protobuf::libprotobuf PROPERTIES
                IMPORTED_LOCATION "${Protobuf_LIBRARY}"
            )
            find_path(Protobuf_INCLUDE_DIRS "message.h"
                PATH_SUFFIXES "google/protobuf"
            )
            if(Protobuf_INCLUDE_DIRS)
                set_target_properties(protobuf::libprotobuf PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${Protobuf_INCLUDE_DIRS}"
                )
            else()
                set(WrapProtobuf_Protobuf_FOUND FALSE)
            endif()
        else()
            set(WrapProtobuf_Protobuf_FOUND FALSE)
        endif()
    else()
        if(NOT Protobuf_INCLUDE_DIRS)
            get_target_property(Protobuf_INCLUDE_DIRS protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)
        endif()
    endif()
endif()

if("Protoc" IN_LIST WrapProtobuf_FIND_COMPONENTS OR NOT WrapProtobuf_FIND_COMPONENTS)
    set(WrapProtobuf_Protoc_FOUND TRUE)
    if(NOT TARGET protobuf::libprotoc)
        find_package(Threads REQUIRED)
        find_library(Protoc_LIBRARY protoc)
        if(Protoc_LIBRARY)
            add_library(protobuf::libprotoc UNKNOWN IMPORTED)
            set_target_properties(protobuf::libprotoc PROPERTIES
                IMPORTED_LOCATION "${Protoc_LIBRARY}"
                INTERFACE_LINK_LIBRARIES Threads::Threads
            )

            message(WARNING "aedelev: Protoc_INCLUDE_DIRS")
            find_path(Protoc_INCLUDE_DIRS "descriptor.h"
                PATH_SUFFIXES "google/protobuf"
            )

            if(Protoc_INCLUDE_DIRS)
                set_target_properties(protobuf::libprotoc PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${Protoc_INCLUDE_DIRS}"
                )
            else()
                set(WrapProtobuf_Protoc_FOUND FALSE)
            endif()
        else()
            set(WrapProtobuf_Protoc_FOUND FALSE)
        endif()
    else()
        if(NOT Protoc_INCLUDE_DIRS)
            get_target_property(Protoc_INCLUDE_DIRS protobuf::libprotoc INTERFACE_INCLUDE_DIRECTORIES)
        endif()
    endif()
endif()

if("Generator" IN_LIST WrapProtobuf_FIND_COMPONENTS OR NOT WrapProtobuf_FIND_COMPONENTS)
    set(WrapProtobuf_Generator_FOUND TRUE)
    if(NOT TARGET protobuf::protoc)
        find_program(Protobuf_Protoc_EXECUTABLE protoc)
        if(Protobuf_Protoc_EXECUTABLE)
            add_executable(protobuf::protoc IMPORTED)
            set_target_properties(protobuf::protoc PROPERTIES
                IMPORTED_LOCATION "${Protobuf_Protoc_EXECUTABLE}"
            )
        else()
            set(WrapProtobuf_Generator_FOUND FALSE)
        endif()
    endif()
endif()

if(WrapProtobuf_FIND_COMPONENTS)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(WrapProtobuf HANDLE_COMPONENTS
        REQUIRED_VARS Protoc_INCLUDE_DIRS Protobuf_INCLUDE_DIRS
    )
elseif(TARGET protobuf::protoc AND TARGET protobuf::libprotoc AND TARGET protobuf::libprotobuf)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(WrapProtobuf REQUIRED_VARS
        Protoc_INCLUDE_DIRS Protobuf_INCLUDE_DIRS)
endif()
