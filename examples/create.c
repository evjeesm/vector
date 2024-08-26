#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void default_create(void);
void explicit_opts_create(void);
void extended_header(void);
void cloning(void);

int main(void)
{
    default_create();
    explicit_opts_create();
    extended_header();
    cloning();

    return 0;
}

void default_create(void)
{
    vector_t *vector = vector_create(.element_size = sizeof(int));

    if (!vector)
    {
        perror("vector_create");
        abort();
    }

    // ...

    vector_destroy(vector);
}

void explicit_opts_create(void)
{
    vector_t *vector = vector_create
        (
            .element_size = sizeof(float),
            .initial_cap = 100
        );

    assert(vector && "Buy more RAM!");

    // ...

    vector_destroy(vector);
}

void extended_header(void)
{
    typedef struct
    {
        size_t meta;
        // ...
        // ...
        // ...
    }
    ext_t;

    vector_t *vector = vector_create
        (
            .data_offset = sizeof(ext_t),
            .element_size = sizeof(long)
        );

    // ...

    ext_t *header = vector_get_ext_header(vector);
    *header = (ext_t) {0};

    // ...

    vector_destroy(vector);
}

void cloning(void)
{
    vector_opts_t opts = {.element_size = sizeof(int)};
    vector_t *vector = vector_create_(&opts);

    vector_t *clone = vector_clone(vector);

    vector_destroy(vector);
    vector_destroy(clone);
}

