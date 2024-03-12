#include <check.h>
#include <stdlib.h>
#include <signal.h>

#include "../src/vector.h"

#define MOCK_MEMORY_MAX 256 * sizeof(int)

static vector_t *vector;

static char memory[MOCK_MEMORY_MAX];
static size_t limit = MOCK_MEMORY_MAX;
static size_t allocd = 0;
static bool alloc_error;
static vector_error_t error_type;

void* xmalloc(size_t size)
{
    if (allocd + size > limit)
    {
        return NULL;
    }
    
    void *block = &memory[allocd];
    allocd += size;
    return block;
}

void *xrealloc(void *pts, size_t size)
{
    if (allocd + size > limit)
    {
        return NULL;
    }
    
    void *block = &memory[allocd];
    memcpy(block, &memory[0], allocd);
    allocd += size;
    return block;
}

void xfree(void *ptr)
{
    (void) ptr;
}

static void test_error_callback(vector_t **vector, vector_error_t error, void *param)
{
    alloc_error = true;
    error_type = error;
}

void setup(void)
{
    alloc_error = false;
    allocd = 0;
    vector_create(vector,
        .error_handler = {.callback = test_error_callback}
    );
}

void teardown(void)
{
    vector_destroy(vector);
}


START_TEST (test_vector_data_size_overflow_assert)
{
    vector_t *vec;
    vector_create(vec, .initial_cap = (-1ul / sizeof(int) + 1));
}
END_TEST

START_TEST (test_vector_alloc_size_overflow_assert)
{
    vector_t *vec;
    vector_create(vec, .initial_cap = (-1ul / sizeof(int) - 5));
}
END_TEST

START_TEST (test_vector_out_of_memory_grow)
{
    limit = (c_header_size + 10 * sizeof(int)) /* initial capacity */
          + (c_header_size + 15 * sizeof(int)); /* grown */

    size_t iter_will_fail = 15 * 0.75;

    for (int i = 1; i < iter_will_fail; ++i)
    {
        ck_assert(vector_append_back(&vector, TMP_REF(int, 1)));
        ck_assert(!alloc_error);
    }

    ck_assert(vector_append_back(&vector, TMP_REF(int, 1))); /* operation succeeded because size < capacity */
    ck_assert(alloc_error);
    ck_assert_int_eq(error_type, VECTOR_GROW_ALLOC_ERROR); 
}
END_TEST

START_TEST (test_vector_out_of_memory_shrink)
{
    limit = (c_header_size + 10 * sizeof(int)) /* initial capacity */
          + (c_header_size + 15 * sizeof(int)) /* grown */
          + (c_header_size + 10 * sizeof(int)) /* shrink back to initial */;

    size_t first_grow = 10 * 0.75;

    for (int i = 1; i <= first_grow; ++i)
    {
        ck_assert(vector_append_back(&vector, TMP_REF(int, 1)));
        ck_assert(!alloc_error);
    }
    
    size_t first_shrink = 15 * 0.25;

    while (vector_size(vector) > first_shrink)
    {
        vector_pop_back(&vector);
        ck_assert(!alloc_error);
    }

    vector_pop_back(&vector);
    ck_assert(alloc_error);
    ck_assert_int_eq(error_type, VECTOR_SHRINK_ALLOC_ERROR); 
}
END_TEST

Suite * vector_other_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Vector Failures");
    
    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, setup, teardown);

/*
 * Test assertions when overflow occures
 */
#ifdef CK_FORK
    tcase_add_test_raise_signal(tc_core, test_vector_data_size_overflow_assert, SIGABRT);
    tcase_add_test_raise_signal(tc_core, test_vector_alloc_size_overflow_assert, SIGABRT);
#endif
    tcase_add_test(tc_core, test_vector_out_of_memory_grow);
    tcase_add_test(tc_core, test_vector_out_of_memory_shrink);
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
