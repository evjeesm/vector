#include <check.h>
#include <stdlib.h>
#include <time.h>

#include "../src/vector.h"

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

static void random_fill(vector_t *const vector, const size_t size)
{
    srand(time(0));
    for (size_t i = 0; i < size; ++i)
    {
        int value = rand() % 100;
        vector_set(vector, i, &value);
    }
}

START_TEST (test_vector_create)
{
    const size_t expected = 10;

    ck_assert_ptr_nonnull(vector);

    ck_assert_uint_eq(vector_capacity(vector), expected);
    ck_assert_uint_eq(vector_initial_capacity(vector), expected);
}
END_TEST


START_TEST (test_vector_get_set)
{
    int array[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    for (int i = 0; i < sizeof(array)/sizeof(array[0]); ++i)
    {
        vector_set(vector, i, &array[i]);
    }

    for (int i = 0; i < sizeof(array)/sizeof(array[0]); ++i)
    {
        int *element = (int*) vector_get(vector, i);
        ck_assert_int_eq(*element, array[i]);
    }
}
END_TEST


START_TEST (test_vector_set_zero)
{
    const size_t capacity = vector_capacity(vector);
    random_fill(vector, capacity);

    for (size_t i = 0; i < capacity; ++i)
    {
        vector_set_zero(vector, i);
    }

    const int expected = 0x0;
    for (size_t i = 0; i < capacity; ++i)
    {
        ck_assert_mem_eq(vector_get(vector, i), &expected, sizeof(int));
    }
}
END_TEST


START_TEST (test_vector_clone)
{
    const size_t capacity = vector_capacity(vector);
    random_fill(vector, capacity);
    vector_t *clone = vector_clone(vector);

    ck_assert(clone);
    ck_assert_uint_eq(vector_capacity(clone), vector_capacity(vector));
    ck_assert_uint_eq(vector_initial_capacity(clone), vector_initial_capacity(vector));
    ck_assert_uint_eq(vector_element_size(clone), vector_element_size(vector));

    for (size_t i = 0; i < capacity; ++i)
    {
        int *orig_value = (int*) vector_get(vector, i);
        int *clone_value = (int*) vector_get(clone, i);
        ck_assert_int_eq(*orig_value, *clone_value);
    }

    vector_destroy(clone);
}
END_TEST


START_TEST (test_vector_truncate)
{
    size_t expected_cap = 1024;
    const int expected_value = 999;

    ck_assert(vector_truncate(&vector, expected_cap));
    ck_assert_uint_eq(vector_capacity(vector), expected_cap);

    /* set last element after truncation */
    vector_set(vector, expected_cap - 1, &expected_value);
    ck_assert_int_eq(*(int*) vector_get(vector, expected_cap - 1), expected_value);

    expected_cap = 0; /* truncate to zero is allowed */
    ck_assert(vector_truncate(&vector, expected_cap));
    ck_assert_uint_eq(vector_capacity(vector), expected_cap);
}
END_TEST


START_TEST (test_vector_copy)
{
    const size_t capacity = vector_capacity(vector);
    random_fill(vector, capacity);

    /* copy to external memory */
    int output[3];
    vector_copy(vector, (char*) output, 0, 3);

    ck_assert_mem_eq(vector_get(vector, 0), output, sizeof(int) * 3);

    /* duplicating first 3 elements in vector memory */
    vector_copy(vector, vector_get(vector, 3), 0, 3);

    ck_assert_mem_eq(vector_get(vector, 0), vector_get(vector, 3), sizeof(int) * 3);
}
END_TEST


START_TEST (test_vector_move)
{
    const size_t capacity = vector_capacity(vector);

    /* fill with indices */
    for (int i = 0; i < capacity; ++i)
    {
        vector_set(vector, i, &i);
    }

    size_t from = 0;
    size_t to = 3;
    const size_t length = 6;

    /* test overlapping region */
    vector_move(vector, vector_get(vector, to), from, length);

    for (size_t i = to; i < to + length; ++i)
    {
        ck_assert_int_eq(*(int*) vector_get(vector, i), i - to);
    }
}
END_TEST


START_TEST (test_vector_shift)
{
    const size_t capacity = vector_capacity(vector);

    /* fill half with indices */
    for (int i = 0; i < capacity/2; ++i)
    {
        vector_set(vector, i, &i);
    }

    /* copy first half to second */
    vector_shift(vector, 0, capacity/2, capacity/2);

    ck_assert_mem_eq(vector_get(vector, 0), vector_get(vector, capacity/2), sizeof(int) * capacity/2);

    /* shift elements 1 position left */
    vector_shift(vector, 1, capacity - 1, -1);

    /* 1 2 3 4 0 1 2 3 4 4 */
    ck_assert_int_eq(*(int*) vector_get(vector, 0), 1);
    ck_assert_int_eq(*(int*) vector_get(vector, capacity-1), 4);
    ck_assert_int_eq(*(int*) vector_get(vector, capacity-2), 4);
}
END_TEST


START_TEST (test_vector_swap)
{
    const size_t capacity = vector_capacity(vector);
    random_fill(vector, capacity);

    const size_t index_a = 4;
    const size_t index_b = 5;

    int value_a = *(int*) vector_get(vector, index_a);
    int value_b = *(int*) vector_get(vector, index_b);

    vector_swap(vector, index_a, index_b);

    ck_assert_int_eq(value_a, *(int*) vector_get(vector, index_b));
    ck_assert_int_eq(value_b, *(int*) vector_get(vector, index_a));
}
END_TEST


START_TEST (test_vector_part_copy)
{
    const size_t capacity = vector_capacity(vector);
    const int number = 0xaabbccdd;
    for (size_t i = 0; i < capacity; ++i)
    {
        vector_set(vector, i, &number);
    }

    size_t part_offset = 3; /* in bytes inside element */
    size_t part_length = 1;

    char aa_buf[10];

    vector_part_copy(vector, aa_buf, 0, capacity, part_offset, part_length);

    for (size_t i = 0; i < sizeof(aa_buf); ++i)
    {
        ck_assert_int_eq((char)0xaa, aa_buf[i]);
    }

    char bbcc_buf[20];
    part_offset = 1;
    part_length = 2;

    vector_part_copy(vector, bbcc_buf, 0, 10, part_offset, part_length);

    for (size_t i = 0; i < sizeof(bbcc_buf); i += 2)
    {
        ck_assert_int_eq(*(short*)&bbcc_buf[i], (short)0xbbcc);
    }
}
END_TEST


static bool greater_then(const void *const element, void *const param)
{
    const int *elem = (int*)element;
    const int *value = (int*)param;
    return *elem > *value;
}

typedef struct
{
    int min, max;
}
in_range_t;

static bool in_range_excl(const void *const element, void *const param)
{
    const int *el = element;
    in_range_t *in_range = param;

    return in_range->min < *el && *el < in_range->max;
}

START_TEST (test_vector_linear_find)
{
    const size_t capacity = vector_capacity(vector);
    const int data[] = {45, 20, -33, 91, 63, 9, 500, 1, 0, 7};
    memcpy(vector_get(vector, 0), data, sizeof(int) * capacity);

    void *found = vector_linear_find(vector, capacity, greater_then, TMP_REF(int, 90));
    ck_assert_int_eq(*(int*)found, 91);

    found = vector_linear_find(vector, capacity, greater_then, TMP_REF(int, 500));
    ck_assert_ptr_null(found);

    found = vector_linear_find(vector, capacity, in_range_excl, &(in_range_t){.min = 45, .max = 91});
    ck_assert_int_eq(*(int*)found, 63);
}
END_TEST


static ssize_t cmp_int_asc(const void *value, const void *element, void *param)
{
    (void) param;
    return *(int*) value - *(int*) element;
}


START_TEST (test_vector_binary_find)
{
    const size_t capacity = vector_capacity(vector);
    const int data[] = {-100, -1, 0, 10, 12, 20, 21, 30, 34, 60};
    memcpy(vector_get(vector, 0), data, sizeof(int) * capacity);

    for (size_t i = 0; i < capacity; ++i)
    {
        void *element = vector_get(vector, i);
        void *found = vector_binary_find(vector, element, capacity, cmp_int_asc, NULL);

        ck_assert(found);
        ck_assert_int_eq(*(int*)element, *(int*)found);
        ck_assert_ptr_eq(element, found);
    }
}
END_TEST


START_TEST (test_vector_binary_find_lex)
{
    const size_t words = 5;
    const size_t word_len = 3;

    const char* data[] = {"aaa", "abc", "abd", "acz", "xyz"};
    for (size_t i = 0; i < words; ++i)
    {
        vector_set(vector, i, data[i]);
    }

    for (size_t i = 0; i < words; ++i)
    {
        void *element = vector_get(vector, i);
        void *found = vector_binary_find(vector, element, words, cmp_lex_asc, (void*)word_len);

        ck_assert(found);
        ck_assert_mem_eq(element, found, word_len);
        ck_assert_ptr_eq(element, found);
    }
}
END_TEST


START_TEST (test_vector_binary_find_insert_place)
{
    const size_t capacity = vector_capacity(vector);
    const int data[] = {-100, -1, 0, 10, 12, 20, 21, 30, 34, 60};
    memcpy(vector_get(vector, 0), data, sizeof(int) * capacity);
    
    size_t index = 0;

    index = vector_binary_find_insert_place(vector, TMP_REF(int, -200), capacity, cmp_int_asc, NULL);
    ck_assert_uint_eq(0, index);

    index = vector_binary_find_insert_place(vector, TMP_REF(int, -20), capacity, cmp_int_asc, NULL);
    ck_assert_uint_eq(1, index);

    index = vector_binary_find_insert_place(vector, TMP_REF(int, 0), capacity, cmp_int_asc, NULL);
    ck_assert_uint_eq(3, index); /* skips equal numbers according to current cmp implementation */

    index = vector_binary_find_insert_place(vector, TMP_REF(int, 1), capacity, cmp_int_asc, NULL);
    ck_assert_uint_eq(3, index);
}
END_TEST


START_TEST (test_vector_spread)
{
    const size_t capacity = vector_capacity(vector);
    random_fill(vector, capacity);

    const size_t spread_index = 5;
    const int value = 0x88aa88aa;
    vector_set(vector, spread_index, &value);

    vector_spread(vector, spread_index, 5);

    for (size_t i = spread_index; i < capacity; ++i)
    {
        ck_assert_mem_eq(&value, vector_get(vector, i), sizeof(int));
    }
}
END_TEST


Suite *vector_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Vector");
    
    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, setup_empty, teardown);
    tcase_add_test(tc_core, test_vector_create);
    tcase_add_test(tc_core, test_vector_get_set);
    tcase_add_test(tc_core, test_vector_set_zero);
    tcase_add_test(tc_core, test_vector_clone);
    tcase_add_test(tc_core, test_vector_truncate);
    tcase_add_test(tc_core, test_vector_copy);
    tcase_add_test(tc_core, test_vector_move);
    tcase_add_test(tc_core, test_vector_shift);
    tcase_add_test(tc_core, test_vector_spread);
    tcase_add_test(tc_core, test_vector_swap);
    tcase_add_test(tc_core, test_vector_part_copy);
    tcase_add_test(tc_core, test_vector_linear_find);
    tcase_add_test(tc_core, test_vector_binary_find);
    tcase_add_test(tc_core, test_vector_binary_find_lex);
    tcase_add_test(tc_core, test_vector_binary_find_insert_place);

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

