#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_ALIGNMENT 2 * 1024 * 1024 // 4MB
#define ALIGNED(bytes) &(alloc_t){.alignment = bytes}

#if defined _WIN32 || defined __CYGWIN__
#define aligned_alloc(alignment, size) _aligned_malloc(size, alignment)
#endif

typedef struct alloc
{
    size_t alignment;
    size_t last_size;
}
alloc_t;

int main(void)
{
    vector_t *aligned_vector = vector_create (
        .element_size = sizeof(int),
        .alloc_opts = alloc_opts (
            .size = sizeof(alloc_t), 
            .data = ALIGNED(MAX_ALIGNMENT),
        ),
    );

    assert((0 == (size_t)aligned_vector % MAX_ALIGNMENT)
        && "Address must be aligned to MAX_ALIGNMENT");

    // ...

    vector_destroy(aligned_vector);
    return 0;
}

void *vector_alloc(const size_t alloc_size, void *const param)
{
    assert(param);
    alloc_t* alloc = (alloc_t*)param;
    return aligned_alloc(alloc->alignment, alloc_size);
}

void *vector_realloc(void *const ptr, size_t alloc_size, void *const param)
{
    assert(ptr);
    assert(alloc_size > 0);
    assert(param);

    alloc_t *alloc = (alloc_t*)param;

    if (alloc_size == alloc->last_size)
    {
        return ptr;
    }

    void *new = aligned_alloc(alloc->alignment, alloc_size);
    if (new)
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

