#include <check.h>
#include <stdlib.h>
#include <signal.h>

#include "../src/vector.h"

static vector_t *vector;
static const int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

void setup(void)
{
    vector_create(vector);
    for (size_t i = 0; i < sizeof(data)/sizeof(data[0]); ++i)
        vector_append_back(&vector, &data[i]);
}

void teardown(void)
{
    vector_destroy(vector);
}

START_TEST (test_vector_swap_ranges)
{
    const int expected[] = {1, 6, 7, 8, 9, 5, 2, 3, 4, 10};
    const size_t idx_a = 1;
    const size_t len_a = 3;
    const size_t idx_b = 5;
    const size_t len_b = 4;
    const size_t init_size = vector_size(vector);

    ck_assert(vector_swap_ranges(&vector, idx_a, len_a, idx_b, len_b));
    size_t size = vector_size(vector);
    ck_assert_uint_eq(size, init_size);

    for (size_t i = 0; i < size; ++i)
    {
        ck_assert_uint_eq(expected[i], *(int*) vector_get(vector, i));
    }
}
END_TEST

START_TEST (test_vector_swap_ranges_messed_order)
{
    const int expected[] = {1, 6, 7, 8, 9, 5, 2, 3, 4, 10};
    const size_t idx_a = 5;
    const size_t len_a = 4;
    const size_t idx_b = 1;
    const size_t len_b = 3;
    const size_t init_size = vector_size(vector);

    ck_assert(vector_swap_ranges(&vector, idx_a, len_a, idx_b, len_b));
    size_t size = vector_size(vector);
    ck_assert_uint_eq(size, init_size);
    for (size_t i = 0; i < size; ++i)
    {
        ck_assert_uint_eq(expected[i], *(int*) vector_get(vector, i));
    }
}
END_TEST

START_TEST (test_vector_reverse)
{
    const size_t init_size = vector_size(vector);

    vector_reverse(&vector);
    size_t size = vector_size(vector);
    ck_assert_uint_eq(size, init_size);
    for (size_t i = 0; i < size; ++i)
    {
        ck_assert_uint_eq(data[size - i - 1], *(int*) vector_get(vector, i));
    }
}
END_TEST


Suite * vector_other_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Vector Other");
    
    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_vector_swap_ranges);
#ifdef CK_FORK
    tcase_add_test_raise_signal(tc_core, test_vector_swap_ranges_messed_order, SIGABRT);
#endif
    tcase_add_test(tc_core, test_vector_reverse);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = vector_other_suite();
    sr = srunner_create(s);

    /* srunner_set_fork_status(sr, CK_NOFORK); */
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
