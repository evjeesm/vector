#include "vector.h"

#include <assert.h> /* assert */
#include <stdio.h>  /* fprintf */
#include <stdlib.h> /* malloc, realloc, free */
#include <string.h> /* memcpy, memset */

#ifndef NDEBUG
#define DEBUG(code) code
#else
#define DEBUG(code)
#endif

#define VECTOR_HANDLE_ERROR(vector_pp, error) \
    (*vector_pp)->error_handler.callback(error, (*vector_pp)->error_handler.param)

#define ASSERT_OVERFLOW(element_size, capacity, data_size, alloc_size, message) \
    assert((data_size / element_size == capacity && alloc_size > data_size) && message);

struct vector_t
{
    size_t data_offset;
    size_t element_size;
    size_t initial_cap;
    size_t capacity;
    vector_error_handler_t error_handler;
    char memory[];
};

/**                          ***
* === Forward Declarations === *
**                            */

static size_t calculate_alloc_size (const size_t element_size,
        const size_t capacity,
        const size_t data_offset);

static void memswap (char *restrict a,
        char *restrict b,
        const size_t size);

static void *binary_find (const vector_t *const vector,
        const void *const value,
        const size_t start,
        const size_t end,
        const compare_t cmp,
        void *const param);

static size_t binary_find_insert_place (const vector_t *const vector,
        const void *const value,
        const size_t start,
        const size_t end,
        const compare_t cmp,
        void *const param);


/**                          ***
* === API Implementation   === *
**                            */

void vector_create_(vector_t **const vector, const vector_opts_t *const opts)
{
    assert(vector);
    assert(opts);
    assert(opts->element_size);

    const size_t alloc_size = calculate_alloc_size(opts->element_size,
            opts->initial_cap,
            opts->data_offset);

    *vector = (vector_t *) vector_alloc(alloc_size);
    if (!*vector)
    {
        opts->error_handler.callback(VECTOR_ALLOC_ERROR, opts->error_handler.param);
        return;
    }

    (**vector) = (vector_t) {
        .data_offset = opts->data_offset,
        .element_size = opts->element_size,
        .initial_cap = opts->initial_cap,
        .capacity = opts->initial_cap,
        .error_handler = opts->error_handler
    };
}


void vector_destroy(vector_t *const vector)
{
    assert(vector);
    vector_free(vector);
}


void* vector_get_ext_header(const vector_t *const vector)
{
    assert(vector);
    assert((vector->data_offset != 0) && "trying to access extended header that wasn't alloc'd");
    return (void*)vector->memory;
}


size_t vector_data_offset(const vector_t *const vector)
{
    assert(vector);
    return vector->data_offset;
}


vector_t *vector_clone(const vector_t *const vector)
{
    assert(vector);

    const size_t alloc_size = calculate_alloc_size(vector->element_size, vector->capacity, vector->data_offset);
    vector_t *clone = (vector_t *) vector_alloc(alloc_size);
    if (!clone)
    {
        VECTOR_HANDLE_ERROR(&vector, VECTOR_ALLOC_ERROR);
        return NULL;
    }
    memcpy(clone, vector, alloc_size);
    return clone;
}


void vector_copy(const vector_t *const vector, char *const dest, const size_t offset, const size_t length)
{
    assert(dest);
    assert(vector);
    assert((offset + length <= vector->capacity) && "`offset + length` exceeds vector's capacity!");

    memcpy(dest, vector_get(vector, offset), length * (vector->element_size));
}


void vector_move(const vector_t *const vector, char *dest, const size_t offset, const size_t length)
{
    assert(dest);
    assert(vector);
    assert((offset + length <= vector_capacity(vector)) && "`offset + length` exceeds vector's capacity!");

    memmove(dest, vector_get(vector, offset), length * (vector->element_size));
}


void vector_part_copy(const vector_t *const vector,
        char *dest,
        const size_t offset,
        const size_t length,
        const size_t part_offset,
        const size_t part_length)
{
    for (size_t i = offset; i < length; ++i)
    {
        char *src = (char *)vector_get(vector, i) + part_offset;
        memcpy(dest, src, part_length);
        dest += part_length;
    }
}


size_t vector_element_size(const vector_t *const vector)
{
    assert(vector);
    return vector->element_size;
}


size_t vector_capacity(const vector_t *const vector)
{
    assert(vector);
    return vector->capacity;
}


size_t vector_initial_capacity(const vector_t *const vector)
{
    assert(vector);
    return vector->initial_cap;
}


void *vector_linear_find(const vector_t *const vector, const size_t limit, const predicate_t predicate, void *param)
{
    assert(vector);
    assert(predicate);

    assert((limit <= vector_capacity(vector)) && "Vector out of capacity bounds!");

    for (size_t i = 0; i < limit; ++i)
    {
        void *element = vector_get(vector, i);
        if (predicate(element, param))
        {
            return element;
        }
    }
    return NULL;
}


void *vector_binary_find(const vector_t *const vector, const void *const value, const size_t limit, const compare_t cmp, void *param)
{
    assert(vector);
    assert(value);
    assert(cmp);

    assert((limit <= vector->capacity) && "Limit out of capacity bounds!");
    return binary_find(vector, value, 0, limit, cmp, param);
}


size_t vector_binary_find_insert_place(const vector_t *const vector, const void *const value, const size_t limit, const compare_t cmp, void *param)
{
    assert(vector);
    assert(value);
    assert(cmp);

    assert((limit <= vector->capacity) && "Limit out of capacity bounds!");
    return binary_find_insert_place(vector, value, 0, limit, cmp, param);
}


ssize_t cmp_lex_asc(const void *value, const void *element, void *param)
{

    return memcmp(value, element, (size_t)param);
}


ssize_t cmp_lex_dsc(const void *value, const void *element, void *param)
{
    return memcmp(element, value, (size_t)param);
}


void *vector_get(const vector_t *const vector, const size_t index)
{
    assert(vector);
    assert((index < vector->capacity) && "Index out of capacity bounds!");

    return (void*) (vector->memory + vector->data_offset + index * vector->element_size);
}


void vector_set(vector_t *const vector, const size_t index, const void *const value)
{
    assert((index < vector->capacity) && "Index out of capacity bounds!");
    void *dest = vector_get(vector, index);
    memcpy(dest, value, vector->element_size);
}


void vector_set_zero(vector_t *const vector, const size_t index)
{
    assert((index < vector->capacity) && "Index out of capacity bounds!");
    void *dest = vector_get(vector, index);
    memset(dest, 0x00, vector->element_size);
}


void vector_spread(vector_t *const vector, const size_t index, const size_t amount)
{
    assert(vector);
    assert(amount > 1);
    assert((index < vector->capacity) && "Index out of capacity bounds!");
    assert((index + amount <= vector->capacity) && "`index + amount` exceedes vector's capacity.");

    size_t elements_set = 1;

    /* copy elements exponentially through out the range */
    for (; (elements_set + elements_set) <= amount; elements_set += elements_set)
    {
        void *dest = vector_get(vector, index + elements_set);
        vector_copy(vector, dest, index, elements_set);
    }
    /* copy rest that left */
    for (; elements_set < amount; ++elements_set)
    {
        void *dest = vector_get(vector, index + elements_set);
        vector_copy(vector, dest, index + elements_set - 1, 1);
    }
}


void vector_shift(vector_t *const vector, const size_t offset, const size_t length, const ssize_t shift)
{
    assert(vector);
    assert(shift != 0);
    assert((offset < vector->capacity) && "Offset out of bounds.");
    assert(((ssize_t)offset + shift >= 0) && "Shifted range underflows allocated buffer");
    assert((offset + shift + length <= vector->capacity) && "Shifted range exceedes capacity");

    vector_move(vector, vector_get(vector, offset + shift), offset, length);
}


bool vector_truncate(vector_t **const vector, const size_t capacity, const vector_error_t error)
{
    assert(vector && *vector);

    const size_t alloc_size = calculate_alloc_size((*vector)->element_size, capacity, (*vector)->data_offset);

    vector_t *vec = (vector_t*) vector_realloc(*vector, alloc_size);
    if (!vec)
    {
        VECTOR_HANDLE_ERROR(vector, error);
        return false;
    }

    vec->capacity = capacity;
    *vector = vec;
    return true;
}


void vector_swap(vector_t *const vector, const size_t index_a, const size_t index_b)
{
    assert(vector);
    assert(index_a != index_b);
    assert(index_a < vector->capacity && index_b < vector->capacity);

    void *a = vector_get(vector, index_a);
    void *b = vector_get(vector, index_b);
    memswap(a, b, vector->element_size);
}


__attribute__((weak)) void *vector_alloc(const size_t alloc_size)
{
    return malloc(alloc_size);
}


__attribute__((weak)) void *vector_realloc(void *ptr, const size_t alloc_size)
{
    return realloc(ptr, alloc_size);
}


__attribute__((weak)) void vector_free(void *ptr)
{
    free(ptr);
}


void vector_default_error_callback(const vector_error_t error, void *const param)
{
    (void) param;
    (void) error;

    fprintf(stderr, "Vector :: Allocation error occured! abort()\n");
    abort();
}


void vector_manual_error_callback(const vector_error_t error, void *const param)
{
    vector_error_t* error_out = param;
    *error_out = error;
}


/**                       **
* === Static Functions === *
**                        */

static size_t calculate_alloc_size(const size_t element_size, const size_t capacity, const size_t data_offset)
{
    const size_t data_size = element_size * capacity;
    const size_t alloc_size = sizeof(vector_t) + data_offset + data_size;
    ASSERT_OVERFLOW(element_size, capacity, data_size, alloc_size, "allocation size overflow!");
    return alloc_size;
}


static size_t binary_find_insert_place(const vector_t *const vector,
        const void *value,
        const size_t start,
        const size_t end,
        const compare_t cmp,
        void *param)
{
    if (start == end)
    {
        return start;
    }

    const size_t middle = (start + end) / 2;
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
        const size_t start,
        const size_t end,
        const compare_t cmp,
        void *param)
{
    if (start == end)
    {
        return NULL;
    }

    const size_t middle = (start + end) / 2;
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

