#include <check.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include "../src/vector.h"

#define HEADER_SIZE 24
#define MOCK_MEMORY_MAX 256 * sizeof(int)

static char memory[MOCK_MEMORY_MAX];
static size_t limit = MOCK_MEMORY_MAX;
static size_t allocd = 0;

void* vector_alloc(size_t size)
{
    if (allocd + size > limit)
    {
        return NULL;
    }
    
    void *block = &memory[allocd];
    allocd += size;
    return block;
}

void *vector_realloc(void *ptr, size_t size)
{
    (void) ptr;
    if (allocd + size > limit)
    {
        return NULL;
    }
    
    void *block = &memory[allocd];
    memcpy(block, &memory[0], allocd);
    allocd += size;
    return block;
}

void vector_free(void *ptr)
{
    (void) ptr;
}

void setup(void)
{
    allocd = 0;
    memset(memory, 0, limit);
}

void teardown(void)
{
    // noth
}

START_TEST (test_vector_data_size_overflow_assert)
{
    vector_t *vec = vector_create(
        .element_size = sizeof(int),
        .initial_cap = (-1ul / sizeof(int) + 1)
    );
}
END_TEST


START_TEST (test_vector_alloc_size_overflow_assert)
{
    vector_t *vec = vector_create(
        .element_size = sizeof(int),
        .initial_cap = (-1ul / sizeof(int) - 5)
    );
}
END_TEST


START_TEST (test_vector_alloc_failure)
{
    vector_t *vec = vector_create(
        .element_size = sizeof(int),
        .initial_cap = (MOCK_MEMORY_MAX / sizeof(int))
    ); /* exceedes maximum */
    
    ck_assert_ptr_null(vec);
}
END_TEST


START_TEST (test_vector_resize)
{
    vector_t *vec = vector_create(
        .element_size = sizeof(int),
        .initial_cap = 10
    );

    ck_assert_uint_eq(VECTOR_SUCCESS, vector_resize(&vec, 11, 999));
    ck_assert_uint_eq(VECTOR_ALLOC_ERROR, vector_resize(&vec, limit, VECTOR_ALLOC_ERROR));

    vector_destroy(vec);
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
    tcase_add_test_raise_signal(tc_core, test_vector_data_size_overflow_assert, SIGABRT);
    tcase_add_test_raise_signal(tc_core, test_vector_alloc_size_overflow_assert, SIGABRT);
    tcase_add_test(tc_core, test_vector_resize);

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
