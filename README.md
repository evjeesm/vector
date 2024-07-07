# Vector - extendable base for contiguous dynamic arrays.
Classic style library. (vector_t implementation is not exposed in the header file)

Provides facilities for implementing any kinds of random access containers.
Does not perform auto scaling and tracking of stored elements 
(all these functionalities have to be implemented in derived containers by design)

Also, memory alignment specifics are in total responsibility of the end user.
Uses `stdlib.h` allocation mechanism by default, but that behavior can be tweaked.

Compiles to *static* archive and/or *shared* object. 

## Supported platforms
- GNU Linux
- crossplatform compatability is not planned yet.

## Dependencies
**Build Tools**:
  - gcc
  - make
  - autotools:
    - automake >= 1.11.2
    - autoconf
    - autoconf-archive - install separately (for valgrind support)
    - libtool
  - check - testing framework
  - valgrind (optionally) - for memory leak checks

**Std Libraries**:
  - stdlib.h
  - string.h

## Building
- install **Build Tools** dependencies.
  - on **debian**:
    ```sh
    $ sudo apt-get install gcc make automake autoconf autoconf-archive libtool check valgrind
    ```
- clone the repository
  ```sh
  $ git clone https://githib.com/EvgeniSemenov/vector.git vector; cd vector
  ```
- run `./autogen.sh`
- run `./configure CFLAGS="<YOUR COMPILATION FLAGS>"` for instance `-Wall -Wextra -O3 -DNDEBUG`
- run `make check`
  whole thing will be compiled and tested with *check*
- compiled artefacts will be stored in `./src/.libs/`

Currently, I am not providing installation options.
The Main reason is that - as I am working in parallel on large hierarchy of libraries, 
its preferable for me to have a single source of truth, no discrepancy in versions
(change in one project automatically reflected in others), reduced compilation time.

So each library I use is stored once on my system and softlinked with other projects.

## Usage
link against `libvector_static.a` or `libvector.so`

### Minimal Example
```c
#include "vector.h"
int main(void)
{
    vector_t *vector = vector_create(.element_size = sizeof(int));
    int a = 69;
    vector_set(&vector, 0, &a);
    vector_set(&vector, 1, TMP_REF(int, 42));

    vector_get(vector, 1, &a); // a = 42
    vector_destroy(vector);
}
```
