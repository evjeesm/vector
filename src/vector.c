#include "vector.h"

#include <assert.h> /* assert */
#include <stdio.h>  /* fprintf */
#include <stdlib.h> /* malloc, realloc, free */
#include <string.h> /* memcpy, memset */

#define VECTOR_HANDLE_ERROR(vector_pp, error) \
    (*vector_pp)->opts.error_handler.callback(vector_pp, error, (*vector_pp)->opts.error_handler.param)

#ifndef NDEBUG
#define DEBUG(code) code
#else
#define DEBUG(code)
#endif

#define ASSERT_OVERFLOW(element_size, capacity, data_size, alloc_size, message) \
    assert((data_size / element_size == capacity && alloc_size > data_size) && message);

struct vector_t
{
    vector_opts_t opts;
    size_t capacity;
    char memory[];
};

/**                          ***
* === Forward Declarations === *
**                            */

static bool equal_bytes(const void *a, const void *b, void *param);
static void memswap(char *restrict a, char *restrict b, size_t size);
static void default_error_callback(const vector_t *const *const vector,
        const vector_error_t error, void *param);
static void *binary_find(const vector_t *const vector, const void *const value,
        const ssize_t start, const ssize_t end, const compare_t cmp, void *param);

/**                          ***
* === API Implementation   === *
**                            */

vector_t *vector_create_(const vector_opts_t *const opts)
{
    const size_t data_size = opts->element_size * opts->initial_cap;
    const size_t alloc_size = sizeof(vector_t) + opts->data_offset + data_size;

    vector_t *vec = NULL;

    ASSERT_OVERFLOW(opts->element_size, opts->initial_cap, data_size, alloc_size, "allocation size overflow!");

    vector_error_callback_t error_cbk = (opts->error_handler.callback
        ? opts->error_handler.callback
        : (const vector_error_callback_t) default_error_callback);

    vec = (vector_t *) malloc(alloc_size);
    if (!vec)
    {
        error_cbk(&vec, VECTOR_ALLOC_ERROR, opts->error_handler.param);
        return NULL;
    }

    *vec = (vector_t){
        .opts = *opts,
        .capacity = opts->initial_cap,
    };

    vec->opts.error_handler.callback = error_cbk;
    return vec;
}


void vector_destroy(vector_t *const vector)
{
    free(vector);
}


void* vector_get_ext_header(const vector_t *vector)
{
    assert((vector->opts.data_offset != 0) && "trying to access extended header that wasn't alloc'd");
    return (void*)vector->memory;
}


vector_t *vector_clone(const vector_t *const vector)
{
    const size_t alloc_size = sizeof(vector_t) + vector->opts.data_offset + vector->capacity * vector_element_size(vector);
    vector_t *clone = (vector_t *) malloc(alloc_size);
    if (!clone)
    {
        VECTOR_HANDLE_ERROR((vector_t **)&vector, VECTOR_ALLOC_ERROR);
        return NULL;
    }
    memcpy(clone, vector, alloc_size);
    return clone;
}


void vector_copy(void *const dest, const vector_t *const vector, const size_t offset, const size_t length)
{
    assert(offset + length <= vector_capacity(vector));
    memcpy(dest, vector_get(vector, offset), length * (vector_element_size(vector)));
}


size_t vector_element_size(const vector_t *const vector)
{
    return vector_element_size(vector);
}


size_t vector_capacity(const vector_t *vector)
{
    return vector->capacity;
}


void *vector_linear_find(const vector_t *const vector, const void *value, const size_t limit, const predicate_t predicate, void *param)
{
    for (size_t i = 0; i < limit; ++i)
    {
        void *element = vector_get(vector, i);
        if (predicate(value, element, param))
        {
            return element;
        }
    }
    return NULL;
}


void *vector_binary_find(const vector_t *const vector, const void *value, const size_t limit, const compare_t cmp, void *param)
{
    return binary_find(vector, value, 0, limit, cmp, param);
}


void *vector_get(const vector_t *const vector, const size_t index)
{
    assert(index < vector_capacity(vector));
    const void *dest = vector->memory + vector->opts.data_offset + index * vector_element_size(vector);
    return (void*)dest;
}


void vector_set(vector_t *const vector, const size_t index, const void *const value)
{
    void *dest = vector_get(vector, index);
    memcpy(dest, value, vector_element_size(vector));
}


void vector_set_zero(vector_t *const vector, const size_t index)
{
    void *dest = vector_get(vector, index);
    memset(dest, 0x00, vector_element_size(vector));
}


void vector_spread(vector_t *const vector, const size_t index, const size_t amount)
{
    assert((index < vector_capacity(vector)) && "index exceedes vector's capacity.");
    assert((index + amount <= vector_capacity(vector)) && "spread range exceeded vector's capacity.");

    size_t elements_set = 1;

    /* copy elements exponentially through out the range */
    for (; (elements_set + elements_set) <= amount; elements_set += elements_set)
    {
        void *dest = vector_get(vector, index + elements_set);
        vector_copy(dest, vector, index, elements_set);
    }
    /* copy rest that left */
    for (; elements_set < amount; ++elements_set)
    {
        void *dest = vector_get(vector, index + elements_set);
        vector_copy(dest, vector, index + elements_set - 1, 1);
    }
}


bool vector_truncate(vector_t **const vector, const size_t capacity)
{
    vector_opts_t *opts = &(*vector)->opts;
    size_t data_size = capacity * (*vector)->opts.element_size;
    size_t alloc_size = sizeof(vector_t) + (*vector)->opts.data_offset + data_size;

    ASSERT_OVERFLOW(opts->element_size, capacity, data_size, alloc_size, "allocation size overflow!");

    vector_t *vec = (vector_t*) realloc(*vector, alloc_size);
    if (!vec) return false;

    vec->capacity = capacity;
    *vector = vec;
    return true;
}


void vector_swap(vector_t *const vector, const size_t index_a, const size_t index_b)
{
    assert(index_a != index_b);
    assert(index_a < vector_capacity(vector) && index_b < vector_capacity(vector));

    void *a = vector_get(vector, index_a);
    void *b = vector_get(vector, index_b);
    memswap(a, b, vector_element_size(vector));
}


static bool equal_bytes(const void *a, const void *b, void *param)
{
    return 0 == memcmp(a, b, (size_t)param);
}


static size_t binary_find_insert_place(const vector_t *const vector,
        const void *value,
        const ssize_t start,
        const ssize_t end,
        const compare_t cmp,
        void *param)
{
    if (start == end)
    {
        return start;
    }

    const ssize_t middle = (start + end) / 2;
    const void *middle_value = vector_get(vector, middle);

    if (0 == cmp(value, middle_value, param))
    {
        return middle + 1;
    }

    if (0 < cmp(value, middle_value, param))
    {
        return binary_find_insert_place(vector, value, middle + 1, end, cmp, param);
    }

    return binary_find_insert_place(vector, value, start, middle, cmp, param);
}


static void *binary_find(const vector_t *const vector,
        const void *const value,
        const ssize_t start,
        const ssize_t end,
        const compare_t cmp,
        void *param)
{
    if (start == end)
    {
        return NULL;
    }

    const ssize_t middle = (start + end) / 2;
    void *middle_value = vector_get(vector, middle);

    if (0 == cmp(value, middle_value, param))
    {
        return middle_value;
    }

    if (0 < cmp(value, middle_value, param))
    {
        return binary_find(vector, value, middle + 1, end, cmp, param);
    }

    return binary_find(vector, value, start, middle, cmp, param);
}


static void memswap(char *restrict a, char *restrict b, const size_t size)
{
    union {
        size_t _word;
        char _bytes[sizeof(size_t)];
    }
    tmp;

    for (size_t i = 0; i < size / sizeof(tmp); ++i)
    {
        tmp._word = *(size_t*)a;
        *(size_t*)a = *(size_t*)b;
        *(size_t*)b = tmp._word;
        a += sizeof(tmp);
        b += sizeof(tmp);
    }
    for (size_t i = 0; i < size % sizeof(tmp); ++i, ++a, ++b)
    {
        tmp._bytes[0] = *a;
        *a = *b;
        *b = tmp._bytes[0];
    }
}


static void default_error_callback(const vector_t *const *const vector,
        const vector_error_t error,
        void *param)
{
    (void) param;

    const char *operation;
    switch (error)
    {
        case VECTOR_ALLOC_ERROR: operation = "VECTOR_ALLOC_ERROR"; break;
        default: operation = "UNKNOWN";
    }

    fprintf(stderr, "Allocation error occured.\nvector at (%p) on %s\nabort()\n", (void*)*vector, operation);
    abort();
}
