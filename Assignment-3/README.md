# Assignment 3

## To compile, simply run:
1. `cmake .`
1. `make`
This will generate `a3_test` executable.
There will create `libsma.a` for the library file where it can be attached to other program.


## Notes to Grader
A complete different approach was used for the assignment.
* No tests has been altered :)
* The list contains all the blocks, free or not.
* The TAG consists of:
    - size of the block (int)
    - if it is free (int), overkill i know but this helped with byte addressability.
This helped solve memory alignment where it is 4 byte addressable.
* Docs about functions are in sma.h. I put them there because it is easier to reference in an IDE.

## Issues During assignment
Indicate any issues you ran into doing this assignment.
* too many to list  them :'( This assignment took way too long.
* Tag system was hard
* Needed in-depth knowledge of pointer, addresses
* Advanced debugging skilled required such as opening in memory view
