cmake_minimum_required(VERSION 3.16)

if(NOT PROTO_FILE OR NOT EXISTS "${PROTO_FILE}")
    message(FATAL_ERROR "Unable to scan '${PROTO_FILE}': file doesn't exist.")
endif()

file(READ "${PROTO_FILE}" file_content)
if(NOT file_content)
    message(FATAL_ERROR "Unable to read ${PROTO_FILE}.")
endif()

STRING(REPLACE ";" "\\\\;" file_content "${file_content}")
STRING(REPLACE "\n" ";" file_content "${file_content}")

set(proto_key_common_regex "[\t ]+([a-zA-Z0-9_]+)")

set(unclosed_braces 0)
set(in_message_scope FALSE)
foreach(item IN LISTS file_content)
    if(item MATCHES "^[\t ]*package[\t ]+([a-zA-Z0-9_\.\-]+)")
        set(proto_package "${CMAKE_MATCH_1}")
    elseif(item MATCHES "^[\t ]*message${proto_key_common_regex}")
        # Skip adding nested messages to the list.
        # Multi-file generator puts the nested messages to the same file as the parent one. So
        # generated
        if(unclosed_braces EQUAL 0)
            list(APPEND proto_messages "${CMAKE_MATCH_1}")
            set(in_message_scope TRUE)
        endif()
    elseif(item MATCHES "^[\t ]*service${proto_key_common_regex}")
        list(APPEND proto_services "${CMAKE_MATCH_1}")
    elseif(item MATCHES "^[\t ]*enum${proto_key_common_regex}")
        list(APPEND proto_enums "${CMAKE_MATCH_1}")
    endif()
    if(in_message_scope)
        if(item MATCHES "[^/]*\\{")
            math(EXPR unclosed_braces "${unclosed_braces} + 1")
        endif()
        if(item MATCHES "[^/]*\\}")
            math(EXPR unclosed_braces "${unclosed_braces} - 1")
            if(unclosed_braces EQUAL 0)
                set(in_message_scope FALSE)
            endif()
        endif()
    endif()
endforeach()

unset(output_files)
string(REPLACE "." "/" package_full_path "${proto_package}")
if(MULTI)
    foreach(item IN LISTS proto_messages)
        string(TOLOWER "${item}" item)
        list(APPEND output_files
            "${package_full_path}/${item}.h"
            "${package_full_path}/${item}.cpp"
        )
    endforeach()
    foreach(item IN LISTS proto_services)
        string(TOLOWER "${item}" item)
        list(APPEND output_files
            "${package_full_path}/${item}client.h"
            "${package_full_path}/${item}client.cpp"
        # TODO: Generating of a server side code is not supported yet.
        #     "${package_full_path}/${item}server.h"
        #     "${package_full_path}/${item}server.cpp"
        )
    endforeach()
else()
    get_filename_component(basename "${PROTO_FILE}" NAME_WLE)
    list(LENGTH proto_messages message_count)
    list(LENGTH proto_enums enum_count)
    list(LENGTH proto_services service_count)

    set(folder_path "")
    if(FOLDER)
        set(folder_path "${package_full_path}/")
    endif()
    if(proto_messages OR proto_enums)
        list(APPEND output_files
            "${folder_path}${basename}.qpb.h"
            "${folder_path}${basename}.qpb.cpp"
        )
    endif()
    if(proto_services)
        list(APPEND output_files
            "${folder_path}${basename}_grpc.qpb.h"
            "${folder_path}${basename}_grpc.qpb.cpp"
        )
    endif()
endif()

message("${output_files}")
