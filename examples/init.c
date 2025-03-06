
#include "vector.h"


int main(void)
{
    vector_t *vector = vector_create_init_(
    VECTOR_OPTS(
        .element_size=sizeof(int),
        .initial_cap = 10,
    ),
    VECTOR_DATA(
        int,
        1, 2, 3
    )
    );

    vector_t *vector2 = vector_create_init(
        VECTOR_OPTS(),
        1, 2, 3
    );

    return 0;
}
