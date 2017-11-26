cmake_minimum_required(VERSION 3.0)

project(mqtt-tools
    VERSION 0.1.0
    LANGUAGES C)

#
# NOTE  This project uses semantic versioning as described at
#
#       https://semver.org/
#
#       As long as all components (libraries, agents, helper) are in the
#       same Git repository, all have to share the same version numbers,
#       because those can not be distinguished with different tags.
#

# Additional CMake modules
include(GNUInstallDirs)				# cmake 2.8.5

# Options
option(BUILD_SHARED_LIBS
    "Global flag to cause add_library to create shared libraries if on."
    OFF
)

# Requirements (commented libraries noted here for later use)
find_library(CONFIG_LIBRARY NAMES config)
#find_library(PTHREAD_LIBRARY NAMES pthread)
#find_library(POPT_LIBRARY NAMES popt)
find_library(MOSQUITTO_LIBRARY NAMES mosquitto)

# Headers
configure_file(
    mqtta-build.h.in
    mqtta-build.h
    @ONLY
)
include_directories(
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Libraries

## mosqhelper
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
install(FILES
	mosqhelper.h
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
)

## mosqagent
add_library(mosqagent
    mosqagent.c
)
set_target_properties(mosqagent PROPERTIES
    OUTPUT_NAME mosqagent
    SOVERSION   ${PROJECT_VERSION_MAJOR}
    VERSION     ${PROJECT_VERSION}
)
target_link_libraries(mosqagent
    mosqhelper
    "${CONFIG_LIBRARY}"
    "${MOSQUITTO_LIBRARY}"
)
install(TARGETS mosqagent
	EXPORT ${PROJECT_NAME}-targets
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(FILES
	mosqagent.h
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
)

# Agents

# mqtt-clock
add_executable(mqtt-clock
    mqtt-clock.c
)
target_link_libraries(mqtt-clock
    mosqhelper
    mosqagent
)
install(TARGETS mqtt-clock
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

# Install CMake targets
install(EXPORT ${PROJECT_NAME}-targets
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

# For later
#target_link_libraries(mqtt-clock ${POPT_LIBRARY})
#target_link_libraries(mqtt-clock ${PTHREAD_LIBRARY})
