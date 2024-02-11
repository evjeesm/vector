#include "vector.h"

#include <assert.h> /* assert */
#include <stdlib.h> /* malloc, realloc, free */
#include <string.h> /* memcpy, memset */
#include <math.h>   /* pow, ceil */

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
static bool make_space_for_range(vector_t **vector, size_t index, size_t amount);
static bool free_space_at(vector_t **vector, size_t index, size_t amount);
static size_t binary_find_insert_place(const vector_t *vector, compare_t cmp, const void *value, void *param, ssize_t start, ssize_t end);
static void *binary_find(const vector_t *vector, compare_t cmp, const void *value, void *param, ssize_t start, ssize_t end);


vector_t *vector_create_(const vector_opts_t *opts)
{
    size_t next_cap = opts->initial_cap * opts->grow_factor;
    size_t grow_at = opts->initial_cap * opts->grow_threshold;
    size_t next_shrink_at = next_cap * opts->shrink_threshold;

    assert(next_shrink_at <= grow_at);

    vector_t *vec = (vector_t *) malloc(sizeof(vector_t) + opts->initial_cap * opts->esize);
    if (!vec) return NULL;
    *vec = (vector_t){
        .opts = *opts,
        .capacity = opts->initial_cap,
    };
    return vec;
}


vector_t *vector_clone(const vector_t *vector)
{
    size_t alloc_size = sizeof(vector_t) + vector->capacity * vector->opts.esize;
    vector_t *clone = (vector_t *) malloc(alloc_size);
    if (!clone) return NULL;
    memcpy(clone, vector, alloc_size);
    return clone;
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
    if (vector->size == 0) return NULL;
    if (index >= vector->size) return NULL;
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
    if (!make_space_for_range(vector, index, 1)) return false;
    (void)vector_set(*vector, index, value);
    return true;
}


bool vector_insert_fill(vector_t **vector, size_t index, size_t amount, const void *value)
{
    if (!make_space_for_range(vector, index, amount)) return false;

    size_t elements_set = 1;
    (void)vector_set(*vector, index, value);
    
    /* copy elements exponentially through out the range */
    for (; (elements_set + elements_set) <= amount; elements_set += elements_set)
    {
        memcpy(vector_get(*vector, index + elements_set), vector_get(*vector, index), (*vector)->opts.esize * elements_set);
    }
    /* copy rest that left */
    for (; elements_set < amount; ++elements_set)
    {
        memcpy(vector_get(*vector, index + elements_set), vector_get(*vector, index + elements_set - 1), (*vector)->opts.esize);
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
    size_t index = (*vector)->size;
    if (!make_space_for_range(vector, index, 1)) return false;
    (void)vector_set(*vector, index, value);
    return true;
}


bool vector_pop_back(vector_t **vector)
{
    size_t index = (*vector)->size - 1;
    void *dest = (*vector)->memory + index * (*vector)->opts.esize;
    memset(dest, 0, (*vector)->opts.esize);
    return free_space_at(vector, index, 1);
}


bool vector_append_front(vector_t **vector, const void *value)
{
    if (!make_space_for_range(vector, 0, 1)) return false;
    (void)vector_set(*vector, 0, value);
    return true;
}


bool vector_pop_front(vector_t **vector)
{
    return free_space_at(vector, 0, 1);
}


bool vector_remove(vector_t **vector, size_t index)
{
    return free_space_at(vector, index, 1);
}


bool vector_remove_range(vector_t **vector, size_t index, size_t amount)
{
    return free_space_at(vector, index, amount);
}


bool vector_truncate(vector_t **vector, size_t capacity)
{
    capacity = (capacity < (*vector)->opts.initial_cap) ? (*vector)->opts.initial_cap : capacity;

    vector_t *vec = (vector_t*) realloc(*vector, sizeof(vector_t) + capacity * (*vector)->opts.esize);
    if (!vec) return false;

    vec->capacity = capacity;
    if (vec->size > capacity) vec->size = capacity;

    *vector = vec;
    return true;
}


static void *memxor(void *restrict dest, const void *restrict src, size_t n)
{
    char const *s = src;
    char *d = dest;

    for (; n > 0; n--)
        *d++ ^= *s++;

    return dest;
}

void vector_swap(vector_t *vector, size_t index_a, size_t index_b)
{
    assert(index_a != index_b);
    assert(index_a < vector->size && index_b < vector->size);

    void *a = vector_get(vector, index_a);
    void *b = vector_get(vector, index_b);

    memxor(a, b, vector->opts.esize);
    memxor(b, a, vector->opts.esize);
    memxor(a, b, vector->opts.esize);
}


bool vector_swap_ranges(vector_t **vector, size_t idx_a, size_t len_a, size_t idx_b, size_t len_b)
{
    assert((idx_a + len_a <= idx_b || idx_b + len_b <= idx_a)
        && "ranges must not overlap.");

    assert(((idx_a + len_a <= (*vector)->size) && (idx_b + len_b <= (*vector)->size))
        && "ranges must be in bounds of vector.");

    assert((idx_a < idx_b) && "ranges must preserve ordering!");
    
    /* duplicating range `b */
    if (!make_space_for_range(vector, idx_a, len_b))
    {
        return false;
    }
    idx_b += len_b; /* update index of range `b */
    memcpy(vector_get(*vector, idx_a), vector_get(*vector, idx_b), (*vector)->opts.esize * len_b);

    idx_a += len_b; /* update index of range `a */

    ssize_t dlt = len_a - len_b;
    if (dlt > 0) /* `b shorter then `a */
    {
        if (!make_space_for_range(vector, idx_b, dlt)) /* expand range `b */
        {
            return false;
        }
    }
    else if (!free_space_at(vector, idx_b, -dlt)) /* crop range `b*/
    {
        return false;
    }

    memcpy(vector_get(*vector, idx_b), vector_get(*vector, idx_a), (*vector)->opts.esize * len_a);
    return vector_remove_range(vector, idx_a, len_a);
}

//  [1, 2, 3| 4, 5, 6, 7]
//  [4, 5, 6, 7| 1, 2, 3]
//  [6, 7| 4, 5| 2, 3| 1]
//  [7| 6| 5| 4| 3, 2| 1]

void vector_reverse(vector_t **vector)
{
    const size_t size = vector_size(*vector);

    for (size_t group = 1; group < size / 2; group += group)
    {
        for (size_t i = 0; i < size; i += group)
        {
            size_t sec_range_len = (size % group == 0) ? group : (size % group);

            vector_swap_ranges(vector, i, group, i + group, sec_range_len);
        }
    }
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


static bool free_space_at(vector_t **vector, size_t index, size_t amount)
{
    size_t size = vector_size(*vector);

    if (index >= size) return true; /* already free */

    size_t rest_from_index = size - index;

    if (amount >= rest_from_index)
    {
        (*vector)->size -= rest_from_index;
    }
    else
    {
        size_t rest = rest_from_index - amount;
        char *to = (*vector)->memory + (*vector)->opts.esize * index;
        char *from = to + (*vector)->opts.esize * amount;

        memmove(to, from, (*vector)->opts.esize * rest);
        (*vector)->size -= amount;
    }

    return vector_shrink(vector, vector_has_to_shrink(*vector, amount));
}


static bool make_space_for_range(vector_t **vector, size_t index, size_t amount)
{
    size_t size = vector_size(*vector);
    if (index > size)
    {
        amount += index - size;
    }

    if (!vector_grow(vector, vector_has_to_grow(*vector, amount)))
    {
        return false;
    }
    
    if (index < (*vector)->size)
    {
        char *from = (*vector)->memory + (*vector)->opts.esize * index;
        char *to = from + (*vector)->opts.esize * amount;

        memmove(to, from, (*vector)->opts.esize * ((*vector)->size - index));
    }
    (*vector)->size += amount;

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
