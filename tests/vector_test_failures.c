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

void *vector_realloc(void *pts, size_t size)
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

void vector_free(void *ptr)
{
    (void) ptr;
}


START_TEST (test_vector_data_size_overflow_assert)
{
    vector_t *vec;
    vector_create(vec,
        .element_size = sizeof(int),
        .initial_cap = (-1ul / sizeof(int) + 1)
    );
}
END_TEST


START_TEST (test_vector_alloc_size_overflow_assert)
{
    vector_t *vec;
    vector_create(vec,
        .element_size = sizeof(int),
        .initial_cap = (-1ul / sizeof(int) - 5)
    );
}
END_TEST


START_TEST (test_vector_alloc_failure)
{
    vector_t *vec;
    vector_create(vec,
        .element_size = sizeof(int),
        .initial_cap = (MOCK_MEMORY_MAX / sizeof(int))
    ); /* exceedes maximum */

    /* default error handler is called */
}
END_TEST


START_TEST (test_vector_alloc_manual_error_handling)
{
    vector_t *vec;
    vector_error_t error = VECTOR_NO_ERROR;
    vector_create_manual_errhdl(vec, &error,
        .element_size = sizeof(int),
        .initial_cap = MOCK_MEMORY_MAX,
    );

    ck_assert(error == VECTOR_ALLOC_ERROR);

    error = VECTOR_NO_ERROR;
    vector_create_manual_errhdl(vec, &error,
        .element_size = sizeof(int),
        .initial_cap = 200
    );

    ck_assert(error == VECTOR_NO_ERROR);

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

    /*
     * Test assertions when overflow occures
     */
    tcase_add_test_raise_signal(tc_core, test_vector_data_size_overflow_assert, SIGABRT);
    tcase_add_test_raise_signal(tc_core, test_vector_alloc_size_overflow_assert, SIGABRT);
    tcase_add_test_raise_signal(tc_core, test_vector_alloc_failure, SIGABRT);
    tcase_add_test(tc_core, test_vector_alloc_manual_error_handling);

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
