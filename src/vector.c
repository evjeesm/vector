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
static void default_error_callback(vector_t **vector, vector_error_t error, void *param);
static void *binary_find(const vector_t *vector, const void *value, ssize_t start, ssize_t end, compare_t cmp, void *param);


/**                          ***
* === API Implementation   === *
**                            */

vector_t *vector_create_(const vector_opts_t *opts)
{
    const size_t data_size = opts->element_size * opts->initial_cap;
    const size_t alloc_size = sizeof(vector_t) + opts->data_offset + data_size;

    vector_t *vec = NULL;

    ASSERT_OVERFLOW(opts->element_size, opts->initial_cap, data_size, alloc_size, "allocation size overflow!");

    vector_error_callback_t error_cbk = (opts->error_handler.callback
        ? opts->error_handler.callback
        : default_error_callback);

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


void* vector_get_ext_header(const vector_t *vector)
{
    assert((vector->opts.data_offset != 0) && "trying to access extended header that wasn't alloc'd");
    return (void*)vector->memory;
}


vector_t *vector_clone(const vector_t *vector)
{
    const size_t alloc_size = sizeof(vector_t) + vector->opts.data_offset + vector->capacity * vector->opts.element_size;
    vector_t *clone = (vector_t *) malloc(alloc_size);
    if (!clone)
    {
        VECTOR_HANDLE_ERROR((vector_t **)&vector, VECTOR_ALLOC_ERROR);
        return NULL;
    }
    memcpy(clone, vector, alloc_size);
    return clone;
}


void vector_copy(void *dest, const vector_t *vector, size_t offset, size_t length)
{
    assert(offset + length <= vector->capacity);
    memcpy(dest, vector_get(vector, offset), length * (vector->opts.element_size));
}


void vector_destroy(vector_t *vector)
{
    free(vector);
}


size_t vector_element_size(const vector_t *vector)
{
    return vector->opts.element_size;
}


size_t vector_capacity(const vector_t *vector)
{
    return vector->capacity;
}


void *vector_linear_find(const vector_t *vector, const void *value, const size_t limit, predicate_t predicate, void *param)
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


void *vector_binary_find(const vector_t *vector, const void *value, const size_t limit, compare_t cmp, void *param)
{
    if (0 == limit) return NULL;
    return binary_find(vector, value, 0, limit, cmp, param);
}


void *vector_get(const vector_t *vector, size_t index)
{
    assert(index < vector_capacity(vector));
    const void *dest = vector->memory + vector->opts.data_offset + index * vector->opts.element_size;
    return (void*)dest;
}


void vector_set(vector_t *vector, size_t index, const void *value)
{
    void *dest = vector_get(vector, index);

    if (value) memcpy(dest, value, vector->opts.element_size);
    else memset(dest, 0, vector->opts.element_size);
}


void vector_spread(vector_t *vector, size_t index, size_t amount)
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


bool vector_truncate(vector_t **vector, size_t capacity)
{
    capacity = (capacity < (*vector)->opts.initial_cap)
        ? (*vector)->opts.initial_cap
        : capacity;

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


void vector_swap(vector_t *vector, size_t index_a, size_t index_b)
{
    assert(index_a != index_b);
    assert(index_a < vector_capacity(vector) && index_b < vector_capacity(vector));

    void *a = vector_get(vector, index_a);
    void *b = vector_get(vector, index_b);
    memswap(a, b, vector->opts.element_size);
}


static bool equal_bytes(const void *a, const void *b, void *param)
{
    return 0 == memcmp(a, b, (size_t)param);
}


static size_t binary_find_insert_place(const vector_t *vector, const void *value, compare_t cmp, void *param, ssize_t start, ssize_t end)
{
    if (start >= end)
    {
        if (0 < cmp(value, vector_get(vector, start), param))
        {
            return start + 1;
        }
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
        return binary_find_insert_place(vector, cmp, value, param, middle + 1, end);
    }

    return binary_find_insert_place(vector, cmp, value, param, start, middle - 1);
}


static void *binary_find(const vector_t *vector, const void *value, ssize_t start, ssize_t end, compare_t cmp, void *param)
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


static void memswap(char *restrict a, char *restrict b, size_t size)
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


static void default_error_callback(vector_t **vector, vector_error_t error, void *param)
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
