/**
* @file
* @author Evgeni Semenov
* @brief Implementation of the vector
*/

#include "vector.h"
#include "memswap.h"

#include <assert.h> /** assert */
#include <stdio.h>  /** fprintf */
#include <stdlib.h> /** malloc, realloc, free */
#include <string.h> /** memcpy, memset */

/**
 * @internal
 * @brief Assert for allocation size overflow detection.
 * Less likely to happen.
 */
#define ASSERT_OVERFLOW(element_size, capacity, data_size, alloc_size, message) \
    assert((data_size / element_size == capacity && alloc_size > data_size) && message);

struct vector_t
{
    size_t element_size;   /**< @brief Size of the underling element type. */
    size_t capacity;       /**< @brief Current amount of allocated elements. */
    size_t ext_header_size;/**< @brief Size of the extention header. */
    size_t allocator_size; /**< @brief Size of the allocator. */
    char memory[];
    /**< @brief Beginning of the vector's memory region.
    *    @details Must be offsetted by @ref vector_t::ext_header_size
    *             and allocator size(if present) to get to the elements.
    */
};

/*                             *
* === Forward Declarations === *
*                             */

/**
* @brief   Calculates allocation size for the vector.
*/
static size_t calculate_alloc_size (const size_t element_size,
        const size_t capacity,
        const size_t allocator_size,
        const size_t ext_header_size);

/**
* @brief   Access allocator region of the vector.
*/
static void *get_allocator(const vector_t *const vector);

/**
* @brief   Performs binary search on a vectors range.
*
* @returns pointer to the found element on success or NULL otherwise.
*/
static void *binary_find (const vector_t *const vector,
        const void *const value,
        const size_t start,
        const size_t end,
        const compare_t cmp,
        void *const param);

/**
* @brief Performs binary search on a vectors range.
*
* @returns index of the found element on success or -1 otherwise.
*/
static ssize_t binary_find_index (const vector_t *const vector,
        const void *const value,
        const size_t start,
        const size_t end,
        const compare_t cmp,
        void *param);


/*                             *
* === API Implementation   === *
*                             */

vector_t *vector_create_(const vector_opts_t *const opts)
{
    assert(opts);
    assert(opts->element_size);

    const size_t alloc_size = calculate_alloc_size(opts->element_size,
            opts->initial_cap,
            opts->alloc_opts.size,
            opts->ext_header_size);

    vector_t *vector = (vector_t *) vector_alloc(alloc_size, opts->alloc_opts.data);
    if (!vector)
    {
        return NULL;
    }

    (*vector) = (vector_t) {
        .element_size = opts->element_size,
        .capacity = opts->initial_cap,
        .ext_header_size = opts->ext_header_size,
        .allocator_size = opts->alloc_opts.size,
    };

    /* copy allocator struct */
    memcpy(get_allocator(vector), opts->alloc_opts.data, opts->alloc_opts.size);

    return vector;
}


void vector_destroy(vector_t *const vector)
{
    assert(vector);
    vector_free(vector, vector->memory);
}


vector_t *vector_clone(const vector_t *const vector)
{
    assert(vector);

    const size_t alloc_size = calculate_alloc_size(vector->element_size,
            vector->capacity,
            vector->allocator_size,
            vector->ext_header_size);

    // inheriting original vectors allocation method
    vector_t *clone = (vector_t *) vector_alloc(alloc_size, get_allocator(vector));
    if (!clone)
    {
        return NULL;
    }

    memcpy(clone, vector, alloc_size);

    return clone;
}


vector_status_t vector_resize(vector_t **const vector, const size_t capacity, const vector_status_t error)
{
    assert(vector && *vector);

    const size_t alloc_size = calculate_alloc_size((*vector)->element_size, 
            capacity,
            (*vector)->allocator_size,
            (*vector)->ext_header_size);

    vector_t *vec = (vector_t*) vector_realloc(*vector, alloc_size, get_allocator(*vector));
    if (!vec)
    {
        return error;
    }

    vec->capacity = capacity;
    *vector = vec;
    return VECTOR_SUCCESS;
}


void* vector_get_ext_header(const vector_t *const vector)
{
    assert(vector);
    assert((vector->ext_header_size != 0) && "trying to access extended header that wasn't alloc'd");
    return (void*)vector->memory + vector->allocator_size;
}


size_t vector_data_offset(const vector_t *const vector)
{
    assert(vector);
    return vector->ext_header_size + vector->allocator_size;
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
    assert((offset + length <= vector->capacity) && "`offset + length` exceeds vector's capacity!");

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


alloc_opts_t vector_alloc_opts(const vector_t *const vector)
{
    assert(vector);
    return (alloc_opts_t) {
        .size = vector->allocator_size,
        .data = vector->allocator_size ? get_allocator(vector) : NULL,
    };
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


size_t vector_capacity_bytes(const vector_t *const vector)
{
    assert(vector);
    return vector->capacity * vector->element_size;
}


void *vector_linear_find(const vector_t *const vector, const size_t limit, const predicate_t predicate, void *param)
{
    assert(vector);
    assert(predicate);

    assert((limit <= vector->capacity) && "Vector out of capacity bounds!");

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


void *vector_binary_find(const vector_t *const vector,
        const void *const value,
        const size_t limit,
        const compare_t cmp,
        void *param)
{
    assert(vector);
    assert(value);
    assert(cmp);

    assert((limit <= vector->capacity) && "Limit out of capacity bounds!");
    return binary_find(vector, value, 0, limit, cmp, param);
}


ssize_t vector_binary_find_index(const vector_t *const vector,
        const void *const value,
        const size_t limit,
        const compare_t cmp,
        void *const param)
{
    assert(vector);
    assert(value);
    assert(cmp);

    assert((limit <= vector->capacity) && "Limit out of capacity bounds!");
    return binary_find_index(vector, value, 0, limit, cmp, param);
}


char *vector_data(const vector_t *const vector)
{
    assert(vector);
    return (char*) vector->memory + vector_data_offset(vector);
}


void *vector_get(const vector_t *const vector, const size_t index)
{
    assert(vector);
    assert((index < vector->capacity) && "Index out of capacity bounds!");

    return (void*) (vector->memory + vector_data_offset(vector) + index * vector->element_size);
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


void vector_swap(vector_t *const vector, const size_t index_a, const size_t index_b)
{
    assert(vector);
    assert(index_a != index_b);
    assert(index_a < vector->capacity && index_b < vector->capacity);

    void *a = vector_get(vector, index_a);
    void *b = vector_get(vector, index_b);
    memswap(a, b, vector->element_size);
}


int vector_foreach(const vector_t *const vector, const size_t limit, const foreach_t func, void *const param)
{
    assert(vector);
    assert(limit && limit <= vector->capacity);
    assert(func);

    for (size_t i = 0; i < limit; ++i)
    {
        int status = func(vector_get(vector, i), param);
        if (status) return status;
    }

    return 0;
}


int vector_aggregate(const vector_t *const vector, const size_t limit, const aggregate_t func, void *const acc, void *const param)
{
    assert(vector);
    assert(limit && limit <= vector->capacity);
    assert(func);

    for (size_t i = 0; i < limit; ++i)
    {
        int status = func(vector_get(vector, i), acc, param);
        if (status) return status;
    }

    return 0;
}


int vector_transform(vector_t *const vector, const size_t limit, const transform_t func, void *const param)
{
    assert(vector);
    assert(limit && limit <= vector->capacity);
    assert(func);

    return vector_foreach(vector, limit, (foreach_t)func, param);
}


void * __attribute__((weak)) vector_alloc(const size_t alloc_size, void *const param)
{
    (void)param;
    return malloc(alloc_size);
}


void * __attribute__((weak)) vector_realloc(void *ptr, const size_t alloc_size, void *const param)
{
    (void)param;
    return realloc(ptr, alloc_size);
}


void __attribute__((weak)) vector_free(void *ptr, void *const param)
{
    (void)param;
    free(ptr);
}


size_t calc_aligned_size(const size_t size, const size_t alignment)
{
    return (size + alignment - 1) / alignment * alignment;
}


ssize_t cmp_lex_asc(const void *value, const void *element, void *param)
{
    return memcmp(value, element, (size_t)param);
}


ssize_t cmp_lex_dsc(const void *value, const void *element, void *param)
{
    return memcmp(element, value, (size_t)param);
}


/*                        **
* === Static Functions === *
*                         */

static size_t calculate_alloc_size(const size_t element_size,
        const size_t capacity,
        const size_t allocator_size,
        const size_t ext_header_size)
{
    const size_t data_size = element_size * capacity;
    const size_t alloc_size = sizeof(vector_t) + allocator_size + ext_header_size + data_size;
    ASSERT_OVERFLOW(element_size, capacity, data_size, alloc_size, "allocation size overflow!");
    return alloc_size;
}


static void *get_allocator(const vector_t *const vector)
{
    // assert((vector->allocator_size) && "No allocator region!");
    return (char*)vector->memory;
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


static ssize_t binary_find_index(const vector_t *const vector,
        const void *const value,
        const size_t start,
        const size_t end,
        const compare_t cmp,
        void *param)
{
    if (start == end)
    {
        return -1;
    }

    const size_t middle = (start + end) / 2;
    void *middle_value = vector_get(vector, middle);

    if (0 == cmp(value, middle_value, param))
    {
        return middle;
    }

    if (0 < cmp(value, middle_value, param))
    {
        return binary_find_index(vector, value, middle + 1, end, cmp, param);
    }

    return binary_find_index(vector, value, start, middle, cmp, param);
}
