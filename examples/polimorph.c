#include "vector.h"

#include <stdio.h>

#define static_assert(cond, msg) enum __static_assert__{ _ = 1/((size_t)cond) }

//
// Add new types in that xmacro list:
//
#define TYPE_LIST \
    X(cat) \
    X(dog)

typedef enum type
{
#define X(type) TYPE_##type,
    TYPE_LIST
#undef X
    TYPE_LAST
}
type_t;

// forward declaration of type specific data structures
#define X(type) \
    typedef struct type##_data type##_data_t;

TYPE_LIST
#undef X

//
// Add type specific functions declarations here:
//
#define X(type) \
    void type##_create(void *const element, void *const param); \
    int type##_make_sound(const void *const element, void *const param); \

TYPE_LIST
#undef X

typedef struct object_header
{
    type_t type;
    // extend header with variables,
    // when shared data is required to store (a.k.a base class variables)

    char data[]; // store type specific data in flexarr
}
object_header_t;

//
// Add type specific data structures:
//
typedef struct cat_data
{
    const char *favorite_fish;
}
cat_data_t;

typedef struct dog_data
{
    const char *owners_name;
}
dog_data_t;

// union used to calculate max allocation size that fit any type.
union
{
#define X(type) type##_data_t type;
    TYPE_LIST
#undef X
}
data_t;

typedef int (*make_sound_t) (const void *const element, void *const param);

int make_sound(const void *const element, void *const param)
{
    // dynamic dispatch
    static make_sound_t table[TYPE_LAST] = {
#define X(type) type##_make_sound,
TYPE_LIST
#undef X
};

    const object_header_t* header = element;
    return table[header->type](element, param);
}


void dog_create(void *const element, void *const param)
{
    object_header_t *object = element;
    object->type = TYPE_dog;
    dog_data_t *data = (dog_data_t*)&object->data;
    *data = *(dog_data_t*)param;
}


void cat_create(void *const element, void *const param)
{
    object_header_t *object = element;
    object->type = TYPE_cat;
    cat_data_t *data = (cat_data_t*)&object->data;
    *data = *(cat_data_t*)param;
}


int cat_make_sound(const void *const element, void *const param)
{
    (void) element;
    (void) param;
    printf("Meaw\n");
    return 0;
}


int dog_make_sound(const void *const element, void *const param)
{
    (void) element;
    (void) param;
    printf("Woff\n");
    return 0;
}


int main()
{
    const size_t size = 10;
    vector_t *v = vector_create(.element_size = sizeof(data_t), .initial_cap = size);
    for (size_t i = 0; i < size; ++i)
    {

        if (i % 2 == 0)
        {
            dog_create(vector_get(v, i), &(dog_data_t){.owners_name = "John"});
            continue;
        }

        cat_create(vector_get(v, i), &(cat_data_t){.favorite_fish = "Salmon"});
    }

    (void) vector_foreach(v, size, make_sound, NULL);
    vector_destroy(v);

    return 0;
}
