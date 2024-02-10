#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdbool.h> /* bool, true, false */
#include <stddef.h>  /* size_t */
#include <sys/types.h> /* ssize_t */

#define TMP_REF(type, value) (type[1]){value}

typedef struct vector_t vector_t;
typedef struct vector_opts_t
{
    size_t esize; /* size of the element */
    size_t initial_cap;
    float grow_factor;
    float grow_threshold;
    float shrink_threshold;
}
vector_opts_t;

/*
* Callback types
*/
typedef bool (*predicate_t) (const void *value, const void *element, void *param);
typedef int (*compare_t) (const void *value, const void *element, void *param);

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
        .esize = sizeof(int), \
        .initial_cap=10, \
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
* Sets element at given `index` to a `value`. May fail if index is invalid.
*/
bool vector_set(vector_t *vector, size_t index, const void *value);


/*
* Appends element at vector's tail, growing vector on demand.
* Updates provided pointer to a new one when relocation happen.
* (Allocation may fail, so returning operation status)
*/
bool vector_append_back(vector_t **vector, const void *value);


/*
* Removes element at tail of the vector, shrinking vector on demand.
* May update provided vector pointer.
* (Allocation may fail, so returning operation status)
*/
bool vector_pop_back(vector_t **vector);


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
* (Allocation may fail, so returning operation status)
*/
bool vector_pop_front(vector_t **vector);


/*
* Truncates vector to a desired capacity, wiping out rest of the elements.
* Capacity's lower limit is `initial_cap`.
* (Allocation may fail, so returning operation status)
*/
bool vector_truncate(vector_t **vector, size_t capacity);


/*
* Inserts new element at given `index`, shifting one element forward
* from that index.
* (Allocation may fail, so returning operation status)
*/
bool vector_insert_at(vector_t **vector, size_t index, const void *value);


/*
* Binary insert. Works only if previous elements
* that contained in the vector are ordered in same way.
* (Allocation may fail, so returning operation status)
*/
bool vector_binary_insert(vector_t **vector, compare_t cmp, const void *value, void *param, size_t *index);


/*
* Removes element at given `index`.
* May fail on realloc or if index is invalid.
*/
bool vector_remove(vector_t **vector, size_t index);

#endif/*_VECTOR_H_*/
