#include <check.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include "../src/vector.h"

#define MOCK_MEMORY_MAX 256 * sizeof(int)

typedef struct mock_alloc
{
    char memory[MOCK_MEMORY_MAX];
    size_t limit;
    size_t allocd;
}
mock_alloc_t;

typedef struct
{
    void *allocator;
}
alloc_param_t;


void* vector_alloc(size_t size, void *const param)
{
    mock_alloc_t *alloc = ((alloc_param_t*)param)->allocator;
    if (alloc->allocd + size > alloc->limit)
    {
        return NULL;
    }

    void *block = &alloc->memory[alloc->allocd];
    alloc->allocd += size;
    return block;
}


void *vector_realloc(void *ptr, size_t size, void *const param)
{
    mock_alloc_t *alloc = ((alloc_param_t*)param)->allocator;
    (void) ptr;
    if (alloc->allocd + size > alloc->limit)
    {
        return NULL;
    }

    void *block = &alloc->memory[alloc->allocd];
    memcpy(block, &alloc->memory[0], alloc->allocd);
    alloc->allocd += size;
    return block;
}


void vector_free(void *ptr, void *const param)
{
    // not actually freeing for the sake of a test.
    (void) param;
    (void) ptr;
}

void setup(void)
{
}

void teardown(void)
{
    // noth
}


START_TEST (test_vector_create_no_opts_provided)
{
    vector_opts_t *opts = NULL;
    vector_t *vec = vector_create_(opts);
    (void) vec;
}
END_TEST


START_TEST (test_vector_create_no_element_size)
{
    vector_t *vec = vector_create();
    (void) vec;
}
END_TEST


START_TEST (test_vector_get_ext_on_null)
{
    vector_t *vec = NULL;
    (void) vector_get_ext_header(vec);
}
END_TEST


START_TEST (test_vector_get_ext_unallocd)
{
    mock_alloc_t alloc = { .limit = MOCK_MEMORY_MAX }; /* will be referenced */
    alloc_param_t alloc_param = { &alloc }; /* struct will be copied into the vectors memory region */

    vector_t *vec = vector_create(.alloc_opts = alloc_opts(.size = sizeof(alloc_param), .data = &alloc_param));
    (void) vector_get_ext_header(vec);
}
END_TEST


START_TEST (test_vector_ext_size_null)
{
    vector_t *vec = NULL;
    (void) vector_ext_header_size(vec);
}
END_TEST


START_TEST (test_vector_data_size_overflow_assert)
{
    mock_alloc_t alloc = { .limit = MOCK_MEMORY_MAX }; /* will be referenced */
    alloc_param_t alloc_param = { &alloc }; /* struct will be copied into the vectors memory region */

    vector_t *vec = vector_create(
        .element_size = sizeof(int),
        .initial_cap = (-1ul / sizeof(int) + 1),
        .alloc_opts = alloc_opts(
            .size = sizeof(alloc_param),
            .data = &alloc_param
        ),
    ); 

    (void) vec;
}
END_TEST


START_TEST (test_vector_alloc_size_overflow_assert)
{
    mock_alloc_t alloc = { .limit = MOCK_MEMORY_MAX };
    alloc_param_t alloc_param = { &alloc }; /* struct will be copied into the vectors memory region */

    vector_t *vec = vector_create(
        .element_size = sizeof(int),
        .initial_cap = (-1ul / sizeof(int) - 5),
        .alloc_opts = alloc_opts(
            .size = sizeof(alloc_param),
            .data = &alloc_param
        ),
    );
    (void) vec;
}
END_TEST


START_TEST (test_vector_alloc_failure)
{
    mock_alloc_t alloc = { .limit = MOCK_MEMORY_MAX };
    alloc_param_t alloc_param = { &alloc }; /* struct will be copied into the vectors memory region */

    vector_t *vec = vector_create(
        .element_size = sizeof(int),
        .initial_cap = (MOCK_MEMORY_MAX / sizeof(int)),
        .alloc_opts = alloc_opts(
            .size = sizeof(alloc_param),
            .data = &alloc_param
        ),
    ); /* exceedes maximum */

    ck_assert_ptr_null(vec);
}
END_TEST


START_TEST (test_vector_resize_null)
{
    vector_t *vec = NULL;
    vector_resize(&vec, 100, VECTOR_ALLOC_ERROR);
}
END_TEST


START_TEST (test_vector_resize_pointer_null)
{
    vector_resize(NULL, 100, VECTOR_ALLOC_ERROR);
}
END_TEST


START_TEST (test_vector_clone_failure)
{
    mock_alloc_t alloc = { .limit = MOCK_MEMORY_MAX };
    alloc_param_t alloc_param = { &alloc }; /* struct will be copied into the vectors memory region */

    vector_t *vec = vector_create(
        .element_size = sizeof(int),
        .initial_cap = (MOCK_MEMORY_MAX / sizeof(int) / 2),
        .alloc_opts = alloc_opts(
            .size = sizeof(alloc_param),
            .data = &alloc_param
        ),
    );

    vector_t *clone = vector_clone(vec); /* failed, not enough memory for cloning */
    ck_assert_ptr_null(clone);
}


START_TEST (test_vector_destroy_null)
{
    vector_t *vec = NULL;
    vector_destroy(vec);
}
END_TEST


START_TEST (test_vector_clone_null)
{
    vector_t *vec = NULL;
    vector_clone(vec);
}
END_TEST


START_TEST (test_vector_resize)
{
    mock_alloc_t alloc = { .limit = MOCK_MEMORY_MAX };
    alloc_param_t alloc_param = { &alloc }; /* struct will be copied into the vectors memory region */

    vector_t *vec = vector_create(
        .element_size = sizeof(int),
        .initial_cap = 10,
        .alloc_opts = alloc_opts(
            .size = sizeof(alloc_param),
            .data = &alloc_param
        ),
    );

    ck_assert_uint_eq(VECTOR_SUCCESS, vector_resize(&vec, 11, 999));
    ck_assert_uint_eq(VECTOR_ALLOC_ERROR, vector_resize(&vec, alloc.limit, VECTOR_ALLOC_ERROR));

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

    tcase_add_test_raise_signal(tc_core, test_vector_create_no_opts_provided, SIGABRT);
    tcase_add_test_raise_signal(tc_core, test_vector_create_no_element_size, SIGABRT);

    tcase_add_test_raise_signal(tc_core, test_vector_ext_size_null, SIGABRT);
    tcase_add_test_raise_signal(tc_core, test_vector_get_ext_on_null, SIGABRT);
    tcase_add_test_raise_signal(tc_core, test_vector_get_ext_unallocd, SIGABRT);

    /*
     * Test assertions when overflow occures
     */
    tcase_add_test_raise_signal(tc_core, test_vector_data_size_overflow_assert, SIGABRT);
    tcase_add_test_raise_signal(tc_core, test_vector_alloc_size_overflow_assert, SIGABRT);

    tcase_add_test(tc_core, test_vector_resize);
    tcase_add_test(tc_core, test_vector_alloc_failure);
    tcase_add_test(tc_core, test_vector_clone_failure);

    tcase_add_test_raise_signal(tc_core, test_vector_clone_null, SIGABRT);
    tcase_add_test_raise_signal(tc_core, test_vector_destroy_null, SIGABRT);

    tcase_add_test_raise_signal(tc_core, test_vector_resize_null, SIGABRT);
    tcase_add_test_raise_signal(tc_core, test_vector_resize_pointer_null, SIGABRT);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
#ifdef _WIN64
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

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
