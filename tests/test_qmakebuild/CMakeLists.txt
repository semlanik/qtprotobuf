project(qmakebuild_test)
find_package(Qt5 COMPONENTS Core REQUIRED)
get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
add_custom_target(qmakebuild_test ALL
        COMMAND $<TARGET_FILE:Qt5::qmake> -o QMakefile ${CMAKE_CURRENT_SOURCE_DIR}/qmakebuild.pro CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}
        COMMAND make -f QMakefile 
)

#install(CODE execute_process(COMMAND "make -f QMakefile install"))
