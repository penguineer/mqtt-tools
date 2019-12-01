/*******************************************************************//**
 * \file		mqtta-test-basic.c
 *
 * \brief		Basic unit tests for mqtta library.
 *
 * \author		Alexander Dahl <alex@netz39.de>
 *
 * SPDX-License-Identifier: MIT
 * License-Filename: LICENSES/MIT.txt
 *
 * \copyright	2019 Alexander Dahl, Netz39 e.V., and mqtta contributors
 **********************************************************************/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <mqtt-tools/mqtta.h>

#include "mqtta-build.h"

static void version(void **state) {
    const char *result;

    (void) state; /* unused */

    result = mqtta_version();

    assert_non_null(result);
    assert_string_equal(result, MQTTA_VERSION);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(version),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
