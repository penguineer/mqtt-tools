#
# Copyright 2017,2018 Stefan Haun, Netz39 e.V., and mqtta contributors
#
# SPDX-License-Identifier: MIT
# License-Filename: LICENSES/MIT.txt
#

cmake_minimum_required(VERSION 3.1)

project(mqtt-tools
    VERSION 0.1.0
    LANGUAGES C)

set(MQTTA_DESCRIPTION "A library to simplify agents that react on/with external events or MQTT messages by handling 95% of the MQTT use-cases.")
set(MQTTA_URL "https://github.com/penguineer/mqtt-tools")

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
add_subdirectory(include/mqtt-tools)

# Libraries
add_subdirectory(lib)

# Agents
add_subdirectory(agents)

# Install CMake targets
install(EXPORT ${PROJECT_NAME}-targets
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

# Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
		"${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)
	add_custom_target(doc_${PROJECT_NAME} "${DOXYGEN_EXECUTABLE}"
		WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
		COMMENT "Generating API documentation with Doxygen" VERBATIM
		SOURCES "${PROJECT_SOURCE_DIR}/Doxyfile.in"
	)
endif(DOXYGEN_FOUND)

# For later
#target_link_libraries(mqtt-clock ${POPT_LIBRARY})
#target_link_libraries(mqtt-clock ${PTHREAD_LIBRARY})
