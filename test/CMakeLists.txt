#
# Copyright 2017–2019 Stefan Haun, Netz39 e.V., and mqtta contributors
#
# SPDX-License-Identifier: MIT
# License-Filename: LICENSES/MIT.txt
#

find_package(cmocka CONFIG REQUIRED)

add_executable(mqtta-test-null
	mqtta-test-null.c
)
target_link_libraries(mqtta-test-null
	"${CMOCKA_LIBRARIES}"
)
add_test(NAME mqtta-null
	COMMAND mqtta-test-null
)

add_executable(mqtta-test-basic
	mqtta-test-basic.c
)
target_include_directories(mqtta-test-basic
	PRIVATE
		${PROJECT_BINARY_DIR}/include/mqtt-tools
)
target_link_libraries(mqtta-test-basic
	"${CMOCKA_LIBRARIES}"
	mqtta::mqtta
)
add_test(NAME mqtta-basic
	COMMAND mqtta-test-basic
)
