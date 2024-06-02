#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdbool.h> /* bool, true, false */
#include <stddef.h>  /* size_t */
#include <sys/types.h> /* ssize_t */

#define TMP_REF(type, value) (type[1]){value}

#define VECTOR_MANUAL_ERROR_HANDLER(error_out) \
    { .callback = vector_manual_error_callback, .param = error_out }

#define VECTOR_DEFAULT_ERROR_HANDLER \
    { .callback = vector_default_error_callback }

typedef enum vector_error_t
{
    VECTOR_NO_ERROR = 0,
    VECTOR_ALLOC_ERROR,
    VECTOR_ERROR_LAST
}
vector_error_t;

typedef struct vector_t vector_t;

/*
* Extends vector behavior on allocation error encounter.
*   error - enum value denoting error code.
*   param - user provided argument for error handler callback.
*/
typedef void (*vector_error_callback_t)(const vector_error_t error, void *const param);

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
typedef bool (*predicate_t) (const void *const element, void *const param);
typedef ssize_t (*compare_t) (const void *const value, const void *const element, void *const param);

/*
* The wrapper for `vector_create_` function that provides default values.
* Caller required to provide `element_size`!
*/
#define vector_create(vector_p, ...) {\
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Woverride-init\"") \
    vector_create_(&vector_p, &(vector_opts_t){ \
        .data_offset = 0, \
        .initial_cap = 10, \
        .error_handler = VECTOR_DEFAULT_ERROR_HANDLER, \
        __VA_ARGS__ \
    }); \
    _Pragma("GCC diagnostic pop") \
}

#define vector_create_manual_errhdl(vector_p, error_out, ...) \
    vector_create(vector_p, \
        .error_handler = VECTOR_MANUAL_ERROR_HANDLER(error_out), \
        __VA_ARGS__ \
    )

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
* Access data_offset property for extensions.
*/
size_t vector_data_offset(const vector_t *const vector);


/*
* Makes a copy of the whole vector.
* (Allocation may fail).
*/
vector_t *vector_clone(const vector_t *const vector);


/*
* Truncates vector to a desired capacity, wiping out elements beyond new capacity bounds.
* Takes third parameter which denotes error type.
*/
bool vector_truncate(vector_t **const vector, const size_t capacity, const vector_error_t error);


/*
* Copies range [offset, offset + length) elements into a destination pointer. Destination pointer can point to vector's buffer.
*/
void vector_copy(const vector_t *const vector, char *dest, const size_t offset, const size_t length);


/*
* Works as `vector_copy` but supports overlapping regions.
*/
void vector_move(const vector_t *const vector, char *dest, const size_t offset, const size_t length);


/*
* Partial copy of the elements in a range [offset, offset + length],
* where part of the element described by `part_offset` and `part_length` in bytes.
* All parts stored in a contiguous destination array one next to another.
*/
void vector_part_copy(const vector_t *const vector,
        char *dest,
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
void *vector_linear_find(const vector_t *const vector, const size_t limit, const predicate_t predicate, void *const param);


/*
* Binary search for a value in the vector with parametrized compare function.
* If no matching element found returns null pointer,
* otherwise pointer to a legit location in vector's memory span.
*/
void *vector_binary_find(const vector_t *const vector, const void *const value, const size_t limit, const compare_t cmp, void *param);


/*
* Binary search that returns index of the insert position for a new value
* that satisfies vector's ordering.
*/
size_t vector_binary_find_insert_place(const vector_t *const vector, const void *const value, const size_t limit, const compare_t cmp, void *param);


/*
* Lexicographic comparison in ascending order.
* param is used to pass element size by value (explicit cast to void* required)
*/
ssize_t cmp_lex_asc(const void *const value, const void *const element, void *const param);


/*
* Lexicographic comparison in descending order.
* param is used to pass element size by value (explicit cast to void* required)
*/
ssize_t cmp_lex_dsc(const void *const value, const void *const element, void *const param);


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
* Data will be overriden by shifted range.
*/
void vector_shift(vector_t *const vector, const size_t offset, const size_t length, const ssize_t shift);


/*
* Swaps values of elements designated by indicies.
*/
void vector_swap(vector_t *const vector, const size_t index_a, const size_t index_b);


/*
* Allocator functions:
*/
void *vector_alloc(const size_t alloc_size);

void *vector_realloc(void *ptr, const size_t alloc_size);

void vector_free(void *ptr);

/*
* Error callbacks:
*/
void vector_default_error_callback(const vector_error_t error, void *const param);

void vector_manual_error_callback(const vector_error_t error, void *const param);

#endif/*_VECTOR_H_*/
