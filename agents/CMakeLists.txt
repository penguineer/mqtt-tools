#
# Copyright (c) 2017,2018 Stefan Haun, Netz39 e.V., and mqtta contributors
#
# SPDX-License-Identifier: MIT
# License-Filename: LICENSES/MIT.txt
#

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}/include"
)

# mqtt-clock
add_executable(mqtt-clock
    mqtt-clock.c
)
set_target_properties(mqtt-clock PROPERTIES
	C_STANDARD			99
	C_STANDARD_REQUIRED	ON
)
target_link_libraries(mqtt-clock
    mosqhelper
    mqtta
)
install(TARGETS mqtt-clock
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
