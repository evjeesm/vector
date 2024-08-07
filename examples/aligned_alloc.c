#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_ALIGNMENT 2 * 1024 * 1024 // 4MB
#define ALIGNED(bytes) *(void**)&(alloc_t){.alignment = bytes}

typedef struct alloc
{
    size_t alignment:22;
    size_t last_size:42;
}
alloc_t;
static_assert(sizeof(alloc_t) == sizeof(void *), "alloc_t must fit into alloc_param");


void *vector_alloc(const size_t alloc_size, void *const param)
{
    assert(param);
    alloc_t* alloc = (alloc_t*)&param;
    void *ptr = NULL;

    if (0 == posix_memalign(&ptr, alloc->alignment, alloc_size))
    {
        alloc->last_size = alloc_size;
    }

    return ptr;
}

void *vector_realloc(void *const ptr, size_t alloc_size, void *const param)
{
    assert(param);
    alloc_t *alloc = (alloc_t*)&param;
    void *new = NULL;

    if (0 == posix_memalign(&new, alloc->alignment, alloc_size))
    {
        memcpy(new, ptr, alloc->last_size);
        alloc->last_size = alloc_size;
        free(ptr);
    }
    return new;
}

void vector_free(void *const ptr, void *const param)
{
    (void) param;
    free(ptr);
}

int main(void)
{
    vector_t *aligned_vector = vector_create(
            .element_size = sizeof(int),
            .alloc_param = ALIGNED(MAX_ALIGNMENT)
        );

    assert((0 == (size_t)aligned_vector % MAX_ALIGNMENT)
        && "Address must be aligned to MAX_ALIGNMENT");

    // ...

    vector_destroy(aligned_vector);
    return 0;
}

