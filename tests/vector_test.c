#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../src/vector.h"

static vector_t *vector;

static void setup_empty(void)
{
    vector = vector_create(
       .element_size = sizeof(int),
       .initial_cap = 10
    );
    ck_assert_ptr_nonnull(vector);
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
}
END_TEST


START_TEST (test_vector_get_ext_header)
{
    vector_t *vector = vector_create(.ext_header_size = 16, .element_size = sizeof(int));
    char *header = vector_get_ext_header(vector);
    ck_assert_ptr_nonnull(header);
    vector_destroy(vector);
}
END_TEST


START_TEST (test_vector_ext_header_size)
{
    ck_assert_uint_eq(0, vector_ext_header_size(vector)); /* Zero case */

    /* Non-zero case */
    {
        const size_t ext_size = 16;
        vector_t *vector = vector_create(.ext_header_size = ext_size, .element_size = sizeof(int));
        ck_assert_uint_eq(ext_size, vector_ext_header_size(vector));
        vector_destroy(vector);
    }
}
END_TEST


START_TEST (test_vector_get_set)
{
    int array[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const int size = (int)(sizeof(array)/sizeof(array[0]));

    for (int i = 0; i < size; ++i)
    {
        vector_set(vector, i, &array[i]);
    }

    for (int i = 0; i < size; ++i)
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


START_TEST(test_vector_alloc_opts)
{
    int data = 42; // dummy data
    const size_t data_size = sizeof(data);

    vector_t *v = vector_create(
        .element_size = sizeof(int),
        .alloc_opts = alloc_opts(.data = &data, .size = data_size)
    );

    alloc_opts_t alloc_opts = vector_alloc_opts(v);

    ck_assert_int_eq(*(int*)(alloc_opts.data), data);
    ck_assert_uint_eq(alloc_opts.size, data_size);

    vector_destroy(v);
}
END_TEST


START_TEST(test_vector_alloc_opts_none)
{
    vector_t *v = vector_create(
        .element_size = sizeof(int),
        .alloc_opts = alloc_opts()
    );

    alloc_opts_t alloc_opts = vector_alloc_opts(v);

    ck_assert_ptr_null(alloc_opts.data);
    ck_assert_uint_eq(alloc_opts.size, 0);

    vector_destroy(v);
}
END_TEST


START_TEST(test_vector_capacity_bytes)
{
    size_t capacity_bytes = vector_capacity_bytes(vector);

    ck_assert_uint_eq(capacity_bytes, vector_capacity(vector) * vector_element_size(vector));
}
END_TEST


START_TEST(test_vector_data)
{
    void *data = vector_data(vector);
    void *first_element = vector_get(vector, 0);

    ck_assert_ptr_eq(data, first_element);
}
END_TEST


START_TEST(test_calc_aligned_size)
{
    const size_t alignment = 8;
    size_t aligned;

    aligned = calc_aligned_size(0, alignment);
    ck_assert_uint_eq(aligned, 0);

    aligned = calc_aligned_size(8, alignment);
    ck_assert_uint_eq(aligned, alignment);

    aligned = calc_aligned_size(15, alignment);
    ck_assert_uint_eq(aligned, alignment*2);
}
END_TEST


START_TEST (test_vector_resize)
{
    size_t expected_cap = 1024;
    const int expected_value = 999;

    ck_assert_uint_eq(VECTOR_SUCCESS, vector_resize(&vector, expected_cap, VECTOR_ALLOC_ERROR));
    ck_assert_uint_eq(vector_capacity(vector), expected_cap);

    /* set last element after truncation */
    vector_set(vector, expected_cap - 1, &expected_value);
    ck_assert_int_eq(*(int*) vector_get(vector, expected_cap - 1), expected_value);

    expected_cap = 0; /* truncate to zero is allowed */
    ck_assert_uint_eq(VECTOR_SUCCESS, vector_resize(&vector, expected_cap, VECTOR_ALLOC_ERROR));
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
    const int capacity = (int)vector_capacity(vector);

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
    const int capacity = (int)vector_capacity(vector);

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


START_TEST (test_vector_binary_find_none)
{
    const size_t capacity = vector_capacity(vector);
    const int data[] = {-100, -1, 0, 10, 12, 20, 21, 30, 34, 60};
    memcpy(vector_get(vector, 0), data, sizeof(int) * capacity);
    
    const int value = 33;
    void *element = vector_binary_find(vector, &value, capacity, cmp_int_asc, NULL);
    ck_assert_ptr_null(element);
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


START_TEST (test_vector_binary_find_lex_dsc)
{
    const size_t words = 5;
    const size_t word_len = 3;

    const char* data[] = {"aaa", "abc", "abd", "acz", "xyz"};
    for (size_t i = 0; i < words; ++i)
    {
        vector_set(vector, i, data[words - 1 - i]);
    }

    for (size_t i = 0; i < words; ++i)
    {
        void *element = vector_get(vector, i);
        void *found = vector_binary_find(vector, element, words, cmp_lex_dsc, (void*)word_len);

        ck_assert(found);
        ck_assert_mem_eq(element, found, word_len);
        ck_assert_ptr_eq(element, found);
    }
}
END_TEST


START_TEST (test_vector_binary_find_index)
{
    const size_t capacity = vector_capacity(vector);
    const int data[] = {-100, -1, 0, 10, 12, 20, 21, 30, 34, 60};
    memcpy(vector_get(vector, 0), data, sizeof(int) * capacity);

    for (size_t i = 0; i < capacity; ++i)
    {
        void *element = vector_get(vector, i);
        ssize_t index = vector_binary_find_index(vector, element, capacity, cmp_int_asc, NULL);

        ck_assert_uint_eq(index, i);
    }
}
END_TEST


START_TEST (test_vector_binary_find_index_none)
{
    const size_t capacity = vector_capacity(vector);
    const int data[] = {-100, -1, 0, 10, 12, 20, 21, 30, 34, 60};
    memcpy(vector_get(vector, 0), data, sizeof(int) * capacity);
    const int missing_element = 13;

    ssize_t index = vector_binary_find_index(vector, &missing_element, capacity, cmp_int_asc, NULL);

    ck_assert_uint_eq(-1, index);
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


#define MAXBUFSIZE 256
struct buf
{
    size_t size;
    char buf[MAXBUFSIZE];
};

int to_string(const void *const el, void *const param)
{
    struct buf *buf = param;
    buf->size += sprintf(buf->buf + buf->size, "%d, ", *(int*)el);
    return 0;
}

START_TEST (test_vector_foreach)
{
    /* fill vector */
    const int capacity = vector_capacity(vector);
    for (int i = 0; i < capacity; ++i)
    {
        vector_set(vector, i, &i);
    }

    struct buf buf = {0};
    int status = vector_foreach(vector, capacity, to_string, &buf);

    ck_assert(0 == status);
    ck_assert_uint_eq(30, buf.size);
    ck_assert_mem_eq(buf.buf, "0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ", buf.size);
}
END_TEST


int just_break(const void *const el, void *const param)
{
    (void) el;
    return *(int*)&param;
}


START_TEST (test_vector_foreach_break)
{
    const int capacity = vector_capacity(vector);
    const int expected_status = 1;

    int status = vector_foreach(vector, capacity, just_break, (void*)((size_t)expected_status));
    ck_assert_int_eq(expected_status, status);
}
END_TEST


int add_value(void *const el, void *const param)
{
    *(int*)el += *(int*)param;
    return 0;
}

START_TEST (test_vector_transform)
{
    /* fill vector */
    const int capacity = vector_capacity(vector);
    for (int i = 0; i < capacity; ++i)
    {
        vector_set(vector, i, &i);
    }

    int value = 5;
    int status = vector_transform(vector, capacity, add_value, &value);
    ck_assert(0 == status);

    for (int i = 0; i < capacity; ++i)
    {
        ck_assert_int_eq(i + value, *(int*) vector_get(vector, i));
    }
}


int sum(const void *const el, void *const acc, void *const param)
{
    (void) param;
    *(int*) acc += *(int*) el;
    return 0;
}

START_TEST (test_vector_aggregate)
{
    /* fill vector */
    const int capacity = vector_capacity(vector);
    for (int i = 0; i < capacity; ++i)
    {
        vector_set(vector, i, &i);
    }

    int total = 0;
    int status = vector_aggregate(vector, capacity, sum, &total, NULL);
    ck_assert(0 == status);
    ck_assert_int_eq(total, 45);
}


int sum_with_limit(const void *const el, void *const acc, void *const param)
{
    const int limit = *(int*)&param;
    const int *el_ = (int*) el;
    int *acc_ = acc;
    if (*acc_ >= limit) return 1; /* break */
    *acc_ += *el_;
    return 0;
}

START_TEST (test_vector_aggregate_break)
{
    /* fill vector */
    const int capacity = vector_capacity(vector);
    for (int i = 0; i < capacity; ++i)
    {
        vector_set(vector, i, &i);
    }
    
    const int limit = 9;
    const int expected_sum = 10;
    const int expected_status = 1;
    int total = 0;
    int status = vector_aggregate(vector, capacity, sum_with_limit, &total, (void*)(size_t)limit);

    ck_assert_int_eq(expected_status, status);
    ck_assert_int_eq(expected_sum, total);
}


Suite *vector_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Vector");
    
    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, setup_empty, teardown);
    tcase_add_test(tc_core, test_vector_create);
    tcase_add_test(tc_core, test_vector_get_ext_header);
    tcase_add_test(tc_core, test_vector_ext_header_size);
    tcase_add_test(tc_core, test_vector_get_set);
    tcase_add_test(tc_core, test_vector_set_zero);
    tcase_add_test(tc_core, test_vector_clone);
    tcase_add_test(tc_core, test_vector_alloc_opts);
    tcase_add_test(tc_core, test_vector_alloc_opts_none);
    tcase_add_test(tc_core, test_vector_capacity_bytes);
    tcase_add_test(tc_core, test_vector_data);
    tcase_add_test(tc_core, test_calc_aligned_size);
    tcase_add_test(tc_core, test_vector_resize);
    tcase_add_test(tc_core, test_vector_copy);
    tcase_add_test(tc_core, test_vector_move);
    tcase_add_test(tc_core, test_vector_shift);
    tcase_add_test(tc_core, test_vector_spread);
    tcase_add_test(tc_core, test_vector_swap);
    tcase_add_test(tc_core, test_vector_part_copy);
    tcase_add_test(tc_core, test_vector_linear_find);
    tcase_add_test(tc_core, test_vector_binary_find);
    tcase_add_test(tc_core, test_vector_binary_find_none);
    tcase_add_test(tc_core, test_vector_binary_find_lex);
    tcase_add_test(tc_core, test_vector_binary_find_lex_dsc);
    tcase_add_test(tc_core, test_vector_binary_find_index);
    tcase_add_test(tc_core, test_vector_binary_find_index_none);
    tcase_add_test(tc_core, test_vector_foreach);
    tcase_add_test(tc_core, test_vector_foreach_break);
    tcase_add_test(tc_core, test_vector_transform);
    tcase_add_test(tc_core, test_vector_aggregate);
    tcase_add_test(tc_core, test_vector_aggregate_break);

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

