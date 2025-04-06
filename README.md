# Allocators
This is a small library containing various commoddity memory allocators i'm mainly writing for myself so i don't guarantee about the quality of the software and the stability of the APIs.

## Actual state
At the moment the allocators supported are:<br>
-Arena<br>
-Pool<br>
refer to the relative folders for API informations

## Installation
Every allocator has the same project structure, so there is a single Makefile
used for compilation, the steps to use the code are the following:

include header you can find in the allocator's specific folder (ex. ./arena/arena.h) in your code and:
```sh
# to compile the allocator (then link your code against [ALLOCATOR]/build/[ALLOCATOR].a)
# specifing allocator's name with every letter in lowercase
make ALLOCATOR=*****

# to compile the code using debugging features use the following command
# and then link your program against .[ALLOCATOR]/build/[ALLOCATOR]_debug.a)
# (refer to specific allocators API to check the functionalities enabled in this way)

make debug ALLOCATOR=*****


# to clean build folder
make clean ALLOCATOR=*****
```

## Developer's utilities
The following commands are supposed to be used just by me during development and debugging
of this repo, but i'm writing them here just in case anyone wants to modify the code and could benefit
from the use of sanitizers

```sh

# run tests in [ALLOCATOR]/test folder
make test ALLOCATOR=*****  

# run tests enabling ASAN
make asan ALLOCATOR=*****

# run tests enabling ASAN
make valgrind ALLOCATOR=*****

```


## Platform supported
I'm aiming to make the code work both on Linux and Windows, but since i will mainly use it on Linux I don't guarantee that the code will work every time as expected.

Feel free to report bugs.

Thanks. 


