#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdbool.h> /* bool, true, false */
#include <stddef.h>  /* size_t */
#include <sys/types.h> /* ssize_t */

#ifdef MOCK_MALLOC
    #define malloc xmalloc
    #define realloc xrealloc
    #define free xfree
    extern void *xmalloc(size_t size);
    extern void *xrealloc(void *ptr, size_t size);
    extern void xfree(void *ptr);
    extern const size_t c_header_size;
#endif

#define TMP_REF(type, value) (type[1]){value}

typedef enum vector_error_t
{
    VECTOR_CREATE_ERROR,
    VECTOR_CLONE_ERROR,
    VECTOR_GROW_ALLOC_ERROR,
    VECTOR_SHRINK_ALLOC_ERROR,
    VECTOR_ERROR_LAST
}
vector_error_t;

typedef struct vector_t vector_t;

/*
* Extends vector behavior on allocation error encounter.
* @vector - location of the vector pointer.
* @param - user provided argument for error handler callback.
*/
typedef void (*vector_error_callback_t)(vector_t **vector, vector_error_t error, void *param);
typedef struct vector_error_handler_t
{
    vector_error_callback_t callback;
    void *param;
}
vector_error_handler_t;

typedef struct vector_opts_t
{
    size_t data_offset; /* beginning of the data array relative to `memory` field */
    size_t esize; /* size of the element */
    size_t initial_cap;
    float grow_factor;
    float grow_threshold;
    float shrink_threshold;
    vector_error_handler_t error_handler;
}
vector_opts_t;


/*
* Callback types
*/
typedef bool (*predicate_t) (const void *value, const void *element, void *param);
typedef ssize_t (*compare_t) (const void *value, const void *element, void *param);

/*
* The wrapper for `vector_create_` function that provides default values.
* Usecase:
*    vector_create(vec,
*       .esize = sizeof(float),
*       .initial_cap = 64,
*       .grow_factor = 2.f);
*/
#define vector_create(vec_ptr, ...) {\
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Woverride-init\"") \
    vec_ptr = vector_create_(&(vector_opts_t){ \
        .data_offset = 0, \
        .esize = sizeof(int), \
        .initial_cap = 10, \
        .shrink_threshold = 0.25f, \
        .grow_threshold = 0.75f, \
        .grow_factor = 1.5f, \
        __VA_ARGS__ \
    }); \
    _Pragma("GCC diagnostic pop") \
}

/*
* Vector constructor function that initializes vector
* with properties packed in opts struct.
* Space for `initial_cap` elements will be reserved.
* Vector will not be able to shrink below `initial_cap`.
* In case of allocation fail null pointer will be returned.
*/
vector_t *vector_create_(const vector_opts_t *opts);


/*
* Function returns pointer to reserved space after vector's control struct 
*/
void* vector_extended_header(const vector_t *vector);

/*
* Makes a copy of the whole vector.
* (Allocation may fail).
*/
vector_t *vector_clone(const vector_t *vector);


/*
* Copies range [offset, offset + length - 1] elements into a destination pointer.
*/
void vector_copy(void *dest, const vector_t *vector, size_t offset, size_t length);


/*
* Deallocates vector. After the call pointer will be invalidated.
*/
void vector_destroy(vector_t *vector);


/*
* Removes all of the elements of the vector. But not performs any grow/shrink operations.
*/
void vector_clear(vector_t *vector);


/*
* Reports current amount of elements containing within the vector.
*/
size_t vector_size(const vector_t *vector);

/*
* Reports current element size.
*/
size_t vector_element_size(const vector_t *vector);

/*
* Reports current capacity of the vector.
*/
size_t vector_capacity(const vector_t *vector);


/*
* Checks whenever the vector contains a value via byte to byte comparison.
*/
bool vector_contains(const vector_t *vector, const void *value);


/*
* Linear search for a value in the vector with parametrized predicate.
* If no matching element found returns null pointer,
* otherwise pointer to a legit location in vector's memory span.
*/
void *vector_find(const vector_t *vector, const void *value, predicate_t predicate, void *param);


/*
* Binary search for a value in the vector with parametrized compare function.
* If no matching element found returns null pointer,
* otherwise pointer to a legit location in vector's memory span.
*/
void *vector_binary_find(const vector_t *vector, compare_t cmp, const void *value, void *param);


/*
* Returns pointer for the first element in the vector or
* null if vector is empty.
*/
void *vector_first(const vector_t *vector);


/*
* Returns pointer for the last element in the vector or
* null if vector is empty.
*/
void *vector_last(const vector_t *vector);


/*
* Returns pointer for the element at `index` or
* null pointer if index is out of bounds.
*/
void *vector_get(const vector_t *vector, size_t index);


/*
* Sets element at given `index` to a `value`.
* If `value` is a null pointer, then element will be filled with zeroes.
* May fail if index is invalid.
*/
bool vector_set(vector_t *vector, size_t index, const void *value);


/*
* Inserts new element at given `index`, shifting one element forward
* from that index.
* (Allocation may fail, so returning operation status)
*/
bool vector_insert_at(vector_t **vector, size_t index, const void *value);


/*
* Fills vector range with `amount` copies of same value at `index`.
*/
bool vector_insert_fill(vector_t **vector, size_t index, size_t amount, const void *value);


/*
* Binary insert. Works only if previous elements
* that contained in the vector are ordered in same way.
* (Allocation may fail, so returning operation status)
*/
bool vector_binary_insert(vector_t **vector, compare_t cmp, const void *value, void *param, size_t *index);


/*
* Reserves a place for a single element using binary search.
* returns -1 in case of failure or positive index of the reserved place.
*/
ssize_t vector_binary_reserve(vector_t **vector, compare_t cmp, const void *value, void *param);


/*
* Appends element at vector's tail, growing vector on demand.
* Updates provided pointer to a new one when relocation happen.
* (Allocation may fail, so returning operation status)
*/
bool vector_append_back(vector_t **vector, const void *value);


/*
* Removes element at tail of the vector, shrinking vector on demand.
* May update provided vector pointer.
*/
void vector_pop_back(vector_t **vector);


/*
* Appends element at vector's head by shifting one element forward 
* and growing vector on demand.
* Updates provided pointer to a new one when relocation happen.
* (Allocation may fail, so returning operation status)
*/
bool vector_append_front(vector_t **vector, const void *value);


/*
* Removes element from vector's head by shifting one element backwards
* and shrinking vector on demand.
* May update provided vector pointer.
*/
void vector_pop_front(vector_t **vector);


/*
* Removes element at given `index`.
* May fail on realloc.
*/
void vector_remove(vector_t **vector, size_t index);


/*
* Removes range of elements of `amount` length at given `index`.
* May fail on realloc.
*/
void vector_remove_range(vector_t **vector, size_t index, size_t amount);


/*
* Truncates vector to a desired capacity, wiping out rest of the elements.
* Capacity's lower limit is `initial_cap`.
* (Allocation may fail, so returning operation status)
*/
bool vector_truncate(vector_t **vector, size_t capacity);


/*
* 
*/
void vector_swap(vector_t *vector, size_t index_a, size_t index_b);


/*
* 
*/
bool vector_swap_ranges(vector_t **vector, size_t idx_a, size_t len_a, size_t idx_b, size_t len_b);


/*
* Reverse order of elements in the vector 
*/
bool vector_reverse(vector_t **vector);

#endif/*_VECTOR_H_*/
