#
# Copyright (c) 2017 Stefan Haun, Netz39 e.V., and mqtta contributors
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
target_link_libraries(mqtt-clock
    mosqhelper
    mqtta
)
install(TARGETS mqtt-clock
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
