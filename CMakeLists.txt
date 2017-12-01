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

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

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

# For later
#target_link_libraries(mqtt-clock ${POPT_LIBRARY})
#target_link_libraries(mqtt-clock ${PTHREAD_LIBRARY})
