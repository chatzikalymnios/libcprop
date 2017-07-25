#include <stdlib.h>
#include <check.h>

#include "cprop.h"

void setup() {

}

void teardown() {

}

START_TEST(test_int_eq) {
    ck_assert_int_eq(1, 1);
}
END_TEST

Suite * cprop_suite() {
    Suite *s;
    TCase *tc_core;

    s = suite_create("cprop");

    tc_core = tcase_create("core");

    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_int_eq);
    suite_add_tcase(s, tc_core);

    return s;
}

int main() {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = cprop_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

