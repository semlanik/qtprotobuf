set(CPACK_PACKAGE_CONTACT "Alexey Edelev <semlanik@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION "gRPC and Protobuf generator and bindings for Qt framework")
set(CPACK_PACKAGE_NAME "qtprotobuf")

include("${QT_PROTOBUF_CMAKE_DIR}/deb.cmake")
include("${QT_PROTOBUF_CMAKE_DIR}/rpm.cmake")
include(CPack)
