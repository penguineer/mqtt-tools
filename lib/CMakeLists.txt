#
# 2017–2019 Stefan Haun, Netz39 e.V., and mqtta contributors
#
# SPDX-License-Identifier: MIT
# License-Filename: LICENSES/MIT.txt
#

# mosqhelper
add_library(mosqhelper
    mosqhelper.c
)
add_library(mqtta::mosqhelper ALIAS mosqhelper)
set_target_properties(mosqhelper PROPERTIES
    OUTPUT_NAME         mosqhelper
    SOVERSION           ${PROJECT_VERSION_MAJOR}
    VERSION             ${PROJECT_VERSION}
    C_STANDARD          99
    C_STANDARD_REQUIRED ON
)
target_include_directories(mosqhelper
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
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
add_library(mqtta::mqtta ALIAS mqtta)
set_target_properties(mqtta PROPERTIES
    OUTPUT_NAME         mqtta
    SOVERSION           ${PROJECT_VERSION_MAJOR}
    VERSION             ${PROJECT_VERSION}
    C_STANDARD          99
    C_STANDARD_REQUIRED ON
)
target_include_directories(mqtta
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	PRIVATE
		$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/mqtt-tools>
)
target_link_libraries(mqtta
	PRIVATE
		mqtta::mosqhelper
		"${CONFIG_LIBRARY}"
		"${MOSQUITTO_LIBRARY}"
)
install(TARGETS mqtta
	EXPORT ${PROJECT_NAME}-targets
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
