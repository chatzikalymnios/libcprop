#include <stdlib.h>
#include <check.h>

#include "cprop.h"

Properties *prop;

void setup() {
    prop = cprop_load("tests/test.properties");
    ck_assert_ptr_nonnull(prop);
}

void teardown() {
    cprop_free(prop);
}

START_TEST(check_cprop_get) {
    ck_assert_str_eq(cprop_get(prop, "prop1"), "val1");
    ck_assert_str_eq(cprop_get(prop, "prop2"), "val2");
    ck_assert_str_eq(cprop_get(prop, "prop3"), "val3");
    ck_assert_str_eq(cprop_get(prop, "prop4"), "val4");
    ck_assert_str_eq(cprop_get(prop, "prop with spaces"), "value with spaces");
}
END_TEST

START_TEST(check_cprop_set) {
    // Create new prop
    int res = cprop_set(prop, "newProp", "newValue");
    ck_assert_int_eq(res, 0);
    ck_assert_str_eq(cprop_get(prop, "newProp"), "newValue");

    // Update existing prop
    res = cprop_set(prop, "prop4", "newVal4");
    ck_assert_int_eq(res, 0);
    ck_assert_str_eq(cprop_get(prop, "prop4"), "newVal4");
}
END_TEST

START_TEST(check_cprop_delete) {
    int res = cprop_delete(prop, "prop3");
    ck_assert_int_eq(res, 0);
    ck_assert_ptr_null(cprop_get(prop, "prop3"));
}
END_TEST

Suite * cprop_suite() {
    Suite *s;
    TCase *tc_core;

    s = suite_create("cprop");

    tc_core = tcase_create("core");

    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, check_cprop_get);
    tcase_add_test(tc_core, check_cprop_set);
    tcase_add_test(tc_core, check_cprop_delete);
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

