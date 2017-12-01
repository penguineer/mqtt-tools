include_directories(
	"${PROJECT_SOURCE_DIR}/include/mqtt-tools"
    "${PROJECT_BINARY_DIR}/include/mqtt-tools"
)

# mosqhelper
add_library(mosqhelper
    mosqhelper.c
)
set_target_properties(mosqhelper PROPERTIES
    OUTPUT_NAME mosqhelper
    SOVERSION   ${PROJECT_VERSION_MAJOR}
    VERSION     ${PROJECT_VERSION}
)
target_link_libraries(mosqhelper
    "${MOSQUITTO_LIBRARY}"
)
install(TARGETS mosqhelper
	EXPORT ${PROJECT_NAME}-targets
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

# mqtta
add_library(mqtta
    mqtta.c
)
set_target_properties(mqtta PROPERTIES
    OUTPUT_NAME mqtta
    SOVERSION   ${PROJECT_VERSION_MAJOR}
    VERSION     ${PROJECT_VERSION}
)
target_link_libraries(mqtta
    mosqhelper
    "${CONFIG_LIBRARY}"
    "${MOSQUITTO_LIBRARY}"
)
install(TARGETS mqtta
	EXPORT ${PROJECT_NAME}-targets
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
