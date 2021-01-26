if(WrapgRPC_FOUND)
    include(QtProtobufTestHelpers)
    add_subdirectory("examples_common")

    add_subdirectory("addressbook")
    add_subdirectory("addressbookserver")
    configure_file(client_server_driver.sh.in addressbook_driver.sh @ONLY)

    add_subdirectory("simplechat")
    add_subdirectory("simplechatserver")
    configure_file(client_server_driver.sh.in simplechat_driver.sh @ONLY)

    add_subdirectory("clienttutorial")
else()
    message(WARNING "gRPC not found: disable examples")
endif()
