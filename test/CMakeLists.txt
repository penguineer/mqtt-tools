#
# Copyright 2017–2019 Stefan Haun, Netz39 e.V., and mqtta contributors
#
# SPDX-License-Identifier: MIT
# License-Filename: LICENSES/MIT.txt
#

find_library(CMOCKA_LIBRARY NAMES cmocka)

add_executable(mqtta-test-null
	mqtta-test-null.c
)
target_link_libraries(mqtta-test-null
	"${CMOCKA_LIBRARY}"
)
add_test(NAME test_null
	COMMAND mqtta-test-null
)
