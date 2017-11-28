#
# Copyright (c) 2017 Stefan Haun, Netz39 e.V., and mqtta contributors
#
# SPDX-License-Identifier: MIT
# License-Filename: LICENSES/MIT.txt
#

configure_file(
    mqtta-build.h.in
    mqtta-build.h
    @ONLY
)

install(FILES
	mqtta.h
	mosqhelper.h
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/mqtt-tools"
)
