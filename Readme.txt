To compile, simply run:
1- cmake .
2- make
This will generate "a3_test" executable.
There will create "libsma.a" for the library file where it can be attached to other program.

No tests has been altered :)

A complete different approach was used for the assignment.
* The list contains all the blocks, free or not.
* The TAG consists of:
    - size of the block (int)
    - if it is free (int), overkill i know but this helped with byte addressability.
This helped solve memory alignment where it is 4 byte addressable.
