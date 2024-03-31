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
#endif

#define TMP_REF(type, value) (type[1]){value}

typedef enum vector_error_t
{
    VECTOR_ALLOC_ERROR = 0,
    VECTOR_ERROR_LAST
}
vector_error_t;

typedef struct vector_t vector_t;

/*
* Extends vector behavior on allocation error encounter.
*   vector - location of the vector pointer.
*   param - user provided argument for error handler callback.
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
    size_t element_size; /* size of the element */
    size_t initial_cap;
    vector_error_handler_t error_handler;
}
vector_opts_t;

/*
* Callback types
*/
typedef bool (*predicate_t) (const void *const value, const void *const element, void *param);
typedef ssize_t (*compare_t) (const void *const value, const void *const element, void *param);

/*
* The wrapper for `vector_create_` function that provides default values.
*/
#define vector_create(vector_p, ...) {\
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Woverride-init\"") \
    vector_create_(&vector_p, &(vector_opts_t){ \
        .data_offset = 0, \
        .element_size = sizeof(int), \
        .initial_cap = 10, \
        __VA_ARGS__ \
    }); \
    _Pragma("GCC diagnostic pop") \
}

/*
* Vector constructor function that initializes vector
* with properties packed in opts struct.
* Space for `initial_cap` elements will be reserved.
* In case of allocation fail null pointer will be returned via `vector` argument.
*/
void vector_create_(vector_t **const vector, const vector_opts_t *const opts);


/*
* Deallocates vector. After the call pointer will be invalidated.
*/
void vector_destroy(vector_t *const vector);


/*
* Function returns pointer to reserved space after vector's control struct.
* Space for the header extension has to be preallocated on vector creation,
* size of this region is specified by `data_offset` property in `opts` struct.
*/
void* vector_get_ext_header(const vector_t *const vector);


/*
* Makes a copy of the whole vector.
* (Allocation may fail).
*/
vector_t *vector_clone(const vector_t *const vector);


/*
* Truncates vector to a desired capacity, wiping out elements beyond new capacity bounds.
*/
bool vector_truncate(vector_t **const vector, const size_t capacity);


/*
* Copies range [offset, offset + length) elements into a destination pointer.
*/
void vector_copy(char *dest, const vector_t *const vector, const size_t offset, const size_t length);


/*
* Partial copy of the elements in a range [offset, offset + length],
* where part of the element described by `part_offset` and `part_length` in bytes.
* All parts stored in a contiguous destination array one next to another.
*/
void vector_part_copy(char *dest,
        const vector_t *const vector,
        const size_t offset,
        const size_t length,
        const size_t part_offset,
        const size_t part_length);


/*
* Reports current element size.
*/
size_t vector_element_size(const vector_t *const vector);


/*
* Reports current capacity of the vector.
*/
size_t vector_capacity(const vector_t *const vector);


/*
* Reports initial capacity of the vector.
*/
size_t vector_initial_capacity(const vector_t *const vector);


/*
* Linear search for a value in the vector with parametrized predicate.
* If no matching element found returns null pointer,
* otherwise pointer to a legit location in vector's memory span.
*/
void *vector_linear_find(const vector_t *const vector, const void *const value, const size_t limit, const predicate_t predicate, void *param);


/*
* Binary search for a value in the vector with parametrized compare function.
* If no matching element found returns null pointer,
* otherwise pointer to a legit location in vector's memory span.
*/
void *vector_binary_find(const vector_t *const vector, const void *const value, const size_t limit, const compare_t cmp, void *param);


/*
* Returns pointer for the element at `index`.
*/
void *vector_get(const vector_t *const vector, const size_t index);


/*
* Sets element at given `index` to a `value`.
*/
void vector_set(vector_t *const vector, const size_t index, const void *const value);


/*
* Sets element at given `index` to a zero value.
*/
void vector_set_zero(vector_t *const vector, const size_t index);


/*
* Copies element's state at `index` across `amount` of elements including index.
*
*   | 0 | 1 | 2 | 3 |
*   |   | X |       | index = 1
*   |   | X | X | X | amount = 3
*
*/
void vector_spread(vector_t *const vector, const size_t index, const size_t amount);


/*
* Shifting length elements at `offset` by `shift` times in direction of a sign.
* TODO
*/
void vector_shift(vector_t *const vector, const size_t offset, const size_t length, const ssize_t shift);


/*
* Swaps values of elements designated by indicies.
*/
void vector_swap(vector_t *const vector, const size_t index_a, const size_t index_b);


#endif/*_VECTOR_H_*/
