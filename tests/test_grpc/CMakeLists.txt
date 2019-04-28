set(TARGET qtgrpc_test)

set(GENERATED_HEADERS
    simplestringmessage.h
    testserviceclient.h
#    testserviceserver.h
)

file(GLOB SOURCES main.cpp
#    servertest.cpp
    clienttest.cpp)
add_test_target(TARGET ${TARGET}
    GENERATED_HEADERS ${GENERATED_HEADERS}
    SOURCES ${SOURCES})

add_subdirectory(echoserver)
