#include <check.h>
#include <stdlib.h>
#include "../src/vector.h"

static int cmp(const void *a, const void *b, void *param)
{
    (void)param;
    return *(int*)a - *(int*)b;
}

static vector_t *vector;

void setup(void)
{
    vector_create(vector,
       .esize = sizeof(int),
       .initial_cap = 10,
       .shrink_threshold = 0.25f,
       .grow_threshold = 0.75f,
       .grow_factor = 1.5f
    );

}

void teardown(void)
{
    vector_destroy(vector);
}

START_TEST (test_vector_other)
{
    ck_assert_ptr_nonnull(vector);
    ck_assert_uint_eq(vector_size(vector), 0);
    ck_assert_uint_eq(vector_capacity(vector), 10);
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
    tcase_add_test(tc_core, test_vector_other);
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
    return 0;
}
/*  */
#if 0
int main(void)
{
    vector_t *vec;

    vector_create(vec,
       .esize = sizeof(int),
       .initial_cap = 10,
       .shrink_threshold = 0.25f,
       .grow_threshold = 0.75f,
       .grow_factor = 1.5f
    );

    /* for (int n = 0; n < 10; ++n) */
    /* { */
    /*     vector_append_back(&vec, &n); */
    /*     printf(">> %d\n", n); */
    /* } */

    vector_binary_insert(&vec, cmp, (int[1]){9}, NULL, NULL);
    vector_binary_insert(&vec, cmp, (int[1]){6}, NULL, NULL);
    vector_binary_insert(&vec, cmp, (int[1]){2}, NULL, NULL);
    vector_binary_insert(&vec, cmp, (int[1]){0}, NULL, NULL);
    vector_binary_insert(&vec, cmp, (int[1]){5}, NULL, NULL);
    vector_binary_insert(&vec, cmp, (int[1]){4}, NULL, NULL);
    vector_binary_insert(&vec, cmp, (int[1]){7}, NULL, NULL);
    vector_binary_insert(&vec, cmp, (int[1]){3}, NULL, NULL);
    vector_binary_insert(&vec, cmp, (int[1]){1}, NULL, NULL);
    vector_binary_insert(&vec, cmp, (int[1]){8}, NULL, NULL);

    /* int v = 10; */
    /* size_t i = binary_find_insert_place(vec, cmp, &v, NULL, 0, 9); */
    /* printf("  > %zu \n", i); */

    for (int n = 0; n < 10; ++n)
    {
        int *v = (int*)vector_binary_find(vec, cmp, &n, NULL);
        printf("== %d\n", *v);
    }

    /* for (int n = 0; n < 10; ++n) */
    /* { */
    /*     int *v = (int*)vector_last(vec); */
    /*     printf("<-- %d\n", *v); */
    /*     vector_pop_back(&vec); */
    /* } */
    vector_destroy(vec);
    return 0;
}

#endif

