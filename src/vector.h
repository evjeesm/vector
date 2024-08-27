/**
* @file
* @author Evgeni Semenov
* @brief Public interface of the vector
*/

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdbool.h>    /* bool, true, false */
#include <stddef.h>     /* size_t */
#include <sys/types.h>  /* ssize_t */

/**
* @brief   Create temporary typed reference.
* @details Used to create temporary reference to an object,
*          that need to be passed via pointer.
*          Life time of the referenced object is bounded by a scope in which it was defined.
*/
#define TMP_REF(type, value) (type[1]){value}

/**
* @brief   Vector control structure type
* @details Designed to be passed by reference.
*          Functions that can cause reallocation of the vector
*          must take a double pointer to a vector,
*          this way vector pointer will be updated.
*/
typedef struct vector_t vector_t;

/**
* @brief Allocator options.
*/
typedef struct alloc_opts_t
{
    size_t size;
    /**< @brief Size of the allocator data */

    void *data;
    /**< @brief User defined allocator structure.
     *   @details Will be copied into vector's memory region.
     */
}
alloc_opts_t;

/**
* @brief   Vector options.
* @details Parameters that are passed to a @ref vector_create_ function,
*          they provide all information needed for vector creation.
*/
typedef struct vector_opts_t
{
    /* required: */
    size_t element_size;      /**< @brief @copybrief vector_t::element_size */

    /* optional: */
    size_t initial_cap;       /**< @brief Amount of elements that will be preallocated. */
    size_t ext_header_size;   /**< @brief @copybrief vector_t::ext_header_size */
    alloc_opts_t *alloc_opts; /**< @brief optional allocator */
}
vector_opts_t;

/**
* @brief   Status enum that indicates errors of operations that may fail.
* @details This enum is designed to be extended by user enums for derived containes.
*/
typedef enum vector_status_t
{
    VECTOR_SUCCESS = 0, /**< Success operation status code, reserved. */
    VECTOR_ALLOC_ERROR, /**< Allocation error status code, reserved. */
    VECTOR_STATUS_LAST  /**< Indicates end of the enum values, can be used as next value in successor enum */
}
vector_status_t;

/**
 * @addtogroup Vector_API Vector API
 * @brief      Main vectors methods. @{ */

/**
* @addtogroup Callbacks
* @brief      Callbacks used by various API funcions. @{ */

/**
* @brief Predicate, tells if traversed element matches user's criteria.
*
* @param[in] element Points to an element inside a vector.
* @param[in] param   Additional parameter from user, if you don't need it, pass @c NULL.
*
* @returns           @c true - element matches, otherwise @c false
*/
typedef bool (*predicate_t) (const void *const element, void *const param);

/**
* @brief Compare, used to define traversal order.
* @see binary_find
*
* @param[in] value   Comparison reference value.
* @param[in] element Points to an element inside a vector.
* @param[in] param   Additional parameter from user, if you don't need it, pass @c NULL.
* @returns           Positive (value > element), negative (value < element) or zero (value == element).
*/
typedef ssize_t (*compare_t) (const void *const value, const void *const element, void *const param);

/**
* @brief Callback determines an operation for @ref vector_foreach.
*
* This operation will be performed on each element of the vector,
* until one of the calls retuned non-zero value.
*
* @param[in]      element Points to an element in a processed vector.
* @param[in,out]  param   User defined parameter that for scaling.
* @returns                User defined code, non-zero code results in loop break.
*/
typedef int (*foreach_t) (const void *const element, void *const param);

/**
* @brief   Callback determines an operation for @ref vector_aggregate.
* @details This operation will be performed on each element of the vector,
*          until one of the calls retuned non-zero value.
*          Reduces vector into a @c acc parameter, non-destructive operation.
*
* @param[in]      element Points to an element in a processed vector.
* @param[in,out]  acc     Accumulator that stores end result.
* @param[in,out]  param   User defined parameter that for scaling.
* @returns                User defined code, non-zero code results in loop break.
*/
typedef int (*aggregate_t) (const void *const element, void *const acc, void *const param);

/**
* @brief   Callback determines an operation for @ref vector_transform.
* @details This operation will be performed on each element of the vector,
*          until one of the calls retuned non-zero value.
*          Destructive operation, designed to mutate vector elements.
*
* @param[in]      element Points to an element in a processed vector.
* @param[in,out]  param   User defined parameter that for scaling.
* @returns                User defined code, non-zero code results in loop break.
*/
typedef int (*transform_t) (void *const element, void *const param);
/** @} */

/**
 * @addtogroup Lifetime
 * @brief      Constructors/Destructors @{ */

/** @brief Use this macro to define allocator opts in @ref vector_opts_t */
#define alloc_opts(...) &(alloc_opts_t){__VA_ARGS__}

/**
* @brief   Vector constructor.
* @details Preferable way to invoke vector constructor.
*          Provides default values.
* @warning @ref vector_opts_t::element_size "element_size" is mandatory!
* @see vector_create_
*/
#define vector_create(...) \
    vector_create_( \
        &(vector_opts_t) { \
            .initial_cap = 10, \
            __VA_ARGS__ \
        }\
    )\

/**
* @brief   Vector contructor.
* @details Vector constructor function that initializes vector
*          with properties packed in opts struct.
*          Space for @a initial_cap elements will be reserved.
*          In case of allocation fail null pointer will be returned via @c vector argument.
*
* @param[in] opts Options according to which vector will be created.
* @returns        Fresh new instance of the vector or @c NULL if allocation failed.
*/
vector_t *vector_create_(const vector_opts_t *const opts);


/**
* @brief   Deallocates vector.
*
* A pointer will be invalidated after the call.
*
* @param[in] vector Vector pointer that will be deallocated.
*/
void vector_destroy(vector_t *const vector);


/**
* @brief   Duplicates a vector.
* @details Makes an exact copy of the whole vector. (Allocation may fail).
*
* @param[in] vector Vector prototype to be copied.
* @returns          Copy of the vector on success, @c NULL pointer otherwise.
*/
vector_t *vector_clone(const vector_t *const vector);


/**
* @brief   Performs allocation resize.
* @details Resizes vector to a desired capacity, 
*          wiping out elements beyond new capacity bounds.
*          Takes third parameter which denotes error type
*          that will be returned if resize fails.
*
* @param[in] vector   Reference to vectors pointer.
* @param[in] capacity Desired vectors capacity.
* @param[in] error    Extension feature, error status code that will be returned upon allocation failure.
* @returns            Operation status.
*/
vector_status_t vector_resize(vector_t **const vector, const size_t capacity, const vector_status_t error);

/** @} @noop Lifetime */

/**
* @addtogroup Extension
* @brief Support for derived classes. @{ */

/**
* @brief   Provides a location where user can put a header for the derived class.
* @details Function returns a pointer to reserved space after vector's control struct.
*          Space for the header extension has to be preallocated on vector creation,
*          size of this region is specified by @ref vector_t::ext_header_size property in @ref vector_opts_t struct.
*
* @param[in] vector Pointer to vector.
* @returns Pointer to @ref vector_t::memory
*/
void* vector_get_ext_header(const vector_t *const vector);


/**
* @brief   Compute offset from vector_t::memory to first element.
*
* @param[in] vector Pointer to vector.
* @returns          Data offset in bytes
*/
size_t vector_data_offset(const vector_t *const vector);

/** @} @noop Extension */

/**
* @brief   Copy element range to other location.
* @details Copies range [offset, offset + length) elements into a destination pointer.
*          Destination pointer can point to vector's buffer.
*
* @param[in]  vector Pointer to vector instance.
* @param[out] dest   Destination pointer.
* @param[in]  offset Offset in @ref vector_t::element_size "elements" (begin index).
* @param[in]  length Size of the coping range in elements.
*/
void vector_copy(const vector_t *const vector,
        char *dest,
        const size_t offset,
        const size_t length);


/**
* @brief   Moves range of the vector elements to another location.
* @details Works as @ref vector_copy, but supports overlapping regions.
*
* @param[in]  vector Pointer to vector instance.
* @param[out] dest   Destination pointer.
* @param[in]  offset Offset in @ref vector_t::element_size "elements" (begin index).
* @param[in]  length Size of the coping range in elements.
*/
void vector_move(const vector_t *const vector,
        char *dest,
        const size_t offset,
        const size_t length);


/**
* @brief   Partial copying.
* @details Partial copy of the elements in a range [offset, offset + length],
*          where part of the element described by @c part_offset and @c part_length in bytes.
*          All parts stored in a contiguous destination array one next to another.
*
* @param[in]  vector     Pointer to vector instance.
* @param[out] dest       Destination pointer.
* @param[in]  offset     Offset in @ref vector_t::element_size "elements" (begin index).
* @param[in] length      Size of the coping range in elements.
* @param[in] part_offset Offset in bytes inside an element,
*                        begining of the portion to copy.
* @param[in] part_length Length of the copying portion in bytes.
*/
void vector_part_copy(const vector_t *const vector,
        char *dest,
        const size_t offset,
        const size_t length,
        const size_t part_offset,
        const size_t part_length);

/**
* @addtogroup Properties
* @brief   Access properties of a vector. @{ */

/**
* @brief   Access allocator options.
*
* @param[in] vector Pointer to a vector instance.
* @returns   pointer to an allocator and size.
*/
alloc_opts_t vector_alloc_opts(const vector_t *const vector);


/**
* @brief   Reports current element size.
*
* @param[in] vector Pointer to a vector instance.
* @returns          Element size in bytes.
*/
size_t vector_element_size(const vector_t *const vector);


/**
* @brief   Reports current capacity of the vector.
*
* @param[in] vector Pointer to a vector instance.
* @returns          Amount of elements currently allocated.
*/
size_t vector_capacity(const vector_t *const vector);


/**
* @brief Reports current capacity of the vector in bytes
*
* @param[in] vector Pointer to a vector instance.
* @returns          Amount of bytes currently allocated for elements.
*/
size_t vector_capacity_bytes(const vector_t *const vector);

/** @} @noop Properties */

/**
 * @addtogroup Searches
 * @brief Methods for querying the vector. @{ */

/**
* @brief   Simple linear search for unordered data.
* @details Linear search for a value in the vector with parametrized predicate.
*          If no matching element found returns null pointer,
*          otherwise pointer to a legit location in vector's memory span.
*
* @param[in] vector    Pointer to a vector instance.
* @param[in] limit     Iteration limit before break.
* @param[in] predicate Condition for desired element to be found.
* @param[in] param     User defined parameter, passed to @c predicate.
* @returns             Pointer to a found element.
*/
void *vector_linear_find(const vector_t *const vector,
        const size_t limit,
        const predicate_t predicate,
        void *const param);


/**
* @brief   Run binary search on the vector.
* @details @copydetails binary_find
*
* @param[in] vector Pointer to a vector instance.
* @param[in] value  Reference value to be compared to elements.
* @param[in] limit  Iteration limit before break.
* @param[in] cmp    Condition for desired element to be found.
* @param[in] param  User defined parameter, passed to @c predicate.
* @returns          Pointer to a found element.
*/
void *vector_binary_find(const vector_t *const vector,
        const void *const value,
        const size_t limit,
        const compare_t cmp,
        void *const param);


/**
* @brief   Run binary search on the vector.
* @details @copydetails binary_find_index
*
* @param[in] vector Pointer to a vector instance.
* @param[in] value  Reference value to be compared to elements.
* @param[in] limit  Iteration limit before break.
* @param[in] cmp    Condition for desired element to be found.
* @param[in] param  User defined parameter, passed to @c predicate.
* @returns          Index of found element or @c NULL if none.
*/
ssize_t vector_binary_find_index(const vector_t *const vector,
        const void *const value,
        const size_t limit,
        const compare_t cmp,
        void *const param);

/** @} @noop Searches */

/**
* @addtogroup Elements
* @brief Access and manipulate elements of a vector. @{ */


/**
* @brief   Gives a pointer to a location where elements' data begins.
* @warning Does not assert when capacity is zero.
*          Intended to be used for pointer arithmetics in derived containers.
*
* @param[in] vector Pointer to a vector instance.
* @returns          Location where elements are stored.
*/
char *vector_data(const vector_t *const vector);


/**
* @brief Returns pointer for the element at @c index.
*
* @param[in] vector Pointer to a vector instance.
* @param[in] index  Denotes an element to be overriden by @c value.
* @returns          A pointer to a vector element at @c index.
*/
void *vector_get(const vector_t *const vector, const size_t index);


/**
* @brief Sets element at given @c index to a @c value.
*
* @param[in] vector Pointer to a vector instance.
* @param[in] index  Denotes an element to be overriden by @c value.
* @param[in] value  Value to be stored at the @c index.
*/
void vector_set(vector_t *const vector, const size_t index, const void *const value);


/**
* @brief Sets element at given @c index to a zero value.
*
* @param[in] vector Pointer to a vector instance.
* @param[in] index  Denotes an element to be zeroed.
*/
void vector_set_zero(vector_t *const vector, const size_t index);


/**
* @brief   Duplicates existing element across range.
* @details Copies element at `index` across `amount` of elements including index.
  @verbatim
  | 0 | 1 | 2 | 3 |
  |   | X |       | index = 1
  |   | X | X | X | amount = 3
  @endverbatim
* @param[in] vector Pointer to a vector instance.
* @param[in] index  Index at which perform spread.
* @param[in] amount Length of the spread range. (1 - has no effect)
*/
void vector_spread(vector_t *const vector, const size_t index, const size_t amount);


/**
* @brief   Shift range of elements.
* @details Shifting @c length elements at @c offset by @c shift times in direction of a sign.
*          Data will be overriden by shifted range.
*          shift < 0 => left; shift > 0 => right;
*
* @param[in] vector Pointer to vector instance.
* @param[in] offset Offset in @ref vector_t::element_size "elements" (begin index).
* @param[in] length Size of the shifting range in elements.
* @param[in] shift  Direction and steps to shift in elements.
*/
void vector_shift(vector_t *const vector,
        const size_t offset,
        const size_t length,
        const ssize_t shift);


/**
* @brief Swaps values of elements designated by indicies.
* @warning index_a should differ from index_b
*
* @param[in] vector  Pointer to vector instance.
* @param[in] index_a Designator of the first element.
* @param[in] index_b Designarot of the second element.
*/
void vector_swap(vector_t *const vector,
        const size_t index_a,
        const size_t index_b);


/**
* @brief   Perform immutable action on each element of the vector.
* @details Run non-modifying @c func on @c limit elements of the vector.
*
* @param[in] vector     Pointer to vector instance.
* @param[in] limit      Limit maximum iterations.
* @param[in] func       Action to be performed.
* @param[in,out] param  User defined parameter, passed to func.
* @returns              Zero on success, or nonzero value - user defined status code.
*/
int vector_foreach(const vector_t *const vector,
        const size_t limit,
        const foreach_t func,
        void *const param);


/**
* @brief   Perform immutable accamulating action on each element of the vector.
* @details Run non-modifying function on @c limit elements of the vector reducing them into @c acc.
*          Return zero on success, or nonzero value - user defined status code.
*
* @param[in] vector     Pointer to vector instance.
* @param[in] limit      Limit maximum iterations.
* @param[in] func       Action to be performed.
* @param[out] acc       Accamulator that stores calculation result.
* @param[in,out] param  User defined parameter, passed to func.
* @returns              Zero on success, or nonzero value - user defined status code.
*/
int vector_aggregate(const vector_t *const vector,
        const size_t limit,
        const aggregate_t func,
        void *const acc,
        void *const param);


/**
* @brief   Perform mutable transformation on each element of the vector.
* @details Run modifying function on @c limit elements of the vector.
*
* @param[in] vector     Pointer to vector instance.
* @param[in] limit      Limit maximum iterations.
* @param[in] func       Action to be performed.
* @param[in,out] param  User defined parameter, passed to func.
* @returns              Zero on success, or nonzero value - user defined status code.
*/
int vector_transform(vector_t *const vector,
        const size_t limit,
        const transform_t func,
        void *const param);

/** @} @noop Elements */

/**
 * @addtogroup Allocation
 * @brief   Allocator functions.
 * @details These are @a weak @a symbols that can be overridden by the user.
 *          You can customize how you allocate underling memory for the vector.
 *          By passing @p param you can go even further and
 *          customize allocation process per vector instance. @{ */

/**
 * @brief Allocates memory chunk of \a alloc_size.
 *
 * @param[in]     alloc_size  Allocation size in bytes
 * @param[in,out] param       Optional allocator parameter. @see alloc_opts_t
 * @returns allocated memory chunk or @c NULL pointer on failure.
 */
void *vector_alloc(const size_t alloc_size, void *const param);


/**
 * @brief   Reallocates already allocated memory chunk in order to change allocation size.
 * @warning This operation can move whole chunk to a completely different location.
 *
 * @param[in]     ptr         Pointer to a memory chunk previously allocated with current allocator!
 * @param[in]     alloc_size  Allocation size in bytes
 * @param[in,out] param       Optional allocator parameter. @see alloc_opts_t
 * @returns allocated memory chunk or @c NULL pointer on failure.
 */
void *vector_realloc(void *ptr, const size_t alloc_size, void *const param);


/**
* @brief Free allocation that was previously allocated.
*
* @param[in]     ptr   Pointer to a memory chunk previously allocated with current allocator!
* @param[in,out] param Optional allocator parameter. @see alloc_opts_t
*/
void vector_free(void *ptr, void *const param);

/** @} @noop Allocation */

/**
* @addtogroup Utilities
* @brief Commonly used internally and by inherited classes @{ */

/**
* @brief Function calculates size of the element while respecting requirement for alignment.
*
* @param[in] size       Required data size
* @param[in] alignment  To multiple of which the size has to be aligned 
* @returns @c size aligned by @c alignment, where aligned size >= size
*/
size_t calc_aligned_size(const size_t size, const size_t alignment);

/**
* @brief Performs comparison in lexicographical ascending order.
*
* @see compare_t
*/
ssize_t cmp_lex_asc(const void *const value, const void *const element, void *const param);


/**
* @brief Performs comparison in lexicographical descending order.
*
* @see compare_t
*/
ssize_t cmp_lex_dsc(const void *const value, const void *const element, void *const param);

/** @} @noop Utilities */
/** @} @noop Vector_API */

#endif/*_VECTOR_H_*/
