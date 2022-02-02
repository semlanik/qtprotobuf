cmake_minimum_required(VERSION 3.16)

include(.cmake.conf)
project(QtProtobuf
    VERSION "${QT_REPO_MODULE_VERSION}"
    DESCRIPTION "Qt gRPC and Protobuf generator and bindings."
    HOMEPAGE_URL "https://github.com/semlanik/qtprotobuf"
    LANGUAGES CXX C
)

# Make sure we use the fixed BASE argument of qt_add_resource.
set(QT_USE_FIXED_QT_ADD_RESOURCE_BASE TRUE)

find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS BuildInternals Core Network)
find_package(Qt6 ${PROJECT_VERSION} CONFIG OPTIONAL_COMPONENTS Quick Qml Test QuickTest)

include("${CMAKE_CURRENT_LIST_DIR}/src/protobuf/cmake/Qt6ProtobufMacro.cmake")

qt_build_repo()
