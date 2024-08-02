#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdbool.h>    /* bool, true, false */
#include <stddef.h>     /* size_t */
#include <sys/types.h>  /* ssize_t */

/**
 * @brief Create temporary typed reference.
 *
 * Used to create temporary reference to an object,
 * that need to be passed via pointer.
 * Life time of the referenced object is bounded by a scope in which it was defined.
 */
#define TMP_REF(type, value) (type[1]){value}

/**
* @brief Vector control structure type
*
* Designed to be passed by reference.
*
* Functions that can cause reallocation of the vector
* must take a double pointer to a vector,
* this way vector pointer will be updated.
*
* @par Example of vector declaration:
* @code
* vector_t *vector;
* @endcode
*/
typedef struct vector_t vector_t;

/**
* @brief Vector options
*
* Parameters that are passed to a @ref vector_create_ function,
* they provide all information needed for vector creation.
*/
typedef struct vector_opts_t
{
    size_t data_offset;  /**< @copybrief vector_t::data_offset */
    size_t element_size; /**< @copybrief vector_t::element_size */
    size_t initial_cap;  /**< Amount of elements that will be preallocated on @ref vector_create_ "vector creation". */
    void *alloc_param;   /**< @copybrief vector_t::alloc_param */
}
vector_opts_t;

/**
* @brief Status enum that indicates errors of operations that may fail.
*
* This enum is designed to be extended by user enums for derived containes.
*/
typedef enum vector_status_t
{
    VECTOR_SUCCESS = 0, /**< Success operation status code, reserved. */
    VECTOR_ALLOC_ERROR, /**< Allocation error status code, reserved. */
    VECTOR_STATUS_LAST  /**< Indicates end of the enum values, can be used as next value in successor enum */
}
vector_status_t;

/** @addtogroup Callbacks
 *  @{
*   @brief callbacks that used by search functions 
*/

/**
* @brief Predicate, tells if traversed element matches user's criteria.
*
* @param[in] element   Points to an element inside a vector.
* @param[in] param     Additional parameter from user, if you don't need it, pass @c NULL.
*
* @returns             @c true - element matches, otherwise @c false
*/
typedef bool (*predicate_t) (const void *const element, void *const param);

/**
* @brief Compare, used to define traversal order.
*
* @see binary_find
*
* @param[in] value     Comparison reference value.
* @param[in] element   Points to an element inside a vector.
* @param[in] param     Additional parameter from user, if you don't need it, pass @c NULL.
*/
typedef ssize_t (*compare_t) (const void *const value, const void *const element, void *const param);

/*
* Any value other then zero breaks iteration over elements.
*/
typedef int (*foreach_t) (const void *const element, void *const param);
typedef int (*aggregate_t) (const void *const element, void *const acc, void *const param);
typedef int (*transform_t) (void *const element, void *const param);

/** @} */


/**
* A wrapper for a @ref vector_create_
* Provides default values.
* \param[in] element_size required!
*/
#define vector_create(...) \
    vector_create_( \
        &(vector_opts_t) { \
            .initial_cap = 10, \
            __VA_ARGS__ \
        }\
    )\


/**
* Vector constructor function that initializes vector
* with properties packed in opts struct.
* Space for @a initial_cap elements will be reserved.
* In case of allocation fail null pointer will be returned via `vector` argument.
*/
vector_t *vector_create_(const vector_opts_t *const opts);


/**
* Deallocates vector. A pointer will be invalidated after the call.
*/
void vector_destroy(vector_t *const vector);


/**
* Function returns pointer to reserved space after vector's control struct.
* Space for the header extension has to be preallocated on vector creation,
* size of this region is specified by `data_offset` property in `opts` struct.
*/
void* vector_get_ext_header(const vector_t *const vector);


/**
* Access data_offset property for extensions.
*/
size_t vector_data_offset(const vector_t *const vector);


/**
* Makes a copy of the whole vector.
* (Allocation may fail).
*/
vector_t *vector_clone(const vector_t *const vector);


/**
* Resizes vector to a desired capacity, wiping out elements beyond new capacity bounds.
* Takes third parameter which denotes error type that will be returned if resize fails.
*/
vector_status_t vector_resize(vector_t **const vector, const size_t capacity, const vector_status_t error);


/**
* Copies range [offset, offset + length) elements into a destination pointer. Destination pointer can point to vector's buffer.
*/
void vector_copy(const vector_t *const vector,
        char *dest,
        const size_t offset,
        const size_t length);


/**
* Works as `vector_copy` but supports overlapping regions.
*/
void vector_move(const vector_t *const vector,
        char *dest,
        const size_t offset,
        const size_t length);


/**
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


/**
* Reports current element size.
*/
size_t vector_element_size(const vector_t *const vector);


/**
* Reports current capacity of the vector.
*/
size_t vector_capacity(const vector_t *const vector);


/**
* Reports current capacity of the vector in bytes
*/
size_t vector_capacity_bytes(const vector_t *const vector);


/**
* Reports initial capacity of the vector.
*/
size_t vector_initial_capacity(const vector_t *const vector);


/**
* Reports initial capacity of the vector in bytes.
*/
size_t vector_initial_capacity_bytes(const vector_t *const vector);


/**
* Returns a pointer to the beginning of element data buffer.
* WARNING: Does not assert when capacity is zero.
*          Intended to be used for pointer arithmetics in derived containers.
*/
char *vector_data(const vector_t *const vector);


/**
* Linear search for a value in the vector with parametrized predicate.
* If no matching element found returns null pointer,
* otherwise pointer to a legit location in vector's memory span.
*/
void *vector_linear_find(const vector_t *const vector,
        const size_t limit,
        const predicate_t predicate,
        void *const param);


/**
* Binary search for a value in the vector with parametrized compare function.
* If no matching element found returns null pointer,
* otherwise pointer to a legit location in vector's memory span.
*/
void *vector_binary_find(const vector_t *const vector,
        const void *const value,
        const size_t limit,
        const compare_t cmp,
        void *const param);


/**
* Binary search, returns index of the element that matches or (-1) when not found.
*/
ssize_t vector_binary_find_index(const vector_t *const vector,
        const void *const value,
        const size_t limit,
        const compare_t cmp,
        void *const param);


/**
* Lexicographic comparison in ascending order.
* param is used to pass element size by value (explicit cast to void* required)
*/
ssize_t cmp_lex_asc(const void *const value, const void *const element, void *const param);


/**
* Lexicographic comparison in descending order.
* param is used to pass element size by value (explicit cast to void* required)
*/
ssize_t cmp_lex_dsc(const void *const value, const void *const element, void *const param);


/**
* Returns pointer for the element at `index`.
*/
void *vector_get(const vector_t *const vector, const size_t index);


/**
* Sets element at given `index` to a `value`.
*/
void vector_set(vector_t *const vector, const size_t index, const void *const value);


/**
* Sets element at given `index` to a zero value.
*/
void vector_set_zero(vector_t *const vector, const size_t index);


/**
* Copies element's state at `index` across `amount` of elements including index.
*
*   | 0 | 1 | 2 | 3 |
*   |   | X |       | index = 1
*   |   | X | X | X | amount = 3
*
*/
void vector_spread(vector_t *const vector, const size_t index, const size_t amount);


/**
* Shifting length elements at `offset` by `shift` times in direction of a sign.
* Data will be overriden by shifted range.
*/
void vector_shift(vector_t *const vector,
        const size_t offset,
        const size_t length,
        const ssize_t shift);


/**
* Swaps values of elements designated by indicies.
*/
void vector_swap(vector_t *const vector, const size_t index_a, const size_t index_b);


/**
* Run nonmodifying function on 'limit' elements of the vector.
* Return zero on success, or nonzero value - user defined status code.
*/
int vector_foreach(const vector_t *const vector,
        const size_t limit,
        const foreach_t func,
        void *const param);


/**
* Run nonmodifying function on 'limit' elements of the vector reducing them into `acc`.
* Return zero on success, or nonzero value - user defined status code.
*/
int vector_aggregate(const vector_t *const vector,
        const size_t limit,
        const aggregate_t func,
        void *const acc,
        void *const param);


/**
* Run modifying function on 'limit' elements of the vector.
* Return zero on success, or nonzero value - user defined status code.
*/
int vector_transform(vector_t *const vector,
        const size_t limit,
        const transform_t func,
        void *const param);


/** @addtogroup Allocation 
 * @{
 */
/**
 * @brief Allocates memory chunk of \a alloc_size.
 *
 * @param[in]     alloc_size  Allocation size in bytes
 * @param[in,out] param       Optional allocator parameter. @see vector_t::alloc_param
 *
 * @returns allocated memory chunk or @c NULL pointer on failure.
 */
void *vector_alloc(const size_t alloc_size, void *const param);

/**
 * @brief Reallocates already allocated memory chunk in order to change allocation size.
 *
 * > [!warning]
 * > This operation can move whole chunk to a completely different location.
 * > Make sure you update 
 *
 * @param[in]     ptr         Pointer to a memory chunk previously allocated with current allocator!
 * @param[in]     alloc_size  Allocation size in bytes
 * @param[in,out] param       Optional allocator parameter. @see vector_t::alloc_param
 *
 * @returns allocated memory chunk or @c NULL pointer on failure.
 */
void *vector_realloc(void *ptr, const size_t alloc_size, void *const param);

/**
* @brief Free allocation that was previously allocated.
*
* @param[in]     ptr         Pointer to a memory chunk previously allocated with current allocator!
* @param[in,out] param       Optional allocator parameter. @see vector_t::alloc_param
*/
void vector_free(void *ptr, void *const param);
/** @} */

/**
* Utility functions:
*/

/**
* Function calculates size of the element
* while respecting requirement for alignment.
*/
size_t calc_aligned_size(const size_t size, const size_t alignment);

#endif/*_VECTOR_H_*/
