#include <check.h>
#include <stdlib.h>
#include "../src/vector.h"

#define BOOL_STR(value) (value ? "`true`" : "`false`")

static ssize_t cmp(const void *a, const void *b, void *param)
{
    (void)param;
    return (ssize_t) *(int*)a - *(int*)b;
}

static bool pred(const void *a, const void *b, void *param)
{
    return *(int*)a == *(int*)b;
}

static vector_t *vector;

static void setup_empty(void)
{
    vector_create(vector,
       .esize = sizeof(int),
       .initial_cap = 10,
       .shrink_threshold = 0.35f,
       .grow_threshold = 0.75f,
       .grow_factor = 1.5f
    );
}

static void teardown(void)
{
    vector_destroy(vector);
}


START_TEST (test_vector_create)
{
    ck_assert_ptr_nonnull(vector);
    ck_assert_uint_eq(vector_size(vector), 0);
    ck_assert_uint_eq(vector_capacity(vector), 10);
}
END_TEST


START_TEST (test_vector_clone)
{
    vector_append_back(&vector, TMP_REF(int, 64));

    vector_t *clone = vector_clone(vector);
    ck_assert_ptr_ne(vector, clone);
    ck_assert_uint_eq(vector_capacity(vector), vector_capacity(clone));
    ck_assert_uint_eq(vector_size(vector), vector_size(clone));
    ck_assert_mem_eq(vector_get(vector, 0), vector_get(clone, 0), sizeof(int));
    vector_destroy(clone);
}
END_TEST


START_TEST (test_vector_append_single)
{
    bool status = vector_append_back(&vector, TMP_REF(int, 1));
    ck_assert_msg(status,
        "Append may not fail unless failed to reallocate memory when growing.\n"
        "Expected %s, but got %s\n", BOOL_STR(true), BOOL_STR(status));
    ck_assert_int_eq(*(int*)vector_first(vector), 1);
    ck_assert_uint_eq(vector_size(vector), 1);
    ck_assert_uint_eq(vector_capacity(vector), 10);
}
END_TEST


START_TEST (test_vector_pop_empty)
{
    vector_pop_back(&vector);
}
END_TEST


START_TEST (test_vector_pop_single)
{
    vector_append_back(&vector, TMP_REF(int, 64));

    vector_pop_back(&vector);
    ck_assert_uint_eq(vector_size(vector), 0);
}
END_TEST


START_TEST (test_vector_first)
{
    ck_assert_ptr_null(vector_first(vector));

    bool status = vector_append_back(&vector, TMP_REF(int, 1));
    ck_assert(status);

    int * elem = (int*) vector_first(vector);
    ck_assert_int_eq(*elem, 1);
    ck_assert_ptr_eq(vector_first(vector), vector_last(vector));

    status = vector_append_back(&vector, TMP_REF(int, 1));
    ck_assert(status);

    ck_assert_ptr_ne(vector_first(vector), vector_last(vector));
    ck_assert_ptr_eq(vector_first(vector), elem);
}
END_TEST


START_TEST (test_vector_last)
{
    ck_assert_ptr_null(vector_last(vector));

    bool status = vector_append_back(&vector, TMP_REF(int, 1));
    ck_assert(status);

    int * elem = (int *) vector_last(vector);
    ck_assert_int_eq(*elem, 1);
    ck_assert_ptr_eq(vector_first(vector), vector_last(vector));

    status = vector_append_back(&vector, TMP_REF(int, 2));
    ck_assert(status);

    int *last = (int *) vector_last(vector);
    ck_assert_int_eq(*last, 2);
    ck_assert_ptr_ne(last, elem);
}
END_TEST



START_TEST (test_vector_get)
{
    ck_assert_ptr_null(vector_get(vector, 0));
    ck_assert_ptr_null(vector_get(vector, 100));
    
    ck_assert(vector_append_back(&vector, TMP_REF(int, 0)));
    ck_assert_ptr_nonnull(vector_get(vector, 0));
    
    ck_assert(vector_append_back(&vector, TMP_REF(int, 999)));
    ck_assert_mem_eq(vector_get(vector, 1), TMP_REF(int, 999), sizeof(int));
}
END_TEST



START_TEST (test_vector_set)
{
    ck_assert_msg(!vector_set(vector, 0, TMP_REF(int, 999)),
        "Setting value beyond vector's size must fail.");

    for (int i = 0; i < 20; ++i)
    {
        ck_assert(vector_append_back(&vector, &i));
    }

    for (int i = 0; i < 20; ++i)
    {
        ck_assert(vector_set(vector, i, TMP_REF(int, 100)));
    }

    for (int i = 0; i < 20; ++i)
    {
        ck_assert_mem_eq(vector_get(vector, i), TMP_REF(int, 100), sizeof(int));
    }
}
END_TEST


START_TEST (test_vector_append_front)
{
    for (int appended = 0; appended < 100; ++appended)
    {
        ck_assert(vector_append_front(&vector, &appended));
        for (int prev_appended = appended, index = 0; prev_appended >= 0; --prev_appended, ++index)
        {
            ck_assert_mem_eq(vector_get(vector, index), &prev_appended, sizeof(int));
        }
    }
    ck_assert_uint_eq(vector_size(vector), 100);
}
END_TEST


START_TEST (test_vector_pop_front)
{
    vector_pop_front(&vector);

    for (int appended = 0; appended < 100; ++appended)
    {
        ck_assert(vector_append_front(&vector, &appended));
    }

    for (int prev_appended = 99; prev_appended >= 0; --prev_appended)
    {
        ck_assert_mem_eq(vector_first(vector), &prev_appended, sizeof(int));
        vector_pop_front(&vector);
    }
}
END_TEST


START_TEST (test_vector_truncate)
{
    bool status = vector_truncate(&vector, 100);
    ck_assert(status);
    ck_assert_uint_eq(vector_capacity(vector), 100);
    
    for (int i = 0; i < 20; ++i)
    {
        ck_assert(vector_append_back(&vector, &i));
    }

    status = vector_truncate(&vector, 0);
    ck_assert(status);
    ck_assert_uint_eq(vector_size(vector), vector_capacity(vector));
    ck_assert_uint_eq(vector_size(vector), 10);
}
END_TEST


START_TEST (test_vector_clear)
{
    for (int i = 0; i < 20; ++i)
    {
        ck_assert(vector_append_back(&vector, &i));
    }
    size_t cap = vector_capacity(vector);
    vector_clear(vector);
    ck_assert_uint_eq(vector_size(vector), 0);
    ck_assert_uint_eq(vector_capacity(vector), cap);
}
END_TEST


START_TEST (test_vector_contains)
{
    ck_assert(!vector_contains(vector, TMP_REF(int, 0)));
    ck_assert(vector_append_back(&vector, TMP_REF(int, 0)));
    ck_assert(vector_contains(vector, TMP_REF(int, 0)));
    ck_assert(vector_append_back(&vector, TMP_REF(int, 999)));
    ck_assert(vector_contains(vector, TMP_REF(int, 999)));
    vector_pop_back(&vector);
    ck_assert(!vector_contains(vector, TMP_REF(int, 999)));
}
END_TEST


START_TEST (test_vector_find)
{
    void *p;
    
    ck_assert(vector_append_front(&vector, TMP_REF(int, 10)));
    ck_assert(vector_append_front(&vector, TMP_REF(int, 20)));
    ck_assert(vector_append_front(&vector, TMP_REF(int, 30)));

    p = vector_find(vector, TMP_REF(int, 0), pred, NULL);
    ck_assert_ptr_null(p);
    
    p = vector_find(vector, TMP_REF(int, 10), pred, NULL);
    ck_assert_ptr_nonnull(p);
    ck_assert_mem_eq(p, TMP_REF(int, 10), sizeof(int));

    p = vector_find(vector, TMP_REF(int, 20), pred, NULL);
    ck_assert_ptr_nonnull(p);
    ck_assert_mem_eq(p, TMP_REF(int, 20), sizeof(int));

    p = vector_find(vector, TMP_REF(int, 30), pred, NULL);
    ck_assert_ptr_nonnull(p);
    ck_assert_mem_eq(p, TMP_REF(int, 30), sizeof(int));
}
END_TEST


START_TEST (test_vector_insert_at)
{
    ck_assert(vector_insert_at(&vector, 0, TMP_REF(int, 999)));
    ck_assert_uint_eq(vector_size(vector), 1);
    ck_assert_mem_eq(vector_get(vector, 0), TMP_REF(int, 999), sizeof(int));

    ck_assert(vector_insert_at(&vector, 9, TMP_REF(int, 999)));
    ck_assert_uint_eq(vector_size(vector), 10);
    ck_assert_mem_eq(vector_get(vector, 9), TMP_REF(int, 999), sizeof(int));

    for (int i = 1; i < 9; ++i) /* fill the gaps */
    {
        ck_assert(vector_insert_at(&vector, i, &i));
    }

    /* insert at index 5 */
    ck_assert(vector_insert_at(&vector, 5, TMP_REF(int, 777)));
    ck_assert_mem_eq(vector_get(vector, 5), TMP_REF(int, 777), sizeof(int));

    for (int i = 6; i < 9; ++i) /* check that rest of the elements shifted */
    {
        ck_assert_mem_eq(vector_get(vector, i), TMP_REF(int, i - 1), sizeof(int));
    }
}
END_TEST


START_TEST (test_vector_remove)
{
    vector_remove(&vector, 100);

    for (int i = 0; i < 20; ++i)
    {
        ck_assert(vector_append_back(&vector, &i));
    }
    vector_remove(&vector, 5);
    ck_assert_uint_eq(vector_size(vector), 19);
    
    for (int i = 5; i < 19; ++i)
    {
        ck_assert_mem_eq(vector_get(vector, i), TMP_REF(int, i + 1), sizeof(int));
    }
}
END_TEST


START_TEST (test_vector_binary_insert)
{
    int table[] = {10, 23, 31, 1, 4, 22, 34, 71, 15, 18, 0, 5, 45, 99, 2, 0, 71, 22};
    size_t size = sizeof(table)/sizeof(*table);

    for (int i = 0; i < size; ++i)
    {
        ck_assert(vector_binary_insert(&vector, cmp, &table[i], NULL, NULL));
    }

    ck_assert_uint_eq(size, vector_size(vector));

    bool ordered = true;
    int curr = 0;
    int prev = *(int*)vector_first(vector);
    vector_pop_front(&vector);

    while (ordered && (0 < vector_size(vector)))
    {
        curr = *(int*)vector_first(vector);
        vector_pop_front(&vector);
        ordered = prev <= curr;
        prev = curr;
    }
    ck_assert_msg(ordered, "Binary insert has to preserve `cmp` order.");
}
END_TEST


START_TEST (test_vector_binary_insert_random)
{
    srand(3011);
    int random;

    for (int i = 0; i < 100; ++i)
    {
        random = rand();
        vector_binary_insert(&vector, cmp, &random, NULL, NULL);
    }
}
END_TEST


START_TEST (test_vector_binary_find)
{
    int table[] = {10, 23, 31, 1, 4, 34, 71, 15, 18, 5, 45, 99, 2, 0, 71, 22};
    size_t size = sizeof(table)/sizeof(*table);

    for (int i = 0; i < size; ++i)
    {
        ck_assert(vector_binary_insert(&vector, cmp, &table[i], NULL, NULL));
    }

    for (int i = 0; i < size; ++i)
    {
        void *p = vector_binary_find(vector, cmp, &table[i], NULL);
        ck_assert_mem_eq(p, &table[i], sizeof(int));
    }
}
END_TEST


START_TEST (test_vector_insert_fill)
{
    bool status = vector_insert_fill(&vector, 0, 10, TMP_REF(int, 999));

    ck_assert(status);

    size_t size = vector_size(vector);
    ck_assert_uint_eq(size, 10);

    for (int i = 0; i < size; ++i)
    {
        ck_assert_mem_eq(vector_get(vector, i), TMP_REF(int, 999), sizeof(int));
    }
}
END_TEST


START_TEST (test_vector_insert_fill_inbetween)
{
    for (int i = 0; i < 20; ++i)
    {
        ck_assert(vector_append_back(&vector, &i));
    }
    const size_t initial_size = vector_size(vector);
    const size_t insert_idx = 9;
    const size_t amount = 5;

    bool status = vector_insert_fill(&vector, insert_idx, amount, TMP_REF(int, 999));
    ck_assert(status);

    const size_t size = vector_size(vector);
    ck_assert_uint_eq(size, amount + initial_size);

    for (int i = 0, k = 0; i < size; ++i)
    {
        if (i >= insert_idx && i < (insert_idx + amount))
        {
            ck_assert_mem_eq(vector_get(vector, i), TMP_REF(int, 999), sizeof(int));
        }
        else
        {
            ck_assert_mem_eq(vector_get(vector, i), &k, sizeof(int));
            ++k;
        }
    }
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
    tcase_add_test(tc_core, test_vector_clone);
    tcase_add_test(tc_core, test_vector_append_single);
    tcase_add_test(tc_core, test_vector_pop_empty);
    tcase_add_test(tc_core, test_vector_pop_single);
    tcase_add_test(tc_core, test_vector_first);
    tcase_add_test(tc_core, test_vector_last);
    tcase_add_test(tc_core, test_vector_get);
    tcase_add_test(tc_core, test_vector_set);
    tcase_add_test(tc_core, test_vector_truncate);
    tcase_add_test(tc_core, test_vector_clear);
    tcase_add_test(tc_core, test_vector_append_front);
    tcase_add_test(tc_core, test_vector_pop_front);
    tcase_add_test(tc_core, test_vector_contains);
    tcase_add_test(tc_core, test_vector_find);
    tcase_add_test(tc_core, test_vector_insert_at);
    tcase_add_test(tc_core, test_vector_remove);
    tcase_add_test(tc_core, test_vector_binary_insert);
    tcase_add_test(tc_core, test_vector_binary_insert_random);
    tcase_add_test(tc_core, test_vector_binary_find);
    tcase_add_test(tc_core, test_vector_insert_fill_inbetween);

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

