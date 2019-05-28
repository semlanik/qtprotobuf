add_subdirectory("examples_common")

add_subdirectory("addressbook")
add_subdirectory("addressbookserver")
message(STATUS "CLIENT: ${CLIENT_EXEC_PATH}")
message(STATUS "SERVER: ${SERVER_EXEC_PATH}")
configure_file(client_server_driver.sh.in addressbook_driver.sh @ONLY)

add_subdirectory("simplechat")
add_subdirectory("simplechatserver")
message(STATUS "CLIENT: ${CLIENT_EXEC_PATH}")
message(STATUS "SERVER: ${SERVER_EXEC_PATH}")
configure_file(client_server_driver.sh.in simplechat_driver.sh @ONLY)
