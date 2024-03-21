#include <check.h>
#include <stdlib.h>
#include "../src/vector.h"

#define BOOL_STR(value) (value ? "`true`" : "`false`")

static vector_t *vector;

static void setup_empty(void)
{
    vector_create(vector,
       .element_size = sizeof(int),
       .initial_cap = 10
    );
}

static void teardown(void)
{
    vector_destroy(vector);
}


START_TEST (test_vector_create)
{
    ck_assert_ptr_nonnull(vector);
    ck_assert_uint_eq(vector_capacity(vector), 10);
    ck_assert_uint_eq(vector_initial_capacity(vector), 10);
}
END_TEST


START_TEST (test_vector_truncate)
{
    const size_t new_cap = 1024;
    ck_assert(vector_truncate(&vector, new_cap));
    ck_assert_uint_eq(vector_capacity(vector), new_cap);

    vector_set(vector, new_cap - 1, TMP_REF(int, 999)); /* set last element after truncation */
    ck_assert_mem_eq(vector_get(vector, new_cap - 1), TMP_REF(int, 999), sizeof(int)); /* check memory */

    ck_assert(vector_truncate(&vector, 0)); /* truncate to zero is allowed */
    ck_assert_uint_eq(vector_capacity(vector), 0);
}
END_TEST


Suite * vector_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Vector");
    
    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, setup_empty, teardown);
    tcase_add_test(tc_core, test_vector_create);
    tcase_add_test(tc_core, test_vector_truncate);
    suite_add_tcase(s, tc_core);

    return s;
}


int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = vector_suite();
    sr = srunner_create(s);

    /* srunner_set_fork_status(sr, CK_NOFORK); */
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

