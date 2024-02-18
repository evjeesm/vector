#include "vector.h"

#include <assert.h> /* assert */
#include <stdio.h>  /* fprintf */
#include <stdlib.h> /* malloc, realloc, free */
#include <string.h> /* memcpy, memset */
#include <math.h>   /* pow, ceil */

#define VECTOR_HANDLE_ERROR(vector_pp, error) \
    (*vector_pp)->opts.error_handler.callback(vector_pp, error, (*vector_pp)->opts.error_handler.param)

/*
* Try to grow a vector and realloc if required.
* Reports an error if allocation failed.
*/
#define VECTOR_GROW(vector_pp, times) do{\
    if (!vector_grow(vector_pp, vector_has_to_grow(*vector_pp, times))) {\
        VECTOR_HANDLE_ERROR(vector_pp, VECTOR_GROW_ALLOC_ERROR); \
    }\
} while(0)

#define VECTOR_SHRINK(vector_pp, times) do{\
    if (!vector_shrink(vector_pp, vector_has_to_shrink(*vector_pp, times))) {\
        VECTOR_HANDLE_ERROR(vector_pp, VECTOR_SHRINK_ALLOC_ERROR); \
    }\
} while(0)

#define VECTOR_INC(vector_pp) VECTOR_GROW(vector_pp, 1)

#define VECTOR_DEC(vector_pp) VECTOR_SHRINK(vector_pp, 1)

struct vector_t
{
    vector_opts_t opts;
    size_t size;
    size_t capacity;
    char memory[];
};

static bool equal_bytes(const void *a, const void *b, void *param);
static size_t vector_has_to_grow(const vector_t *vector, size_t amount_to_add);
static size_t vector_has_to_shrink(const vector_t *vector, size_t amount_to_remove);
static bool vector_grow(vector_t **vector, size_t times);
static bool vector_shrink(vector_t **vector, size_t times);
static bool make_space_for_range(vector_t *vector, size_t index, size_t amount);
static void free_space_at(vector_t *vector, size_t index, size_t amount);
static size_t binary_find_insert_place(const vector_t *vector, compare_t cmp, const void *value, void *param, ssize_t start, ssize_t end);
static void *binary_find(const vector_t *vector, compare_t cmp, const void *value, void *param, ssize_t start, ssize_t end);
static void memswap(char *restrict a, char *restrict b, size_t size);
static bool vector_reverse_rec(vector_t **vector, size_t i, size_t len);
static bool has_enough_space_for(vector_t *vector, size_t amount);
static void default_error_callback(vector_t **vector, vector_error_t error, void *param);


vector_t *vector_create_(const vector_opts_t *opts)
{
    size_t next_cap = opts->initial_cap * opts->grow_factor;
    size_t grow_at = opts->initial_cap * opts->grow_threshold;
    size_t next_shrink_at = next_cap * opts->shrink_threshold;

    assert(next_shrink_at <= grow_at);

    vector_t *vec = (vector_t *) malloc(sizeof(vector_t) + opts->initial_cap * opts->esize);
    if (!vec)
    {
        VECTOR_HANDLE_ERROR(&vec, VECTOR_CREATE_ERROR);
        return NULL;
    }
    *vec = (vector_t){
        .opts = *opts,
        .capacity = opts->initial_cap,
    };
    if (!vec->opts.error_handler.callback)
    {
        vec->opts.error_handler.callback = default_error_callback;
    }
    return vec;
}


vector_t *vector_clone(const vector_t *vector)
{
    size_t alloc_size = sizeof(vector_t) + vector->capacity * vector->opts.esize;
    vector_t *clone = (vector_t *) malloc(alloc_size);
    if (!clone)
    {
        VECTOR_HANDLE_ERROR(&clone, VECTOR_CREATE_ERROR);
        return NULL;
    }
    memcpy(clone, vector, alloc_size);
    return clone;
}


void vector_copy(void *dest, const vector_t *vector, size_t offset, size_t length)
{
    assert(offset + length <= vector->size);
    memcpy(dest, vector_get(vector, offset), length * (vector->opts.esize));
}


void vector_destroy(vector_t *vector)
{
    free(vector);
}


void vector_clear(vector_t *vector)
{
    vector->size = 0;
}


size_t vector_size(const vector_t *vector)
{
    return vector->size;
}


size_t vector_capacity(const vector_t *vector)
{
    return vector->capacity;
}


bool vector_contains(const vector_t *vector, const void *value)
{
    return vector_find(vector, value, equal_bytes, (void*)vector->opts.esize);
}


void *vector_find(const vector_t *vector, const void *value, predicate_t predicate, void *param)
{
    for (size_t i = 0; i < vector->size; ++i)
    {
        void *element = vector_get(vector, i);
        if (predicate(value, element, param))
        {
            return element;
        }
    }
    return NULL;
}


void *vector_binary_find(const vector_t *vector, compare_t cmp, const void *value, void *param)
{
    if (0 == vector->size) return NULL;
    return binary_find(vector, cmp, value, param, 0, vector->size - 1);
}


void *vector_first(const vector_t *vector)
{
    return vector_get(vector, 0);
}


void *vector_last(const vector_t *vector)
{
    return vector_get(vector, vector->size - 1);
}


void *vector_get(const vector_t *vector, size_t index)
{
    if (vector->size == 0 || index >= vector->size) return NULL;
    const void *dest = vector->memory + index * vector->opts.esize;
    return (void*)dest;
}


bool vector_set(vector_t *vector, size_t index, const void *value)
{
    void *dest = vector_get(vector, index);
    if (dest)
    {
        if (value) memcpy(dest, value, vector->opts.esize);
        else memset(dest, 0, vector->opts.esize);
        return true;
    }
    return false;
}


bool vector_insert_at(vector_t **vector, size_t index, const void *value)
{
    VECTOR_INC(vector);
    if (!make_space_for_range(*vector, index, 1)) return false;
    (void) vector_set(*vector, index, value);
    return true;
}


bool vector_insert_fill(vector_t **vector, size_t index, size_t amount, const void *value)
{
    VECTOR_GROW(vector, amount);
    if (!make_space_for_range(*vector, index, amount)) return false;

    size_t elements_set = 1;
    (void) vector_set(*vector, index, value);
    
    /* copy elements exponentially through out the range */
    for (; (elements_set + elements_set) <= amount; elements_set += elements_set)
    {
        void *dest = vector_get(*vector, index + elements_set);
        vector_copy(dest, *vector, index, elements_set);
    }
    /* copy rest that left */
    for (; elements_set < amount; ++elements_set)
    {
        void *dest = vector_get(*vector, index + elements_set);
        vector_copy(dest, *vector, index + elements_set - 1, 1);
    }

    return true;
}


bool vector_binary_insert(vector_t **vector, compare_t cmp, const void *value, void *param, size_t *index)
{
    size_t size = vector_size(*vector);
    size_t place = (0 != size)
        ? binary_find_insert_place(*vector, cmp, value, param, 0, size - 1)
        : 0;
    if (index) *index = place;
    return vector_insert_at(vector, place, value);
}


bool vector_append_back(vector_t **vector, const void *value)
{
    VECTOR_INC(vector);

    size_t index = (*vector)->size;
    if (!make_space_for_range(*vector, index, 1)) return false;
    (void) vector_set(*vector, index, value);
    return true;
}


void vector_pop_back(vector_t **vector)
{
    size_t index = (*vector)->size - 1;
    (void) vector_set(*vector, index, NULL);
    free_space_at(*vector, index, 1);
    VECTOR_DEC(vector);
}


bool vector_append_front(vector_t **vector, const void *value)
{
    VECTOR_INC(vector);

    if (!make_space_for_range(*vector, 0, 1)) return false;
    (void) vector_set(*vector, 0, value);
    return true;
}


void vector_pop_front(vector_t **vector)
{
    free_space_at(*vector, 0, 1);
    VECTOR_DEC(vector);
}


void vector_remove(vector_t **vector, size_t index)
{
    free_space_at(*vector, index, 1);
    VECTOR_SHRINK(vector, 1);
}


void vector_remove_range(vector_t **vector, size_t index, size_t amount)
{
    free_space_at(*vector, index, amount);
    VECTOR_SHRINK(vector, amount);
}


bool vector_truncate(vector_t **vector, size_t capacity)
{
    capacity = (capacity < (*vector)->opts.initial_cap)
        ? (*vector)->opts.initial_cap
        : capacity;

    vector_t *vec = (vector_t*) realloc(*vector, sizeof(vector_t) + capacity * (*vector)->opts.esize);
    if (!vec) return false;

    vec->capacity = capacity;
    if (vec->size > capacity) vec->size = capacity;

    *vector = vec;
    return true;
}


void vector_swap(vector_t *vector, size_t index_a, size_t index_b)
{
    assert(index_a != index_b);
    assert(index_a < vector->size && index_b < vector->size);

    void *a = vector_get(vector, index_a);
    void *b = vector_get(vector, index_b);
    memswap(a, b, vector->opts.esize);
}


bool vector_move_range(vector_t **dest, size_t dest_idx, vector_t **src, size_t src_idx, size_t size)
{
    VECTOR_GROW(dest, size);
    if (!make_space_for_range(*dest, dest_idx, size)) return false;

    if (*dest == *src && src_idx > dest_idx) src_idx += size;
    vector_copy(vector_get(*dest, dest_idx), *src, src_idx, size);

    free_space_at(*src, src_idx, size);
    VECTOR_SHRINK(src, size);
    return true;
}


bool vector_swap_ranges(vector_t **vector, size_t idx_a, size_t len_a, size_t idx_b, size_t len_b)
{
    assert((idx_a + len_a <= idx_b || idx_b + len_b <= idx_a)
        && "ranges must not overlap.");

    assert(((idx_a + len_a <= (*vector)->size) && (idx_b + len_b <= (*vector)->size))
        && "ranges must be in bounds of vector.");

    assert((idx_a < idx_b) && "ranges must preserve ordering!");

    /* preallocate space for swap */
    if (!vector_truncate(vector, vector_capacity(*vector) + (len_a + len_b))) return false;

    /* duplicating range `b */
    (void) make_space_for_range(*vector, idx_a, len_b);

    idx_b += len_b; /* update index of range `b */
    vector_copy(vector_get(*vector, idx_a), *vector, idx_b, len_b);
    idx_a += len_b; /* update index of range `a */

    if (len_a > len_b) /* `b shorter then `a */
    {
        (void) make_space_for_range(*vector, idx_b, len_a - len_b); /* expand range `b */
    }
    else if (len_a < len_b)
    {
        free_space_at(*vector, idx_b, len_b - len_a); /* crop range `b */
    }

    vector_copy(vector_get(*vector, idx_b), *vector, idx_a, len_a);
    vector_remove_range(vector, idx_a, len_a);
    return true;
}


bool vector_reverse(vector_t **vector)
{
    vector_t *reversed = vector_clone(*vector);
    if (!reversed) return false;
    if (!vector_reverse_rec(&reversed, 0, vector_size(reversed)))
    {
        vector_destroy(reversed);
        return false;
    }
    *vector = reversed;
    return true;
}


static bool vector_reverse_rec(vector_t **vector, size_t i, size_t len)
{
    if (len == 1) return true;
    const size_t first = len / 2;
    const size_t second = len - first;

    if (!vector_swap_ranges(vector, i, first, i + first, second)) return false;

    return vector_reverse_rec(vector, i, second) && vector_reverse_rec(vector, i + second, first);
}


static bool equal_bytes(const void *a, const void *b, void *param)
{
    return 0 == memcmp(a, b, (size_t)param);
}


static size_t vector_has_to_grow(const vector_t *vector, size_t amount_to_add)
{
    assert(amount_to_add > 0);

    size_t times = 0;
    size_t size = vector->size + amount_to_add;
    double capacity = vector->capacity;
    size_t grow_at = capacity * vector->opts.grow_threshold;

    while (size >= grow_at)
    {
        ++times;
        capacity *= vector->opts.grow_factor;
        grow_at = capacity * vector->opts.grow_threshold;
    }

    return times;
}


static size_t vector_has_to_shrink(const vector_t *vector, size_t amount_to_remove)
{
    assert(amount_to_remove > 0);

    size_t times = 0;
    size_t size = vector->size - amount_to_remove;
    double capacity = vector->capacity;
    size_t shrink_at = capacity * vector->opts.shrink_threshold;

    while (size < shrink_at)
    {
        ++times;
        capacity /= vector->opts.grow_factor;
        shrink_at = capacity * vector->opts.shrink_threshold;
    }

    return times;
}


static bool vector_grow(vector_t **vector, size_t times)
{
    if (times == 0) return true;
    double new_cap = (*vector)->capacity * powf((*vector)->opts.grow_factor, times);
    return vector_truncate(vector, ceil(new_cap));
}


static bool vector_shrink(vector_t **vector, size_t times)
{
    if (times == 0) return true;
    double new_cap = (*vector)->capacity / powf((*vector)->opts.grow_factor, times);
    if (new_cap < (*vector)->opts.initial_cap) new_cap = (*vector)->opts.initial_cap;
    return vector_truncate(vector, ceil(new_cap));
}


static void free_space_at(vector_t *vector, size_t index, size_t amount)
{
    size_t size = vector_size(vector);

    if (index >= size) return; /* already free */

    size_t rest_from_index = size - index;

    if (amount >= rest_from_index)
    {
        vector->size -= rest_from_index;
    }
    else
    {
        size_t rest = rest_from_index - amount;
        char *to = vector->memory + vector->opts.esize * index;
        char *from = to + vector->opts.esize * amount;

        memmove(to, from, vector->opts.esize * rest);
        vector->size -= amount;
    }
}


static bool make_space_for_range(vector_t *vector, size_t index, size_t amount)
{
    if (!has_enough_space_for(vector, amount))
    {
        return false;
    }

    size_t size = vector_size(vector);

    if (index > size)
    {
        amount += index - size;
    }

    if (index < vector->size)
    {
        char *from = vector->memory + vector->opts.esize * index;
        char *to = from + vector->opts.esize * amount;

        memmove(to, from, vector->opts.esize * (vector->size - index));
    }
    vector->size += amount;
    return true;
}


static size_t binary_find_insert_place(const vector_t *vector, compare_t cmp, const void *value, void *param, ssize_t start, ssize_t end)
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


static void *binary_find(const vector_t *vector, compare_t cmp, const void *value, void *param, ssize_t start, ssize_t end)
{
    if (start > end)
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
        return binary_find(vector, cmp, value, param, middle + 1, end);
    }

    return binary_find(vector, cmp, value, param, start, middle - 1);
}


static void memswap(char *restrict a, char *restrict b, size_t size)
{
    union {
        size_t _word;
        char _bytes[sizeof(size_t)];
    }
    tmp;

    for (int i = 0; i < size / sizeof(tmp); ++i)
    {
        tmp._word = *(size_t*)a;
        *(size_t*)a = *(size_t*)b;
        *(size_t*)b = tmp._word;
        a += sizeof(tmp);
        b += sizeof(tmp);
    }
    for (int i = 0; i < size % sizeof(tmp); ++i, ++a, ++b)
    {
        tmp._bytes[0] = *a;
        *a = *b;
        *b = tmp._bytes[0];
    }
}

static bool has_enough_space_for(vector_t *vector, size_t amount)
{
    return (vector->capacity - vector->size) >= amount;
}


static void default_error_callback(vector_t **vector, vector_error_t error, void *param)
{
    (void) param;

    const char *operation;
    switch (error)
    {
        case VECTOR_CREATE_ERROR: operation = "VECTOR_CREATE"; break;
        case VECTOR_GROW_ALLOC_ERROR: operation = "VECTOR_GROW"; break;
        case VECTOR_SHRINK_ALLOC_ERROR: operation = "VECTOR_SHRINK"; break;
        default: operation = "UNKNOWN";
    }

    fprintf(stderr, "Allocation error occured.\nvector at (%p) on %s\nabort()\n", *vector, operation);
    abort();
}
