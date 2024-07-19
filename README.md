# Vector - extendable base for contiguous dynamic arrays.
Classic style library. (vector_t implementation is not exposed in the header file)

![check-linux](https://github.com/evjeesm/vector/actions/workflows/linux.yml/badge.svg)
![check-windows](https://github.com/evjeesm/vector/actions/workflows/windows.yml/badge.svg)

Provides facilities for implementing any kinds of random access containers.
Does not perform auto scaling and tracking of stored elements 
(all these functionalities have to be implemented in derived containers by design)

Also, memory alignment specifics are in total responsibility of the end user.
Uses `stdlib.h` allocation mechanism by default, but that behavior can be tweaked.

Compiles to *static* archive and/or *shared* object.

## Supported platforms
- GNU Linux
- Windows

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
- install **Build Tools** dependencies:
  on **debian**:
    ```sh
    $ sudo apt-get install gcc make automake autoconf autoconf-archive libtool check valgrind
    ```
- clone the repository:
  ```sh
  $ git clone https://githib.com/EvgeniSemenov/vector.git vector; cd vector;
  $ git submodules update --init --recursive;
  ```
- run `./autogen.sh` if you do any changes to `configuration.ac` file.
- run `./configure CFLAGS="<YOUR COMPILATION FLAGS>"` for instance `-Wall -Wextra -O3 -DNDEBUG`
- run `make check`, whole thing will be compiled and tested with *check*
- compiled artifacts will be stored in `./src/.libs/`.
  I do recommend using symlinks to that folder instead of installing library during development,
  this way I can be sure that all parts of my projects are synced.

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
